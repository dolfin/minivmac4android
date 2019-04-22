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
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF
#undef ABS
#define ABS(x) (((x)>0)? (x) : -(x))

#undef CLAMP
#define CLAMP(x, lo, hi) (((x) > (hi))? (hi) : (((x) < (lo))? (lo) : (x)))

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

GLOBALVAR ui3b CurMouseButton = falseblnr;

LOCALVAR blnr initDone = falseblnr;

// java
JNIEnv * jEnv;
jclass jClass;
jmethodID jSonyTransfer, jSonyGetSize, jSonyEject, jSonyGetName, jSonyMakeNewDisk, jSonyInsert2;
jmethodID jWarnMsg;
jmethodID jInitScreen, jUpdateScreen;
jmethodID jMySoundInit, jMySoundUnInit, jPlaySound, jMySoundStart, jMySoundStop;
jobject mCore;

static jmethodID nativeCrashed;

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
		if (TimeDiff > 16 * MyInvTimeStep) {
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
	} else if (TimeDiff < - 16 * MyInvTimeStep) {
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

LOCALFUNC blnr InitLocationDat(void)
{
    GetCurrentTicks();
    CurMacDateInSeconds = NewMacDateInSeconds;

    return trueblnr;
}

#if 0
#pragma mark -
#pragma mark Sound
#endif

#if MySoundEnabled
#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
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
LOCALVAR ui4b MinFilledSoundBuffs = kSoundBuffers;
LOCALVAR ui4b TheWriteOffset = 0;

#if 4 == kLn2SoundSampSz
LOCALPROC ConvertSoundBlockToNative(tpSoundSamp p)
{
	int i;

	for (i = kOneBuffLen; --i >= 0; ) {
		*p++ -= 0x8000;
	}
}
#else
#define ConvertSoundBlockToNative(p)
#endif

LOCALPROC MySound_WriteOut(void)
{
	int retry_count = 32;

	label_retry:
	if (--retry_count > 0) {

		tpSoundSamp NextPlayPtr;
		ui4b PlayNowSize = 0;
		ui4b MaskedFillOffset = ThePlayOffset & kOneBuffMask;

		if (MaskedFillOffset != 0) {
			/* take care of left overs */
			PlayNowSize = kOneBuffLen - MaskedFillOffset;
			NextPlayPtr = TheSoundBuffer + (ThePlayOffset & kAllBuffMask);
		} else if (0 != ((TheFillOffset - ThePlayOffset) >> kLnOneBuffLen)) {
			PlayNowSize = kOneBuffLen;
			NextPlayPtr = TheSoundBuffer + (ThePlayOffset & kAllBuffMask);
		} else {
			/* nothing to play now */
		}

		if (0 != PlayNowSize) {
			jbyteArray jBuffer = (*jEnv)->NewByteArray(jEnv, PlayNowSize);
			if (jBuffer != NULL) {
				(*jEnv)->SetByteArrayRegion(jEnv, jBuffer, 0, PlayNowSize, (jbyte *) NextPlayPtr);
				int err = (*jEnv)->CallIntMethod(jEnv, mCore, jPlaySound, jBuffer);
				(*jEnv)->DeleteLocalRef(jEnv, jBuffer);
				if (err >= 0) {
					ThePlayOffset += err;
					goto label_retry;
				}
			}
		}
	}
}

LOCALFUNC blnr MySound_EndWrite0(ui4r actL)
{
	blnr v;

	TheWriteOffset += actL;

	if (0 != (TheWriteOffset & kOneBuffMask)) {
		v = falseblnr;
	} else {
		/* just finished a block */

		TheFillOffset = TheWriteOffset;

		v = trueblnr;
	}

	return v;
}

GLOBALPROC MySound_EndWrite(ui4r actL)
{
	if (MySound_EndWrite0(actL)) {
		ConvertSoundBlockToNative(TheSoundBuffer
								  + ((TheFillOffset - kOneBuffLen) & kAllBuffMask));
		MySound_WriteOut();
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
	if (MinFilledSoundBuffs <= kSoundBuffers) {
		if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
			IncrNextTime();
		} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
			++TrueEmulatedTime;
		}
		MinFilledSoundBuffs = kSoundBuffers + 1;
	}
}

#endif

#if 0
#pragma mark -
#pragma mark Paramter buffers
#endif

#include "COMOSGLU.h"

#include "PBUFSTDC.h"

#include "CONTROLM.h"

/* --- text translation --- */

#if IncludePbufs
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
#endif

#if IncludePbufs
LOCALFUNC tMacErr NativeTextToMacRomanPbuf(const char *x, tPbuf *r)
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
#endif

#if IncludePbufs
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
#endif

#if IncludePbufs
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
#endif

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
 * Method:    notifyDiskEjected
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskCreated (JNIEnv * env, jclass class) {
	vSonyNewDiskWanted = falseblnr;
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
	ui5r actCount = (*jEnv)->CallIntMethod(jEnv, mCore, jSonyTransfer, (jboolean)IsWrite, jBuffer, (jint)Drive_No, (jint)Sony_Start, (jint)Sony_Count);
	(*jEnv)->DeleteLocalRef(jEnv, jBuffer);

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = actCount;
	}

	return (actCount >= 0 ? mnvm_noErr : -1);
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	*Sony_Count = (*jEnv)->CallIntMethod(jEnv, mCore, jSonyGetSize, (jint)Drive_No);
	if (*Sony_Count < 0) return -1;
	return 0;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No) {
	return (*jEnv)->CallIntMethod(jEnv, mCore, jSonyEject, (jint)Drive_No, JNI_FALSE);
}

#if IncludeSonyNew
GLOBALFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	return (*jEnv)->CallIntMethod(jEnv, mCore, jSonyEject, (jint)Drive_No, JNI_TRUE);
}
#endif

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if IncludeSonyGetName
GLOBALFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	jstring result = (*jEnv)->CallObjectMethod(jEnv, mCore, jSonyGetName, (jint)Drive_No);

	if (NULL == result) {
		return -1;
	} else {
		const char *nativeString = (*jEnv)->GetStringUTFChars(jEnv, result, 0);
		tMacErr res = NativeTextToMacRomanPbuf(nativeString, r);
		(*jEnv)->ReleaseStringUTFChars(jEnv, result, nativeString);
		return res;
	}
}
#endif

LOCALFUNC blnr Sony_InsertIth(int i)
{
    blnr v;

    if ((i > 9) || ! FirstFreeDisk(nullpr)) {
        v = falseblnr;
    } else {
        char s[] = "disk?.dsk";

        s[4] = '0' + i;

        jstring jdiskname = (*jEnv)->NewStringUTF(jEnv, s);
        v = (*jEnv)->CallBooleanMethod(jEnv, mCore, jSonyInsert2, jdiskname);
        (*jEnv)->DeleteLocalRef(jEnv, jdiskname);

    }

    return v;
}

LOCALFUNC blnr LoadInitialImages(void)
{
    if (! AnyDiskInserted()) {
        int i;

        for (i = 1; Sony_InsertIth(i); ++i) {
            /* stop on first error (including file not found) */
        }
    }

    return trueblnr;
}

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, char *drivename)
{
	jstring jdrivename = (*jEnv)->NewStringUTF(jEnv, drivename);
	(*jEnv)->CallIntMethod(jEnv, mCore, jSonyMakeNewDisk, (jint)L, jdrivename);
	(*jEnv)->DeleteLocalRef(jEnv, jdrivename);
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDiskAtDefault(ui5b L)
{
	char s[ClStrMaxLength + 1];

	NativeStrFromCStr(s, "untitled.dsk");
	MakeNewDisk(L, s);
}
#endif

#if 0
#pragma mark -
#pragma mark Sound
#endif

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    MySound_Start0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_MySound_1Start0 (JNIEnv * env, jclass class) {
#if MySoundEnabled
	/* Reset variables */
	ThePlayOffset = 0;
	TheFillOffset = 0;
	TheWriteOffset = 0;
	MinFilledSoundBuffs = kSoundBuffers + 1;
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
		jintArray jArray = (*jEnv)->NewIntArray(jEnv, (jsize)0);
		return jArray;
	}

	int changesWidth = right - left;
	int changesHeight = bottom - top;
	jsize changesSize = changesWidth * changesHeight;
	int i,x,y;

	// create java array of changes: top, left, bottom, right, pixels...
	jintArray jArray = (*jEnv)->NewIntArray(jEnv, changesSize);
	jboolean arrayCopy = JNI_FALSE;
	jint *arr = (jint*)(*jEnv)->GetPrimitiveArrayCritical(jEnv, (jarray)jArray, &arrayCopy);
	jint *px = arr;

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

	(*jEnv)->ReleasePrimitiveArrayCritical(jEnv, (jarray)jArray, (void*)arr, 0);
	return jArray;
}

LOCALPROC MyDrawChangesAndClear(void)
{
	if (ScreenChangedBottom > ScreenChangedTop) {
		(*jEnv)->CallVoidMethod(jEnv, mCore, jUpdateScreen, (jint)ScreenChangedTop, (jint)ScreenChangedLeft,
                                (jint)ScreenChangedBottom, (jint)ScreenChangedRight);
		ScreenClearChanges();
	}
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
		/*char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

		jstring jBriefMsg = (*jEnv)->NewStringUTF(jEnv, SavedBriefMsg);
		jstring jLongMsg = (*jEnv)->NewStringUTF(jEnv, SavedLongMsg);
		(*jEnv)->CallVoidMethod(jEnv, mCore, jWarnMsg, jBriefMsg, jLongMsg);
		(*jEnv)->DeleteLocalRef(jEnv, jBriefMsg);
		(*jEnv)->DeleteLocalRef(jEnv, jLongMsg);*/

		SavedBriefMsg = nullpr;
	}
}

#if 0
#pragma mark -
#pragma mark Emulation
#endif

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled
	(*jEnv)->CallVoidMethod(jEnv, mCore, jMySoundStart);
#endif

    StartUpTimeAdjust();
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled
	(*jEnv)->CallVoidMethod(jEnv, mCore, jMySoundStop);
#endif
}

LOCALPROC CheckForSavedTasks(void)
{
	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if (AnyDiskInserted()) {
			MacMsgOverride(kStrQuitWarningTitle,
						   kStrQuitWarningMessage);
		} else {
			ForceMacOff = trueblnr;
		}
	}

	if (ForceMacOff) {
		return;
	}

    if (CurSpeedStopped != (SpeedStopped ||
    	(gBackgroundFlag && ! RunInBackground)))
    {
        CurSpeedStopped = ! CurSpeedStopped;
        if (CurSpeedStopped) {
            EnterSpeedStopped();
        } else {
            LeaveSpeedStopped();
        }
    }

#if IncludeSonyNew
	if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
		if (vSonyNewDiskName != NotAPbuf) {
			ui3p NewDiskNameDat;
			if (MacRomanTextToNativePtr(vSonyNewDiskName, trueblnr,
				&NewDiskNameDat))
			{
				MakeNewDisk(vSonyNewDiskSize, (char *)NewDiskNameDat);
				free(NewDiskNameDat);
			}
			PbufDispose(vSonyNewDiskName);
			vSonyNewDiskName = NotAPbuf;
		} else
#endif
		{
			MakeNewDiskAtDefault(vSonyNewDiskSize);
		}
	}
#endif

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}
}

/* --- main program flow --- */

GLOBALOSGLUPROC DoneWithDrawingForTick(void)
{
#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		AutoScrollScreen();
	}
#endif
	MyDrawChangesAndClear();
}

GLOBALOSGLUFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}


GLOBALOSGLUPROC WaitForNextTick(void)
{
label_retry:
    sleep(0);
	CheckForSavedTasks();
	if (ForceMacOff) {
		return;
	}

	if (CurSpeedStopped) {
		DoneWithDrawingForTick();
		goto label_retry;
	}

	if (ExtraTimeNotOver()) {
		struct timespec rqt;
		struct timespec rmt;

		si5b TimeDiff = GetTimeDiff();
		if (TimeDiff < 0) {
			rqt.tv_sec = 0;
			rqt.tv_nsec = (- TimeDiff) * 1000;
			(void) nanosleep(&rqt, &rmt);
		}
		goto label_retry;
	}

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
#if EnableDemoMsg
		DemoModeSecondNotify();
#endif
	}

#if UseMotionEvents
	if (! CaughtMouse)
#endif
	{
		//CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	dbglog_writelnNum("WaitForNextTick, OnTrueTime", OnTrueTime);
#endif
}

/* --- platform independent code can be thought of as going here --- */

#include "PROGMAIN.h"

LOCALPROC ZapOSGLUVars(void)
{
    //InitDrives();
    //ZapWinStateVars();
}

LOCALPROC ReserveAllocAll(void)
{
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, falseblnr);

    ReserveAllocOneBlock(&screencomparebuff,
                         vMacScreenNumBytes, 5, trueblnr);
#if UseControlKeys
    ReserveAllocOneBlock(&CntrlDisplayBuff,
                         vMacScreenNumBytes, 5, falseblnr);
#endif
#if WantScalingBuff
    ReserveAllocOneBlock(&ScalingBuff,
		ScalingBuffsz, 5, falseblnr);
#endif
#if WantScalingTabl
    ReserveAllocOneBlock(&ScalingTabl,
		ScalingTablsz, 5, falseblnr);
#endif

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
 * Method:    setRequestMacOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setRequestMacOff (JNIEnv * env, jclass class) {
    RequestMacOff = trueblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _resumeEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1resumeEmulation (JNIEnv * env, jclass class) {
	gBackgroundFlag = falseblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _pauseEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1pauseEmulation (JNIEnv * env, jclass class) {
	gBackgroundFlag = trueblnr;
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

LOCALFUNC tMacErr LoadMacRomFrom(void * romData, size_t romSize)
{
    tMacErr err;

    if (NULL == romData) {
        err = mnvm_fnfErr;
    } else {
        memcpy(ROM, romData, kROM_Size);
        if (kROM_Size != romSize) {
            if (kROM_Size > romSize) {
                MacMsgOverride(kStrShortROMTitle,
                               kStrShortROMMessage);
                err = mnvm_eofErr;
            }
        } else {
            err = ROM_IsValid();
        }
    }

    return err;
}

LOCALFUNC blnr LoadMacRom(void * romData, size_t romSize)
{
    tMacErr err;

    if (mnvm_fnfErr == (err = LoadMacRomFrom(romData, romSize)))
    {
    }

    return trueblnr; /* keep launching Mini vMac, regardless */
}

LOCALFUNC blnr Screen_Init(void) {

#if 0 != vMacScreenDepth
    ColorModeWorks = trueblnr;
#endif

	return trueblnr;
}

LOCALFUNC blnr InitOSGLU(void * romData, size_t romSize)
{
    if (AllocMyMemory())
#if CanGetAppPath
        if (InitWhereAmI())
#endif
#if dbglog_HAVE
        if (dbglog_open())
#endif
        //if (ScanCommandLine())
        if (LoadMacRom(romData, romSize))
        if (LoadInitialImages())
#if UseActvCode
            if (ActvCodeInit())
#endif
            if (InitLocationDat())
#if MySoundEnabled
                if ((*jEnv)->CallBooleanMethod(jEnv, mCore, jMySoundInit))
#endif
                if (Screen_Init())
                    //if (CreateMainWindow())
                    //if (KC2MKCInit())
                {
					(*jEnv)->CallVoidMethod(jEnv, mCore, jInitScreen);
                    initDone = trueblnr;
                    return trueblnr;
                }
    return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
    if (MacMsgDisplayed) {
        MacMsgDisplayOff();
    }

    //RestoreKeyRepeat();
#if MayFullScreen
    //UngrabMachine();
#endif
#if MySoundEnabled
	(*jEnv)->CallVoidMethod(jEnv, mCore, jMySoundStop);
#endif
#if MySoundEnabled
    (*jEnv)->CallVoidMethod(jEnv, mCore, jMySoundUnInit);
#endif
#if IncludeHostTextClipExchange
    FreeMyClipBuffer();
#endif
#if IncludePbufs
    UnInitPbufs();
#endif
    UnInitDrives();

#if dbglog_HAVE
    dbglog_close();
#endif

#if CanGetAppPath
    UninitWhereAmI();
#endif
    UnallocMyMemory();

    CheckSavedMacMsg();

    initDone = falseblnr;
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    init
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_init (JNIEnv * env, jclass this, jobject core, jobject romBuffer) {
	if (initDone) return JNI_FALSE;
	
	void * romData = (*env)->GetDirectBufferAddress(env, romBuffer);
	size_t romSize = (*env)->GetDirectBufferCapacity(env, romBuffer);

		mCore = (*env)->NewGlobalRef(env, core);
		// get java method IDs
		jSonyTransfer = (*env)->GetMethodID(env, this, "sonyTransfer", "(ZLjava/nio/ByteBuffer;III)I");
		jSonyGetSize = (*env)->GetMethodID(env, this, "sonyGetSize", "(I)I");
		jSonyEject = (*env)->GetMethodID(env, this, "sonyEject", "(IZ)I");
		jSonyGetName = (*env)->GetMethodID(env, this, "sonyGetName", "(I)Ljava/lang/String;");
		jSonyMakeNewDisk = (*env)->GetMethodID(env, this, "sonyMakeNewDisk", "(ILjava/lang/String;)I");
        jSonyInsert2 = (*env)->GetMethodID(env, this, "sonyInsert2", "(Ljava/lang/String;)Z");
		jWarnMsg = (*env)->GetMethodID(env, this, "warnMsg", "(Ljava/lang/String;Ljava/lang/String;)V");
		jInitScreen = (*env)->GetMethodID(env, this, "initScreen", "()V");
		jUpdateScreen = (*env)->GetMethodID(env, this, "updateScreen", "(IIII)V");
		jPlaySound = (*env)->GetMethodID(env, this, "playSound", "([B)I");
        jMySoundInit = (*env)->GetMethodID(env, this, "MySound_Init", "()Z");
        jMySoundUnInit = (*env)->GetMethodID(env, this, "MySound_UnInit", "()V");
		jMySoundStart = (*env)->GetMethodID(env, this, "MySound_Start", "()V");
		jMySoundStop = (*env)->GetMethodID(env, this, "MySound_Stop", "()V");

		// initialize fields
		jfieldID sDiskPath, sDiskFile, sNumInsertedDisks, sInitOk;
		sDiskPath = (*env)->GetFieldID(env, this, "diskPath", "[Ljava/lang/String;");
		sDiskFile = (*env)->GetFieldID(env, this, "diskFile", "[Ljava/io/RandomAccessFile;");
		sNumInsertedDisks = (*env)->GetFieldID(env, this, "numInsertedDisks", "I");
		sInitOk = (*env)->GetFieldID(env, this, "initOk", "Z");

		// init drives
		jobjectArray diskPath = (*env)->NewObjectArray(env, NumDrives, (*env)->FindClass(env, "java/lang/String"), NULL);
		jobjectArray diskFile = (*env)->NewObjectArray(env, NumDrives, (*env)->FindClass(env, "java/io/RandomAccessFile"), NULL);
		(*env)->SetIntField(env, mCore, sNumInsertedDisks, 0);
		(*env)->SetObjectField(env, mCore, sDiskPath, diskPath);
		(*env)->SetObjectField(env, mCore, sDiskFile, diskFile);

	ZapOSGLUVars();
	if (InitOSGLU(romData, romSize)) {
		// init ok
		(*env)->SetBooleanField(env, mCore, sInitOk, JNI_TRUE);

		ProgramMain();
	}
	(*env)->SetBooleanField(env, mCore, sInitOk, JNI_FALSE);
	UnInitOSGLU();
	(*env)->DeleteGlobalRef(env, mCore);

	return JNI_TRUE;
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
