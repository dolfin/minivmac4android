/*
	SYSDEPNS.h

	Copyright (C) 2006 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	SYStem DEPeNdencies.
*/

#ifdef SYSDEPNS_H
#error "header already included"
#else
#define SYSDEPNS_H
#endif

#include "CNFGGLOB.h"

#ifndef UnusedParam
#define UnusedParam(p) (void) p
#endif

/*--- integer types ----*/

/*
	define signed and unsigned integer types
	for 8 bits, 16 bits, 32 bits, and so on.
*/

/* 8 bits */

/* (ui3b)0 - (ui3b)1 == (ui3b)255 */
#ifndef HaveRealui3b
typedef unsigned char ui3b;
#define HaveRealui3b 1
#endif

/* sizeof(si3b) == sizeof(ui3b) */
#ifndef HaveRealsi3b
typedef signed char si3b;
#define HaveRealsi3b 1
#endif

/* 16 bits */

/* (ui4b)0 - (ui4b)1 == (ui4b)65535 */
#ifndef HaveRealui4b
typedef unsigned short ui4b;
#define HaveRealui4b 1
#endif

/* sizeof(si4b) == sizeof(ui4b) */
#ifndef HaveRealsi4b
typedef short si4b;
#define HaveRealsi4b 1
#endif

/* 32 bits */

/* (ui5b)0 - (ui5b)1 == (ui5b)4294967295 */
#ifndef HaveRealui5b
typedef unsigned long ui5b;
#define HaveRealui5b 1
#endif

/* sizeof(si4b) == sizeof(ui5b) */
#ifndef HaveRealsi5b
typedef long si5b;
#define HaveRealsi5b 1
#endif


/* 64 bits */ /* this is mostly for illustration, not used */
#ifndef HaveRealui6b
#define HaveRealui6b 0
#endif

#ifndef HaveRealsi6b
#define HaveRealsi6b 0
#endif


/*--- integer representation types ----*/

/*
	for each integer type, define
	the most efficient representation
	for parameter passing and temporary
	variables on the current
	computer.
*/

#ifndef ui3beqr
typedef ui3b ui3r;
#define ui3beqr 1
#endif

#ifndef si3beqr
typedef si3b si3r;
#define si3beqr 1
#endif

#ifndef ui4beqr
typedef ui4b ui4r;
#define ui4beqr 1
#endif

#ifndef si4beqr
typedef si4b si4r;
#define si4beqr 1
#endif

#ifndef ui5beqr
typedef ui5b ui5r;
#define ui5beqr 1
#endif

#ifndef si5beqr
typedef si5b si5r;
#define si5beqr 1
#endif

typedef ui3b *ui3p;
typedef ui4b *ui4p;
typedef ui5b *ui5p;

/*
	Largest efficiently supported
	representation types. uimr should be
	large enough to hold number of elements
	of any array we will deal with.
*/
typedef unsigned long uimr;
typedef long simr;

#define blnr int
#define trueblnr 1
#define falseblnr 0

#define nullpr ((void *) 0)

#define anyp ui3p

/* pascal string, single byte characters */
#define ps3p ui3p

#define LOCALVAR static
#ifdef AllFiles
#define GLOBALVAR LOCALVAR
#define EXPORTVAR(t, v)
#else
#define GLOBALVAR
#define EXPORTVAR(t, v) extern t v;
#endif

#define LOCALFUNC static
#define FORWARDFUNC LOCALFUNC
#ifdef AllFiles
#define GLOBALFUNC LOCALFUNC
#define EXPORTFUNC LOCALFUNC
#else
#define GLOBALFUNC
#define EXPORTFUNC extern
#endif
#define IMPORTFUNC EXPORTFUNC
#define TYPEDEFFUNC typedef

#define LOCALPROC LOCALFUNC void
#define GLOBALPROC GLOBALFUNC void
#define EXPORTPROC EXPORTFUNC void
#define IMPORTPROC IMPORTFUNC void
#define FORWARDPROC FORWARDFUNC void
#define TYPEDEFPROC TYPEDEFFUNC void
