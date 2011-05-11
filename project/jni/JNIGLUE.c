/*
	JNIGLUE.c

	Copyright (C) 2009 Jesus A. Alvarez

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
	Java Native Interface GLUE
*/

#include <jni.h>

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#include <sys/time.h>
#include <stdlib.h>
#include "CNFGRAPI.h"
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"

#define kMacEpoch 2082844800
#define MyTickDuration (1/60.14742)
#define UsecPerSec 1000000
#define MyInvTimeStep 16626 /* UsecPerSec / 60.14742 */

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#undef ABS
#define ABS(x) (((x)>0)? (x) : -(x))

#undef CLAMP
#define CLAMP(x, lo, hi) (((x) > (hi))? (hi) : (((x) < (lo))? (lo) : (x)))

IMPORTFUNC blnr InitEmulation(void);
IMPORTPROC DoEmulateOneTick(void);
IMPORTFUNC blnr ScreenFindChanges(si3b TimeAdjust,
	si4b *top, si4b *left, si4b *bottom, si4b *right);
IMPORTPROC DoEmulateExtraTime(void);

GLOBALVAR char *screencomparebuff = nullpr;
GLOBALVAR ui3p RAM = nullpr;
GLOBALVAR ui3p ROM = nullpr;
GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;
GLOBALVAR ui3b CurMouseButton = falseblnr;

#if EnableMouseMotion
LOCALVAR blnr HaveMouseMotion = falseblnr;
#endif

LOCALVAR blnr RequestMacOff = falseblnr;
GLOBALVAR blnr ForceMacOff = falseblnr;
GLOBALVAR blnr WantMacInterrupt = falseblnr;
GLOBALVAR blnr WantMacReset = falseblnr;
GLOBALVAR ui5b vSonyWritableMask = 0;
GLOBALVAR ui5b vSonyInsertedMask = 0;
GLOBALVAR ui5b vSonyMountedMask = 0;

#if IncludeSonyRawMode
GLOBALVAR blnr vSonyRawMode = falseblnr;
#endif

#if IncludePbufs
GLOBALVAR ui5b PbufAllocatedMask;
GLOBALVAR ui5b PbufSize[NumPbufs];
#endif

#if IncludeSonyNew
GLOBALVAR blnr vSonyNewDiskWanted = falseblnr;
GLOBALVAR ui5b vSonyNewDiskSize;
#endif

#if IncludeSonyNameNew
GLOBALVAR ui4b vSonyNewDiskName = NotAPbuf;
#endif

GLOBALVAR ui5b CurMacDateInSeconds = 0;
GLOBALVAR ui5b CurMacLatitude = 0;
GLOBALVAR ui5b CurMacLongitude = 0;
GLOBALVAR ui5b CurMacDelta = 0;
LOCALVAR blnr SpeedStopped = 1;

ui5b MacDateDiff;
LOCALVAR ui5b TrueEmulatedTime = 0;
ui5b CurEmulatedTime = 0;
LOCALVAR ui5b OnTrueTime = 0;
LOCALVAR ui5b LastTimeSec, NextTimeSec;
LOCALVAR ui5b LastTimeUsec, NextTimeUsec;
LOCALVAR blnr initDone = falseblnr;


GLOBALVAR MyEvtQEl MyEvtQA[MyEvtQSz];
GLOBALVAR ui4r MyEvtQIn;
GLOBALVAR ui4r MyEvtQOut;

GLOBALVAR blnr SpeedLimit = trueblnr;
GLOBALVAR ui3b SpeedValue = 1;

// java
JNIEnv * jEnv;
jclass jClass;
jmethodID jSonyRead, jSonyWrite, jSonyGetSize, jSonyEject;
jmethodID jWarnMsg;

static jmethodID nativeCrashed, playSound;

#if IncludePbufs
LOCALFUNC blnr FirstFreePbuf(ui4b *r)
{
	si4b i;

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
LOCALPROC PbufNewNotify(ui4b Pbuf_No, ui5b count)
{
	PbufSize[Pbuf_No] = count;
	PbufAllocatedMask |= ((ui5b)1 << Pbuf_No);
}
#endif

#if IncludePbufs
LOCALPROC PbufDisposeNotify(ui4b Pbuf_No)
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

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
    memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

#if 0
#pragma mark -
#pragma mark Sound
#endif

#if MySoundEnabled
#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 4
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define kLnOneBuffLen 9
#define kLnAllBuffLen (kLn2SoundBuffers + kLnOneBuffLen)
#define kOneBuffLen (1UL << kLnOneBuffLen)
#define kAllBuffLen (1UL << kLnAllBuffLen)
#define kLnOneBuffSz (kLnOneBuffLen + kLn2SoundSampSz - 3)
#define kLnAllBuffSz (kLnAllBuffLen + kLn2SoundSampSz - 3)
#define kOneBuffSz (1UL << kLnOneBuffSz)
#define kAllBuffSz (1UL << kLnAllBuffSz)
#define kOneBuffMask (kOneBuffLen - 1)
#define kAllBuffMask (kAllBuffLen - 1)
#define dbhBufferSize (kAllBuffSz + kOneBuffSz)

LOCALVAR tpSoundSamp TheSoundBuffer = nullpr;
LOCALVAR ui4b ThePlayOffset = 0;
LOCALVAR ui4b TheFillOffset = 0;
LOCALVAR blnr wantplaying = falseblnr;
LOCALVAR ui4b MinFilledSoundBuffs = kSoundBuffers;
LOCALVAR ui4b TheWriteOffset = 0;

GLOBALPROC MySound_EndWrite(ui4r actL)
{
	TheWriteOffset += actL;

	if (0 == (TheWriteOffset & kOneBuffMask)) {
		/* just finished a block */

		if (wantplaying) {
			TheFillOffset = TheWriteOffset;

			(*jEnv)->CallStaticVoidMethod(jEnv, jClass, playSound);
		} else if (((TheWriteOffset - ThePlayOffset) >> kLnOneBuffLen) < 12) {
			/* just wait */
		} else {
			TheFillOffset = TheWriteOffset;
			wantplaying = trueblnr;

			(*jEnv)->CallStaticVoidMethod(jEnv, jClass, playSound);
		}
	}
}

GLOBALFUNC tpSoundSamp MySound_BeginWrite(ui4r n, ui4r *actL)
{
	ui4b ToFillLen = kAllBuffLen - (TheWriteOffset - ThePlayOffset);
	ui4b WriteBuffContig = kOneBuffLen - (TheWriteOffset & kOneBuffMask);

	if (WriteBuffContig < n) {
		n = WriteBuffContig;
	}
	if (ToFillLen < n) {
		/* overwrite previous buffer */
		TheWriteOffset -= kOneBuffLen;
	}

	*actL = n;
	return TheSoundBuffer + (TheWriteOffset & kAllBuffMask);
}

LOCALPROC MySound_SecondNotify(void)
{
	if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
		++CurEmulatedTime;
	} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
		--CurEmulatedTime;
	}
	MinFilledSoundBuffs = kSoundBuffers;
}

#endif


#if 0
#pragma mark -
#pragma mark Event Queue
#endif

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

#if 0
#pragma mark -
#pragma mark Floppy Driver
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    notifyDiskInserted
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskInserted (JNIEnv * env, jclass class, jint drive, jboolean locked) {
	DiskInsertNotify((ui4b)drive, locked?trueblnr:falseblnr);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    notifyDiskEjected
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskEjected (JNIEnv * env, jclass class, jint drive) {
	DiskEjectedNotify((ui4b)drive);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getFirstFreeDisk
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getFirstFreeDisk (JNIEnv * env, jclass class) {
	ui4b drive;
	if (!FirstFreeDisk(&drive)) return -1;
	return (jint)drive;
}

JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getNumDrives (JNIEnv * env, jclass class) {
	return (jint)NumDrives;
}

// callbacks
GLOBALFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	jobject jBuffer;
	jBuffer = (*jEnv)->NewDirectByteBuffer(jEnv, Buffer, (jlong)*Sony_Count);
	return (*jEnv)->CallStaticIntMethod(jEnv, jClass, jSonyRead, jBuffer, (jint)Drive_No, (jint)Sony_Start, (jint)*Sony_Count);
}

GLOBALFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	jobject jBuffer;
	jBuffer = (*jEnv)->NewDirectByteBuffer(jEnv, Buffer, (jlong)*Sony_Count);
	return (*jEnv)->CallStaticIntMethod(jEnv, jClass, jSonyWrite, jBuffer, (jint)Drive_No, (jint)Sony_Start, (jint)*Sony_Count);
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	*Sony_Count = (*jEnv)->CallStaticIntMethod(jEnv, jClass, jSonyGetSize, (jint)Drive_No);
	if (*Sony_Count < 0) return -1;
	return 0;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No) {
	return (*jEnv)->CallStaticIntMethod(jEnv, jClass, jSonyEject, (jint)Drive_No);
}

#if 0
#pragma mark -
#pragma mark Sound
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    soundBuf
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_name_osher_gil_minivmac_Core_soundBuf (JNIEnv * env, jclass class) {
#if MySoundEnabled
	ui3p NextPlayPtr;
	ui4b PlayNowSize = 0;
	ui4b MaskedFillOffset = ThePlayOffset & kOneBuffMask;

	if (MaskedFillOffset != 0) {
		/* take care of left overs */
		PlayNowSize = kOneBuffLen - MaskedFillOffset;
		NextPlayPtr = TheSoundBuffer + (ThePlayOffset & kAllBuffMask);
	} else if (0 != ((TheFillOffset - ThePlayOffset) >> kLnOneBuffLen)) {
		PlayNowSize = kOneBuffLen;
		NextPlayPtr = TheSoundBuffer + (ThePlayOffset & kAllBuffMask);
	}

	if (0 != PlayNowSize) {
		jbyteArray result = (*env)->NewByteArray(env, PlayNowSize);
		if (result == NULL) {
			 return NULL; /* out of memory error thrown */
		}
		(*env)->SetByteArrayRegion(env, result, 0, PlayNowSize , (jbyte*)(NextPlayPtr));
		return result;
	}
#endif
	return NULL;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setPlayOffset
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setPlayOffset (JNIEnv * env, jclass class, jint newValue) {
#if MySoundEnabled
	ThePlayOffset += newValue;
#endif
}


#if 0
#pragma mark -
#pragma mark Screen
#endif

#if 0 != vMacScreenDepth
GLOBALVAR blnr UseColorMode = falseblnr;
#endif

#if 0 != vMacScreenDepth
GLOBALVAR blnr ColorMappingChanged = falseblnr;
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    screenWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenWidth (JNIEnv * env, jclass class) {
	return (jint)vMacScreenWidth;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    screenHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenHeight (JNIEnv * env, jclass class) {
	return (jint)vMacScreenHeight;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getScreenUpdate
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_name_osher_gil_minivmac_Core_getScreenUpdate (JNIEnv * env, jclass class) {
	si4b top, left, bottom, right;
	
	if (!ScreenFindChanges(0, &top, &left, &bottom, &right)) return NULL;
	int changesWidth = right - left;
	int changesHeight = bottom - top;
	int changesSize = changesWidth * changesHeight;
	int i,x,y;
	
	// create java array of changes: top, left, bottom, right, pixels...
	jintArray jArray = (*jEnv)->NewIntArray(jEnv, changesSize+4);
	jboolean arrayCopy = JNI_FALSE;
	jint *arr = (jint*)(*jEnv)->GetPrimitiveArrayCritical(jEnv, (jarray)jArray, &arrayCopy);
	jint *px = &arr[4];
	
	// add coordinates
	arr[0] = (jint)top;
	arr[1] = (jint)left;
	arr[2] = (jint)bottom;
	arr[3] = (jint)right;
	
	// convert pixels
	x = left;
	y = top*vMacScreenByteWidth;
	for(i=0; i < changesSize; i++) {
		int pixel = ((((unsigned char*)screencomparebuff)[y+(x/8)]) << (x%8)) & 0x80;
		px[i] = pixel?BLACK:WHITE;
		if (++x >= right) {
			x = left;
			y += vMacScreenByteWidth;
		}
	}
	
	(*jEnv)->ReleasePrimitiveArrayCritical(jEnv, (jarray)jArray, (void*)px, 0);
	return jArray;
}

#if 0
#pragma mark -
#pragma mark Mouse
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    moveMouse
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_moveMouse (JNIEnv * env, jclass class, jint dx, jint dy) {
	HaveMouseMotion = trueblnr;
	MyMousePositionSetDelta(dx, dy);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setMousePos
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setMousePos (JNIEnv * env, jclass class, jint x, jint y) {
	HaveMouseMotion = falseblnr;
	CurMouseH = CLAMP(x, 0, vMacScreenWidth);
	CurMouseV = CLAMP(y, 0, vMacScreenHeight);

	MyMousePositionSet(CurMouseH, CurMouseV);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setMouseButton
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setMouseButton (JNIEnv * env, jclass class, jboolean down) {
	CurMouseButton = down?trueblnr:falseblnr;
	MyMouseButtonSet(CurMouseButton);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseX
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getMouseX (JNIEnv * env, jclass class) {
	return (jint)CurMouseH;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseY
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getMouseY (JNIEnv * env, jclass class) {
	return (jint)CurMouseV;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseButton
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_getMouseButton (JNIEnv * env, jclass class) {
	return CurMouseButton?JNI_TRUE:JNI_FALSE;
}

#if 0
#pragma mark -
#pragma mark Keyboard
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setKeyDown
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setKeyDown (JNIEnv * env, jclass class, jint key) {
	Keyboard_UpdateKeyMap(key, trueblnr);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setKeyUp
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setKeyUp (JNIEnv * env, jclass class, jint key) {
	Keyboard_UpdateKeyMap(key, falseblnr);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    isKeyDown
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_isKeyDown (JNIEnv * env, jclass class, jint key) {
	ui3b *kp = (ui3b *)theKeys;
	
	if (key < 0 || key >= 128) return JNI_FALSE;
	int bit = 1 << (key & 7);
	return (kp[key / 8] & bit)?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    keysDown
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_name_osher_gil_minivmac_Core_keysDown (JNIEnv * env, jclass class) {
	ui3b *kp = (ui3b *)theKeys;
	unsigned char keys[128];
	int key;
	jsize numKeys = 0;
	
	// get array of keys that are down
	for(key = 0; key < 128; key++) {
		int bit = 1 << (key & 7);
		if (kp[key / 8] & bit)
			keys[numKeys++] = key;
	}
	
	// convert to java array
	jbyteArray jArray = (*env)->NewByteArray(env, numKeys);
	(*env)->SetByteArrayRegion(env, jArray, 0, numKeys, keys);
	return jArray;
}

#if 0
#pragma mark -
#pragma mark Warnings
#endif

GLOBALPROC WarnMsgUnsupportedROM(void) {
	(*jEnv)->CallStaticVoidMethod(jEnv, jClass, jWarnMsg, (jint)1, NULL);
}

#if DetailedAbormalReport
GLOBALPROC WarnMsgAbnormal(char *s)
{
	jstring msg = (*jEnv)->NewStringUTF(jEnv, s);
	(*jEnv)->CallStaticVoidMethod(jEnv, jClass, jWarnMsg, (jint)3, msg);
}
#else
GLOBALPROC WarnMsgAbnormal(void)
{
	(*jEnv)->CallStaticVoidMethod(jEnv, jClass, jWarnMsg, (jint)3, NULL);
}
#endif

GLOBALPROC WarnMsgCorruptedROM(void)
{
	(*jEnv)->CallStaticVoidMethod(jEnv, jClass, jWarnMsg, (jint)2, NULL);
}

#if 0
#pragma mark -
#pragma mark Emulation
#endif

LOCALPROC IncrNextTime(void)
{
    /* increment NextTime by one tick */
    NextTimeUsec += MyInvTimeStep;
    if (NextTimeUsec >= UsecPerSec) {
        NextTimeUsec -= UsecPerSec;
        NextTimeSec += 1;
    }
}

LOCALPROC InitNextTime(void)
{
    NextTimeSec = LastTimeSec;
    NextTimeUsec = LastTimeUsec;
    IncrNextTime();
}

LOCALPROC GetCurrentTicks(void)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    LastTimeSec = (ui5b)t.tv_sec;
    LastTimeUsec = (ui5b)t.tv_usec;
}

void StartUpTimeAdjust (void)
{
    GetCurrentTicks();
    InitNextTime();
}

LOCALFUNC si5b GetTimeDiff(void)
{
    return ((si5b)(LastTimeSec - NextTimeSec)) * UsecPerSec
        + ((si5b)(LastTimeUsec - NextTimeUsec));
}

LOCALFUNC blnr CheckDateTime (void)
{
    ui5b NewMacDate = time(NULL) + MacDateDiff;
    if (NewMacDate != CurMacDateInSeconds) {
        CurMacDateInSeconds = NewMacDate;
        return trueblnr;
    }
    return falseblnr;
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
    si5b TimeDiff;
    
    GetCurrentTicks();
    
    TimeDiff = GetTimeDiff();
    if (TimeDiff >= 0) {
        if (TimeDiff > 4 * MyInvTimeStep) {
            /* emulation interrupted, forget it */
            ++TrueEmulatedTime;
            InitNextTime();
        } else {
            do {
                ++TrueEmulatedTime;
                IncrNextTime();
                TimeDiff -= UsecPerSec;
            } while (TimeDiff >= 0);
        }
    } else if (TimeDiff < - 2 * MyInvTimeStep) {
        /* clock goofed if ever get here, reset */
        InitNextTime();
    }
}

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
    UpdateTrueEmulatedTime();
    return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	si3b n;

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
	}
    
    n = OnTrueTime - CurEmulatedTime;
    if (n > 0) {
        if (n > 8) {
            /* emulation not fast enough */
            n = 8;
            CurEmulatedTime = OnTrueTime - n;
        }
        
        do {
            DoEmulateOneTick();
            ++CurEmulatedTime;
        } while (ExtraTimeNotOver() && (--n > 0));
    }
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setWantMacReset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setWantMacReset (JNIEnv * env, jclass class) {
	WantMacReset = trueblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setWantMacReset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setWantMacInterrupt (JNIEnv * env, jclass class) {
	WantMacInterrupt = trueblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    runTick
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_runTick (JNIEnv * env, jclass class) {
	jEnv = env;
	jClass = class;
	
	if (SpeedStopped) return;
	UpdateTrueEmulatedTime();
	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _resumeEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1resumeEmulation (JNIEnv * env, jclass class) {
	StartUpTimeAdjust();
	SpeedStopped = 0;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _pauseEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1pauseEmulation (JNIEnv * env, jclass class) {
	SpeedStopped = 1;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    isPaused
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_isPaused (JNIEnv * env, jclass class) {
	return SpeedStopped?JNI_TRUE:JNI_FALSE;
}

#if 0
#pragma mark -
#pragma mark Misc
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    init
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_init (JNIEnv * env, jclass this, jobject romBuffer) {
	void *romData;
	size_t romSize;
	struct timeval tv;
	struct timezone tz;
	
	if (initDone) return JNI_FALSE;
	
	// load ROM
	romData = (*env)->GetDirectBufferAddress(env, romBuffer);
	romSize = (*env)->GetDirectBufferCapacity(env, romBuffer);
	ROM = malloc(romSize);
	if (ROM == NULL) goto fail;
	memcpy(ROM, romData, romSize);
	
	// alocate RAM, screen buffer and sound buffer
	RAM = malloc(0x00400004);
	if (RAM == NULL) goto fail;
	screencomparebuff = malloc(vMacScreenNumBytes);
	if (screencomparebuff == NULL) goto fail;
#if MySoundEnabled
	TheSoundBuffer = malloc(dbhBufferSize);
	if (TheSoundBuffer == NULL) goto fail;
#endif
	
	// init location
	gettimeofday(&tv, &tz);
	CurMacDelta = tz.tz_minuteswest / 60;
	MacDateDiff = kMacEpoch + (tz.tz_minuteswest*60);
	CurMacDateInSeconds = tv.tv_sec + MacDateDiff;
	
	// get java method IDs
	jSonyRead = (*env)->GetStaticMethodID(env, this, "sonyRead", "(Ljava/nio/ByteBuffer;III)I");
	jSonyWrite = (*env)->GetStaticMethodID(env, this, "sonyWrite", "(Ljava/nio/ByteBuffer;III)I");
	jSonyGetSize = (*env)->GetStaticMethodID(env, this, "sonyGetSize", "(I)I");
	jSonyEject = (*env)->GetStaticMethodID(env, this, "sonyEject", "(I)I");
	jWarnMsg = (*env)->GetStaticMethodID(env, this, "warnMsg", "(ILjava/lang/String;)V");
	
	if (!InitEmulation()) goto fail;
	
	// initialize fields
	jfieldID sDiskPath, sDiskFile, sNumInsertedDisks, sInitOk;
	sDiskPath = (*env)->GetStaticFieldID(env, this, "diskPath", "[Ljava/lang/String;");
	sDiskFile = (*env)->GetStaticFieldID(env, this, "diskFile", "[Ljava/io/RandomAccessFile;");
	sNumInsertedDisks = (*env)->GetStaticFieldID(env, this, "numInsertedDisks", "I");
	sInitOk = (*env)->GetStaticFieldID(env, this, "initOk", "Z");
	
	// init drives
	jobjectArray diskPath = (*env)->NewObjectArray(env, NumDrives, (*env)->FindClass(env, "java/lang/String"), NULL);
	jobjectArray diskFile = (*env)->NewObjectArray(env, NumDrives, (*env)->FindClass(env, "java/io/RandomAccessFile"), NULL);
	(*env)->SetStaticIntField(env, this, sNumInsertedDisks, 0);
	(*env)->SetStaticObjectField(env, this, sDiskPath, diskPath);
	(*env)->SetStaticObjectField(env, this, sDiskFile, diskFile);
	
	// init ok
	(*env)->SetStaticBooleanField(env, this, sInitOk, JNI_TRUE);
	initDone = trueblnr;
	return JNI_TRUE;
fail:
	if (ROM) free(ROM);
	if (RAM) free(RAM);
	if (screencomparebuff) free(screencomparebuff);
#if MySoundEnabled
	if (TheSoundBuffer) free(TheSoundBuffer);
#endif
	return JNI_FALSE;
}

static struct sigaction old_sa[NSIG];

void android_sigaction(int signal, siginfo_t *info, void *reserved)
{
	(*jEnv)->CallStaticVoidMethod(jEnv, jClass, nativeCrashed);
	old_sa[signal].sa_handler(signal);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
	if ((*jvm)->GetEnv(jvm, (void **)&jEnv, JNI_VERSION_1_2)) return JNI_ERR;
	jClass = (*jEnv)->FindClass(jEnv, "name/osher/gil/minivmac/Core");

	nativeCrashed = (*jEnv)->GetStaticMethodID(jEnv, jClass, "nativeCrashed", "()V");
	playSound = (*jEnv)->GetStaticMethodID(jEnv, jClass, "playSound", "()V");


	// Try to catch crashes...
	struct sigaction handler;
	memset(&handler, 0, sizeof(sigaction));
	handler.sa_sigaction = android_sigaction;
	handler.sa_flags = SA_RESETHAND;
	#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
	CATCHSIG(SIGILL);
	CATCHSIG(SIGABRT);
	CATCHSIG(SIGBUS);
	CATCHSIG(SIGFPE);
	CATCHSIG(SIGSEGV);
	CATCHSIG(SIGSTKFLT);
	CATCHSIG(SIGPIPE);

	return JNI_VERSION_1_2;
}
