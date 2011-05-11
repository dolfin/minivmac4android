/*
	M68KITAB.h

	Copyright (C) 2007, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef M68KITAB_H
#error "header already included"
#else
#define M68KITAB_H
#endif

enum {
	kIKindTst,
	kIKindCmp,
	kIKindCmpI,
	kIKindCmpM,
	kIKindBcc,
	kIKindBra,
	kIKindDBcc,
	kIKindSwap,
	kIKindMoveL,
	kIKindMoveW,
	kIKindMoveB,
	kIKindMoveAL,
	kIKindMoveAW,
	kIKindMoveQ,
	kIKindAddEaR,
	kIKindAddQ,
	kIKindAddI,
	kIKindAddREa,
	kIKindSubEaR,
	kIKindSubQ,
	kIKindSubI,
	kIKindSubREa,
	kIKindLea,
	kIKindPEA,
	kIKindA,
	kIKindBsr,
	kIKindJsr,
	kIKindLinkA6,
	kIKindMOVEMRmML,
	kIKindMOVEMApRL,
	kIKindUnlkA6,
	kIKindRts,
	kIKindJmp,
	kIKindClr,
	kIKindAddA,
	kIKindAddQA,
	kIKindSubA,
	kIKindSubQA,
	kIKindCmpA,
	kIKindAddXd,
	kIKindAddXm,
	kIKindSubXd,
	kIKindSubXm,
	kIKindRolopNM,
	kIKindRolopND,
	kIKindRolopDD,
	kIKindBitOpDD,
	kIKindBitOpDM,
	kIKindBitOpND,
	kIKindBitOpNM,
	kIKindAndI,
	kIKindAndEaD,
	kIKindAndDEa,
	kIKindOrI,
	kIKindOrDEa,
	kIKindOrEaD,
	kIKindEor,
	kIKindEorI,
	kIKindNot,
	kIKindScc,
	kIKindNegX,
	kIKindNeg,
	kIKindEXTW,
	kIKindEXTL,
	kIKindMulU,
	kIKindMulS,
	kIKindDivU,
	kIKindDivS,
	kIKindExgdd,
	kIKindExgaa,
	kIKindExgda,
	kIKindMoveCCREa,
	kIKindMoveEaCCR,
	kIKindMoveSREa,
	kIKindMoveEaSR,
	kIKindBinOpStatusCCR,
	kIKindMOVEMApRW,
	kIKindMOVEMRmMW,
	kIKindMOVEMrm,
	kIKindMOVEMmr,
	kIKindAbcdr,
	kIKindAbcdm,
	kIKindSbcdr,
	kIKindSbcdm,
	kIKindNbcd,
	kIKindRte,
	kIKindNop,
	kIKindMoveP,
	kIKindIllegal,
	kIKindChkW,
	kIKindTrap,
	kIKindTrapV,
	kIKindRtr,
	kIKindLink,
	kIKindUnlk,
	kIKindMoveRUSP,
	kIKindMoveUSPR,
	kIKindTas,
	kIKindF,
	kIKindCallMorRtm,
	kIKindStop,
	kIKindReset,

#if Use68020
	kIKindEXTBL,
	kIKindTRAPcc,
	kIKindChkL,
	kIKindBkpt,
	kIKindDivL,
	kIKindMulL,
	kIKindRtd,
	kIKindMoveC,
	kIKindLinkL,
	kIKindPack,
	kIKindUnpk,
	kIKindCHK2orCMP2,
	kIKindCAS2,
	kIKindCAS,
	kIKindMoveS,
	kIKindBitField,
#endif

	kNumIKinds
};

EXPORTPROC M68KITAB_setup(ui3b *p);
