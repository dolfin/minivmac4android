/*
	MYOSGLUE.c

	Copyright (C) 2006 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	MY Operating System GLUE
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

IMPORTPROC EmulationReserveAlloc(void);
IMPORTFUNC blnr InitEmulation(void);
IMPORTPROC DoEmulateOneTick(void);
IMPORTFUNC blnr ScreenFindChanges(si3b TimeAdjust,
	si4b *top, si4b *left, si4b *bottom, si4b *right);
IMPORTPROC DoEmulateExtraTime(void);

GLOBALVAR ui3p ROM = nullpr;

#if IncludePbufs
GLOBALVAR ui5b PbufAllocatedMask;
GLOBALVAR ui5b PbufSize[NumPbufs];
#endif

GLOBALVAR ui5b vSonyWritableMask = 0;
GLOBALVAR ui5b vSonyInsertedMask = 0;

#if IncludeSonyRawMode
GLOBALVAR blnr vSonyRawMode = falseblnr;
#endif

#if IncludeSonyNew
GLOBALVAR blnr vSonyNewDiskWanted = falseblnr;
GLOBALVAR ui5b vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
GLOBALVAR tPbuf vSonyNewDiskName = NotAPbuf;
#endif

GLOBALVAR ui5b CurMacDateInSeconds = 0;
GLOBALVAR ui5b CurMacLatitude = 0;
GLOBALVAR ui5b CurMacLongitude = 0;
GLOBALVAR ui5b CurMacDelta = 0;

GLOBALVAR char *screencomparebuff = nullpr;

#if 0 != vMacScreenDepth
GLOBALVAR blnr UseColorMode = falseblnr;
#endif

#if 0 != vMacScreenDepth
GLOBALVAR blnr ColorMappingChanged = falseblnr;
#endif

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
GLOBALVAR ui4r CLUT_reds[CLUT_size];
GLOBALVAR ui4r CLUT_greens[CLUT_size];
GLOBALVAR ui4r CLUT_blues[CLUT_size];
#endif

GLOBALVAR MyEvtQEl MyEvtQA[MyEvtQSz];
GLOBALVAR ui4r MyEvtQIn = 0;
GLOBALVAR ui4r MyEvtQOut = 0;

LOCALVAR blnr RequestMacOff = falseblnr;

GLOBALVAR blnr ForceMacOff = falseblnr;

GLOBALVAR blnr WantMacInterrupt = falseblnr;

GLOBALVAR blnr WantMacReset = falseblnr;

GLOBALVAR blnr SpeedLimit = (WantInitSpeedValue != -1);

GLOBALVAR ui3b SpeedValue = WantInitSpeedValue;

GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;

#if EnableMouseMotion
LOCALVAR blnr HaveMouseMotion = falseblnr;
#endif

#if IncludePbufs
LOCALFUNC blnr FirstFreePbuf(tPbuf *r)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (! PbufIsAllocated(i)) {
			*r = i;
			return trueblnr;
		}
	}
	return falseblnr;
}
#endif

#if IncludePbufs
LOCALPROC PbufNewNotify(tPbuf Pbuf_No, ui5b count)
{
	PbufSize[Pbuf_No] = count;
	PbufAllocatedMask |= ((ui5b)1 << Pbuf_No);
}
#endif

#if IncludePbufs
LOCALPROC PbufDisposeNotify(tPbuf Pbuf_No)
{
	PbufAllocatedMask &= ~ ((ui5b)1 << Pbuf_No);
}
#endif

LOCALFUNC blnr FirstFreeDisk(tDrive *Drive_No)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (! vSonyIsInserted(i)) {
			*Drive_No = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALFUNC blnr AnyDiskInserted(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALPROC DiskInsertNotify(tDrive Drive_No, blnr locked)
{
	vSonyInsertedMask |= ((ui5b)1 << Drive_No);
	if (! locked) {
		vSonyWritableMask |= ((ui5b)1 << Drive_No);
	}
}

LOCALPROC DiskEjectedNotify(tDrive Drive_No)
{
	vSonyWritableMask &= ~ ((ui5b)1 << Drive_No);
	vSonyInsertedMask &= ~ ((ui5b)1 << Drive_No);
}

FORWARDPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right);

/* Draw the screen */
LOCALPROC Screen_Draw(si3b TimeAdjust)
{
	si4b top;
	si4b left;
	si4b bottom;
	si4b right;

	if (ScreenFindChanges(TimeAdjust,
		&top, &left, &bottom, &right))
	{
		HaveChangedScreenBuff(top, left, bottom, right);
	}

}

LOCALPROC SetLongs(ui5b *p, long n)
{
	long i;

	for (i = n; --i >= 0; ) {
		*p++ = (ui5b) -1;
	}
}

LOCALVAR uimr ReserveAllocOffset;
LOCALVAR ui3p ReserveAllocBigBlock = nullpr;

#define PowOf2(p) ((uimr)1 << (p))
#define Pow2Mask(p) (PowOf2(p) - 1)
#define FloorPow2Mult(i, p) ((i) & (~ Pow2Mask(p)))
#define CeilPow2Mult(i, p) FloorPow2Mult((i) + Pow2Mask(p), (p))
	/* warning - CeilPow2Mult evaluates p twice */

GLOBALPROC ReserveAllocOneBlock(ui3p *p, uimr n, ui3r align, blnr FillOnes)
{
	ReserveAllocOffset = CeilPow2Mult(ReserveAllocOffset, align);
	if (nullpr == ReserveAllocBigBlock) {
		*p = nullpr;
	} else {
		*p = ReserveAllocBigBlock + ReserveAllocOffset;
		if (FillOnes) {
			SetLongs((ui5b *)*p, n / 4);
		}
	}
	ReserveAllocOffset += n;
}

/* my event queue */

LOCALVAR blnr MyEvtQNeedRecover = falseblnr; /* events lost because of full queue */

LOCALFUNC MyEvtQEl * MyEvtQElPreviousIn(void)
{
	MyEvtQEl *p = NULL;
	if (MyEvtQIn - MyEvtQOut != 0) {
		p = &MyEvtQA[(MyEvtQIn - 1) & MyEvtQIMask];
	}

	return p;
}

LOCALFUNC MyEvtQEl * MyEvtQElAlloc(void)
{
	MyEvtQEl *p = NULL;
	if (MyEvtQIn - MyEvtQOut >= MyEvtQSz) {
		MyEvtQNeedRecover = trueblnr;
	} else {
		p = &MyEvtQA[MyEvtQIn & MyEvtQIMask];

		++MyEvtQIn;
	}

	return p;
}

LOCALVAR ui5b theKeys[4];

LOCALPROC Keyboard_UpdateKeyMap(int key, blnr down)
{
	int k = key & 127; /* just for safety */
	int bit = 1 << (k & 7);
	ui3b *kp = (ui3b *)theKeys;
	ui3b *kpi = &kp[k / 8];
	blnr CurDown = ((*kpi & bit) != 0);
	if (CurDown != down) {
		MyEvtQEl *p = MyEvtQElAlloc();
		if (NULL != p) {
			p->kind = MyEvtQElKindKey;
			p->u.press.key = k;
			p->u.press.down = down;

			if (down) {
				*kpi |= bit;
			} else {
				*kpi &= ~ bit;
			}
		}
	}
}

LOCALVAR blnr MyMouseButtonState = falseblnr;

LOCALPROC MyMouseButtonSet(blnr down)
{
	if (MyMouseButtonState != down) {
		MyEvtQEl *p = MyEvtQElAlloc();
		if (NULL != p) {
			p->kind = MyEvtQElKindMouseButton;
			p->u.press.down = down;

			MyMouseButtonState = down;
		}
	}
}

LOCALPROC MyMousePositionSetDelta(ui4r dh, ui4r dv)
{
	if ((dh != 0) || (dv != 0)) {
		MyEvtQEl *p = MyEvtQElPreviousIn();
		if ((NULL != p) && (MyEvtQElKindMouseDelta == p->kind)) {
			p->u.pos.h += dh;
			p->u.pos.v += dv;
		} else {
			p = MyEvtQElAlloc();
			if (NULL != p) {
				p->kind = MyEvtQElKindMouseDelta;
				p->u.pos.h = dh;
				p->u.pos.v = dv;
			}
		}
	}
}

LOCALVAR ui4b MyMousePosCurV = 0;
LOCALVAR ui4b MyMousePosCurH = 0;

LOCALPROC MyMousePositionSet(ui4r h, ui4r v)
{
	if ((h != MyMousePosCurH) || (v != MyMousePosCurV)) {
		MyEvtQEl *p = MyEvtQElPreviousIn();
		if ((NULL == p) || (MyEvtQElKindMousePos != p->kind)) {
			p = MyEvtQElAlloc();
		}
		if (NULL != p) {
			p->kind = MyEvtQElKindMousePos;
			p->u.pos.h = h;
			p->u.pos.v = v;

			MyMousePosCurH = h;
			MyMousePosCurV = v;
		}
	}
}

#if 0
#define Keyboard_TestKeyMap(key) ((((ui3b *)theKeys)[(key) / 8] & (1 << ((key) & 7))) != 0)
#endif

LOCALPROC InitKeyCodes(void)
{
	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;
}

#define kKeepMaskControl  (1 << 0)
#define kKeepMaskCapsLock (1 << 1)
#define kKeepMaskCommand  (1 << 2)
#define kKeepMaskOption   (1 << 3)
#define kKeepMaskShift    (1 << 4)

LOCALPROC DisconnectKeyCodes(ui5b KeepMask)
{
	/*
		Called when may miss key ups,
		so act is if all pressed keys have been released,
		except maybe for control, caps lock, command,
		option and shift.
	*/

	int j;
	int b;
	int key;
	ui5b m;

	for (j = 0; j < 16; ++j) {
		ui3b k1 = ((ui3b *)theKeys)[j];
		if (0 != k1) {
			ui3b bit = 1;
			for (b = 0; b < 8; ++b) {
				if (0 != (k1 & bit)) {
					key = j * 8 + b;
					switch (key) {
						case MKC_Control: m = kKeepMaskControl; break;
						case MKC_CapsLock: m = kKeepMaskCapsLock; break;
						case MKC_Command: m = kKeepMaskCommand; break;
						case MKC_Option: m = kKeepMaskOption; break;
						case MKC_Shift: m = kKeepMaskShift; break;
						default: m = 0; break;
					}
					if (0 == (KeepMask & m)) {
						Keyboard_UpdateKeyMap(key, falseblnr);
					}
				}
				bit <<= 1;
			}
		}
	}
}

LOCALPROC MyEvtQTryRecoverFromFull(void)
{
	MyMouseButtonSet(falseblnr);
	DisconnectKeyCodes(0);
}

#include "STRCONST.h"
