/*
	JNIGLUE.c

	Copyright (C) 2012 Gil Osher

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

#include <time.h>
#include <stdlib.h>
#include "CNFGRAPI.h"
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"
#include "STRCONST.h"
#include "COMOSGLU.h"

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#undef ABS
#define ABS(x) (((x)>0)? (x) : -(x))

#undef CLAMP
#define CLAMP(x, lo, hi) (((x) > (hi))? (hi) : (((x) < (lo))? (lo) : (x)))

IMPORTFUNC blnr InitEmulation(void);
IMPORTPROC DoEmulateOneTick(void);
IMPORTFUNC blnr ScreenFindChanges(ui3p screencurrentbuff,
		si3b TimeAdjust, si4b *top, si4b *left, si4b *bottom, si4b *right);
IMPORTPROC DoEmulateExtraTime(void);

LOCALVAR blnr CurSpeedStopped = trueblnr;

GLOBALVAR ui3b CurMouseButton = falseblnr;

LOCALVAR blnr initDone = falseblnr;

// java
JNIEnv * jEnv;
jclass jClass;
jmethodID jSonyTransfer, jSonyGetSize, jSonyEject;
jmethodID jWarnMsg;

static jmethodID nativeCrashed, playSound;

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
    memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- control mode and internationalization --- */

#define NeedCell2PlainAsciiMap 1

#include "INTLCHAR.h"

#if 0
#pragma mark -
#pragma mark Time, Date, Location
#endif

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

#include "DATE2SEC.h"

#define TicksPerSecond 1000000

LOCALVAR blnr HaveTimeDelta = falseblnr;
LOCALVAR ui5b TimeDelta;

LOCALVAR ui5b NewMacDateInSeconds;

LOCALVAR ui5b LastTimeSec;
LOCALVAR ui5b LastTimeUsec;

LOCALPROC GetCurrentTicks(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	if (! HaveTimeDelta) {
		time_t Current_Time;
		struct tm *s;

		(void) time(&Current_Time);
		s = localtime(&Current_Time);
		TimeDelta = Date2MacSeconds(s->tm_sec, s->tm_min, s->tm_hour,
			s->tm_mday, 1 + s->tm_mon, 1900 + s->tm_year) - t.tv_sec;

		HaveTimeDelta = trueblnr;
	}

	NewMacDateInSeconds = t.tv_sec + TimeDelta;
	LastTimeSec = (ui5b)t.tv_sec;
	LastTimeUsec = (ui5b)t.tv_usec;
}

#define MyInvTimeStep 16626 /* TicksPerSecond / 60.14742 */

LOCALVAR ui5b NextTimeSec;
LOCALVAR ui5b NextTimeUsec;

LOCALPROC IncrNextTime(void)
{
	NextTimeUsec += MyInvTimeStep;
	if (NextTimeUsec >= TicksPerSecond) {
		NextTimeUsec -= TicksPerSecond;
		NextTimeSec += 1;
	}
}

LOCALPROC InitNextTime(void)
{
	NextTimeSec = LastTimeSec;
	NextTimeUsec = LastTimeUsec;
	IncrNextTime();
}

LOCALPROC StartUpTimeAdjust(void)
{
	GetCurrentTicks();
	InitNextTime();
}

LOCALFUNC si5b GetTimeDiff(void)
{
	return ((si5b)(LastTimeSec - NextTimeSec)) * TicksPerSecond
		+ ((si5b)(LastTimeUsec - NextTimeUsec));
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
				TimeDiff -= TicksPerSecond;
			} while (TimeDiff >= 0);
		}
	} else if (TimeDiff < - 2 * MyInvTimeStep) {
		/* clock goofed if ever get here, reset */
		InitNextTime();
	}
}

LOCALFUNC blnr CheckDateTime(void)
{
	if (CurMacDateInSeconds != NewMacDateInSeconds) {
		CurMacDateInSeconds = NewMacDateInSeconds;
		return trueblnr;
	} else {
		return falseblnr;
	}
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
#pragma mark Paramter buffers
#endif

#include "CONTROLM.h"

/* --- parameter buffers --- */

#if IncludePbufs
LOCALVAR void *PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromPtr(void *p, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		free(p);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = p;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	tMacErr err = mnvm_miscErr;

	void *p = calloc(1, count);
	if (NULL != p) {
		err = PbufNewFromPtr(p, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
	free(PbufDat[i]);
	PbufDisposeNotify(i);
}
#endif

#if IncludePbufs
LOCALPROC UnInitPbufs(void)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#endif

#if IncludePbufs
GLOBALPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	void *p = ((ui3p)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
}
#endif

#if 0
#pragma mark -
#pragma mark Text translation
#endif

/* this is table for Windows, any changes needed for X? */
LOCALVAR const ui3b Native2MacRomanTab[] = {
	0xAD, 0xB0, 0xE2, 0xC4, 0xE3, 0xC9, 0xA0, 0xE0,
	0xF6, 0xE4, 0xB6, 0xDC, 0xCE, 0xB2, 0xB3, 0xB7,
	0xB8, 0xD4, 0xD5, 0xD2, 0xD3, 0xA5, 0xD0, 0xD1,
	0xF7, 0xAA, 0xC5, 0xDD, 0xCF, 0xB9, 0xC3, 0xD9,
	0xCA, 0xC1, 0xA2, 0xA3, 0xDB, 0xB4, 0xBA, 0xA4,
	0xAC, 0xA9, 0xBB, 0xC7, 0xC2, 0xBD, 0xA8, 0xF8,
	0xA1, 0xB1, 0xC6, 0xD7, 0xAB, 0xB5, 0xA6, 0xE1,
	0xFC, 0xDA, 0xBC, 0xC8, 0xDE, 0xDF, 0xF0, 0xC0,
	0xCB, 0xE7, 0xE5, 0xCC, 0x80, 0x81, 0xAE, 0x82,
	0xE9, 0x83, 0xE6, 0xE8, 0xED, 0xEA, 0xEB, 0xEC,
	0xF5, 0x84, 0xF1, 0xEE, 0xEF, 0xCD, 0x85, 0xF9,
	0xAF, 0xF4, 0xF2, 0xF3, 0x86, 0xFA, 0xFB, 0xA7,
	0x88, 0x87, 0x89, 0x8B, 0x8A, 0x8C, 0xBE, 0x8D,
	0x8F, 0x8E, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95,
	0xFD, 0x96, 0x98, 0x97, 0x99, 0x9B, 0x9A, 0xD6,
	0xBF, 0x9D, 0x9C, 0x9E, 0x9F, 0xFE, 0xFF, 0xD8
};

LOCALFUNC tMacErr NativeTextToMacRomanPbuf(char *x, tPbuf *r)
{
	if (NULL == x) {
		return mnvm_miscErr;
	} else {
		ui3p p;
		ui5b L = strlen(x);

		p = (ui3p)malloc(L);
		if (NULL == p) {
			return mnvm_miscErr;
		} else {
			ui3b *p0 = (ui3b *)x;
			ui3b *p1 = (ui3b *)p;
			int i;

			for (i = L; --i >= 0; ) {
				ui3b v = *p0++;
				if (v >= 128) {
					v = Native2MacRomanTab[v - 128];
				} else if (10 == v) {
					v = 13;
				}
				*p1++ = v;
			}

			return PbufNewFromPtr(p, L, r);
		}
	}
}

/* this is table for Windows, any changes needed for X? */
LOCALVAR const ui3b MacRoman2NativeTab[] = {
	0xC4, 0xC5, 0xC7, 0xC9, 0xD1, 0xD6, 0xDC, 0xE1,
	0xE0, 0xE2, 0xE4, 0xE3, 0xE5, 0xE7, 0xE9, 0xE8,
	0xEA, 0xEB, 0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF3,
	0xF2, 0xF4, 0xF6, 0xF5, 0xFA, 0xF9, 0xFB, 0xFC,
	0x86, 0xB0, 0xA2, 0xA3, 0xA7, 0x95, 0xB6, 0xDF,
	0xAE, 0xA9, 0x99, 0xB4, 0xA8, 0x80, 0xC6, 0xD8,
	0x81, 0xB1, 0x8D, 0x8E, 0xA5, 0xB5, 0x8A, 0x8F,
	0x90, 0x9D, 0xA6, 0xAA, 0xBA, 0xAD, 0xE6, 0xF8,
	0xBF, 0xA1, 0xAC, 0x9E, 0x83, 0x9A, 0xB2, 0xAB,
	0xBB, 0x85, 0xA0, 0xC0, 0xC3, 0xD5, 0x8C, 0x9C,
	0x96, 0x97, 0x93, 0x94, 0x91, 0x92, 0xF7, 0xB3,
	0xFF, 0x9F, 0xB9, 0xA4, 0x8B, 0x9B, 0xBC, 0xBD,
	0x87, 0xB7, 0x82, 0x84, 0x89, 0xC2, 0xCA, 0xC1,
	0xCB, 0xC8, 0xCD, 0xCE, 0xCF, 0xCC, 0xD3, 0xD4,
	0xBE, 0xD2, 0xDA, 0xDB, 0xD9, 0xD0, 0x88, 0x98,
	0xAF, 0xD7, 0xDD, 0xDE, 0xB8, 0xF0, 0xFD, 0xFE
};

LOCALFUNC blnr MacRomanTextToNativePtr(tPbuf i, blnr IsFileName,
	ui3p *r)
{
	ui3p p;
	void *Buffer = PbufDat[i];
	ui5b L = PbufSize[i];

	p = (ui3p)malloc(L + 1);
	if (p != NULL) {
		ui3b *p0 = (ui3b *)Buffer;
		ui3b *p1 = (ui3b *)p;
		int j;

		if (IsFileName) {
			for (j = L; --j >= 0; ) {
				ui3b x = *p0++;
				if (x < 32) {
					x = '-';
				} else if (x >= 128) {
					x = MacRoman2NativeTab[x - 128];
				} else {
					switch (x) {
						case '/':
						case '<':
						case '>':
						case '|':
						case ':':
							x = '-';
						default:
							break;
					}
				}
				*p1++ = x;
			}
			if ('.' == p[0]) {
				p[0] = '-';
			}
		} else {
			for (j = L; --j >= 0; ) {
				ui3b x = *p0++;
				if (x >= 128) {
					x = MacRoman2NativeTab[x - 128];
				} else if (13 == x) {
					x = '\n';
				}
				*p1++ = x;
			}
		}
		*p1 = 0;

		*r = p;
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC NativeStrFromCStr(char *r, char *s)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	r[L] = 0;
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
GLOBALFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count, ui5r *Sony_ActCount)
{
	jobject jBuffer;
	jBuffer = (*jEnv)->NewDirectByteBuffer(jEnv, Buffer, (jlong)Sony_Count);
	ui5r actCount = (*jEnv)->CallStaticIntMethod(jEnv, jClass, jSonyTransfer, (jboolean)IsWrite, jBuffer, (jint)Drive_No, (jint)Sony_Start, (jint)Sony_Count);

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = actCount;
	}

	return (actCount >= 0 ? mnvm_noErr : -1);
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

LOCALVAR blnr WasInSpecialMode = falseblnr;

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
 * Method:    screenDepth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenDepth (JNIEnv * env, jclass class) {
	return (jint)vMacScreenDepth;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getScreenUpdate
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_name_osher_gil_minivmac_Core_getScreenUpdate (JNIEnv * env, jclass class) {
	si4b top, left, bottom, right;

	if (0 != SpecialModes) {
		top = 0;
		left = 0;
		bottom = vMacScreenHeight;
		right = vMacScreenWidth;
		WasInSpecialMode = trueblnr;
	} else if (WasInSpecialMode) {
		top = 0;
		left = 0;
		bottom = vMacScreenHeight;
		right = vMacScreenWidth;
		WasInSpecialMode = falseblnr;
	} else if (ScreenChangedBottom > ScreenChangedTop) {
		top = ScreenChangedTop;
		left = ScreenChangedLeft;
		bottom = ScreenChangedBottom;
		right = ScreenChangedRight;
	} else {
		// No change - return empty array.
		jintArray jArray = (*jEnv)->NewIntArray(jEnv, 0);
		return jArray;
	}

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

	ui3p curdrawbuff = GetCurDrawBuff();

	// convert pixels
#if 0 != vMacScreenDepth
	if (UseColorMode) {
#if 4 > vMacScreenDepth
    	x = left;
    	y = top*vMacScreenByteWidth;
    	for(i=0; i < changesSize; i++) {
    		int pixel = (((unsigned char*)curdrawbuff)[y+x]);
    		px[i] = (0xFF000000 |
    				 ((((unsigned int)CLUT_reds[pixel]  ) >> 8) << 16) |
    				 ((((unsigned int)CLUT_greens[pixel]) >> 8) << 8 ) |
    				 ((((unsigned int)CLUT_blues[pixel] ) >> 8)		 ));

    		if (++x >= right) {
    			x = left;
    			y += vMacScreenByteWidth;
    		}
    	}
#else
    	x = left;
    	y = top*vMacScreenByteWidth;
    	for(i=0; i < changesSize; i++) {
    		int pixel = ((unsigned char*)curdrawbuff)[y+x];
    		px[i] = pixel;//((pixel & 0xFF00) >> 8) | ((pixel & 0x00FF) << 8);

    		if (++x >= right) {
    			x = left;
    			y += vMacScreenByteWidth;
    		}
    	}
#endif
    } else {
#endif
    	x = left;
    	y = top*vMacScreenMonoByteWidth;
    	for(i=0; i < changesSize; i++) {
    		int pixel = ((((unsigned char*)curdrawbuff)[y+(x/8)]) << (x%8)) & 0x80;
    		px[i] = pixel?BLACK:WHITE;

    		if (++x >= right) {
    			x = left;
    			y += vMacScreenMonoByteWidth;
    		}
    	}
#if 0 != vMacScreenDepth
    }
#endif

	ScreenClearChanges();

	(*jEnv)->ReleasePrimitiveArrayCritical(jEnv, (jarray)jArray, (void*)arr, 0);
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
	Keyboard_UpdateKeyMap2(key, trueblnr);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setKeyUp
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setKeyUp (JNIEnv * env, jclass class, jint key) {
	Keyboard_UpdateKeyMap2(key, falseblnr);
}

#if 0
#pragma mark -
#pragma mark Basic Dialogs
#endif

LOCALPROC CheckSavedMacMsg(void)
{
	/* called only on quit, if error saved but not yet reported */

	if (nullpr != SavedBriefMsg) {
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

		(*jEnv)->CallStaticVoidMethod(jEnv, jClass, jWarnMsg, SavedBriefMsg, SavedLongMsg);

		SavedBriefMsg = nullpr;
	}
}

#if 0
#pragma mark -
#pragma mark Emulation
#endif

LOCALVAR ui5b OnTrueTime = 0;

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

/* --- platform independent code can be thought of as going here --- */

#include "PROGMAIN.h"

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	si3b n = OnTrueTime - CurEmulatedTime;

	if (n > 0) {
		if (CheckDateTime()) {
#if MySoundEnabled
			MySound_SecondNotify();
#endif
		}

#if UseMotionEvents
		if (! CaughtMouse)
#endif
		{
			//CheckMouseState();
		}

		DoEmulateOneTick();
		++CurEmulatedTime;

#if EnableMouseMotion && MayFullScreen
		if (HaveMouseMotion) {
			AutoScrollScreen();
		}
#endif
		//MyDrawChangesAndClear();

		if (ExtraTimeNotOver() && (--n > 0)) {
			/* lagging, catch up */

			if (n > 8) {
				/* emulation not fast enough */
				n = 8;
				CurEmulatedTime = OnTrueTime - n;
			}

			EmVideoDisable = trueblnr;

			do {
				DoEmulateOneTick();
				++CurEmulatedTime;
			} while (ExtraTimeNotOver()
				&& (--n > 0));

			EmVideoDisable = falseblnr;
		}

		EmLagTime = n;
	}
}

LOCALPROC RunOnEndOfSixtieth(void)
{
	while (ExtraTimeNotOver()) {
		struct timespec rqt;
		struct timespec rmt;

		si5b TimeDiff = GetTimeDiff();
		if (TimeDiff < 0) {
			rqt.tv_sec = 0;
			rqt.tv_nsec = (- TimeDiff) * 1000;
			(void) nanosleep(&rqt, &rmt);
		}
	}

	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
}

LOCALPROC ReserveAllocAll(void)
{
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, falseblnr);
#if MySoundEnabled
	ReserveAllocOneBlock((ui3p *)&TheSoundBuffer,
		dbhBufferSize, 5, falseblnr);
#endif

	EmulationReserveAlloc();
}

LOCALFUNC blnr AllocMyMemory(void)
{
	uimr n;
	blnr IsOk = falseblnr;

	ReserveAllocOffset = 0;
	ReserveAllocBigBlock = nullpr;
	ReserveAllocAll();
	n = ReserveAllocOffset;
	ReserveAllocBigBlock = (ui3p)calloc(1, n);
	if (NULL == ReserveAllocBigBlock) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
	} else {
		ReserveAllocOffset = 0;
		ReserveAllocAll();
		if (n != ReserveAllocOffset) {
			/* oops, program error */
		} else {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

LOCALPROC UnallocMyMemory(void)
{
	if (nullpr != ReserveAllocBigBlock) {
		free((char *)ReserveAllocBigBlock);
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

	//CheckForSavedTasks();
	if (ForceMacOff) {
		return;
	}

	if (CurSpeedStopped) {
		return;
	} else {
		DoEmulateExtraTime();
		RunOnEndOfSixtieth();
	}
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _resumeEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1resumeEmulation (JNIEnv * env, jclass class) {
	CurSpeedStopped = 0;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _pauseEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1pauseEmulation (JNIEnv * env, jclass class) {
	CurSpeedStopped = 1;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    isPaused
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_isPaused (JNIEnv * env, jclass class) {
	return CurSpeedStopped?JNI_TRUE:JNI_FALSE;
}

#if 0
#pragma mark -
#pragma mark Misc
#endif

LOCALFUNC blnr LoadMacRom(void * romData, size_t romSize)
{
	if (romSize < kROM_Size) {
		MacMsg(kStrShortROMTitle, kStrShortROMMessage, trueblnr);
		SpeedStopped = trueblnr;
		return falseblnr;
	} else {
		memcpy(ROM, romData, kROM_Size);
		return trueblnr;
	}
}

LOCALFUNC blnr Screen_Init(void) {
	screencomparebuff = malloc(vMacScreenNumBytes);
	if (screencomparebuff == NULL)
		return falseblnr;

#if UseControlKeys
	CntrlDisplayBuff = malloc(vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL)
		return falseblnr;
#endif

#if 0 != vMacScreenDepth
    ColorModeWorks = trueblnr;
#endif

	return trueblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    init
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_init (JNIEnv * env, jclass this, jobject romBuffer) {
	if (initDone) return JNI_FALSE;
	
	void * romData = (*env)->GetDirectBufferAddress(env, romBuffer);
	size_t romSize = (*env)->GetDirectBufferCapacity(env, romBuffer);

	if (AllocMyMemory())
	if (LoadMacRom(romData, romSize))
	if (Screen_Init())
	if (InitEmulation())
	{
		// get java method IDs
		jSonyTransfer = (*env)->GetStaticMethodID(env, this, "sonyTransfer", "(ZLjava/nio/ByteBuffer;III)I");
		jSonyGetSize = (*env)->GetStaticMethodID(env, this, "sonyGetSize", "(I)I");
		jSonyEject = (*env)->GetStaticMethodID(env, this, "sonyEject", "(I)I");
		jWarnMsg = (*env)->GetStaticMethodID(env, this, "warnMsg", "(Ljava/lang/String;Ljava/lang/String;)V");

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
	}
	return JNI_FALSE;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    uninit
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_uninit (JNIEnv * env, jclass this) {

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

#if MySoundEnabled
	//MySound_Stop();
#endif
#if MySoundEnabled
	//MySound_UnInit();
#endif
#if IncludeHostTextClipExchange
	FreeMyClipBuffer();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif

	UnallocMyMemory();

	CheckSavedMacMsg();
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
	//CATCHSIG(SIGSTKFLT);
	CATCHSIG(SIGPIPE);

	return JNI_VERSION_1_2;
}
