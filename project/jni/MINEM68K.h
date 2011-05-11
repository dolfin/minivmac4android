/*
	MINEM68K.h

	Copyright (C) 2004 Bernd Schmidt, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef MINEM68K_H
#error "header already included"
#else
#define MINEM68K_H
#endif

EXPORTPROC MINEM68K_Init(ui3b **BankReadAddr, ui3b **BankWritAddr,
	ui3b *fIPL);
#if SmallGlobals
EXPORTPROC MINEM68K_ReserveAlloc(void);
#endif

EXPORTPROC m68k_IPLchangeNtfy(void);
EXPORTPROC DiskInsertedPsuedoException(CPTR newpc, ui5b data);
EXPORTPROC m68k_reset(void);

EXPORTFUNC ui5b GetInstructionsRemaining(void);
EXPORTPROC SetInstructionsRemaining(ui5b n);

EXPORTPROC m68k_go_nInstructions(ui5b n);
