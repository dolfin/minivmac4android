/*
	M68KITAB.c

	Copyright (C) 2007 Paul C. Pratt

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
	Motorola 68K Instructions TABle
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#endif

#include "MYOSGLUE.h"
#include "EMCONFIG.h"

#include "M68KITAB.h"

LOCALVAR ui5b opcode;

#define b76 ((opcode >> 6) & 3)
#define b8 ((opcode >> 8) & 1)
#define mode ((opcode >> 3) & 7)
#define reg (opcode & 7)
#define md6 ((opcode >> 6) & 7)
#define rg9 ((opcode >> 9) & 7)

LOCALFUNC blnr IsValidAddrMode(void)
{
	return (mode != 7) || (reg < 5);
}

LOCALFUNC blnr IsValidDstAddrMode(void)
{
	return (md6 != 7) || (rg9 < 2);
}

LOCALFUNC MayNotInline ui3b CheckDataAltAddrMode(ui3b v)
{
	blnr IsOk;

	switch (mode) {
		case 1:
		default: /* keep compiler happy */
			IsOk = falseblnr;
			break;
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			IsOk = trueblnr;
			break;
		case 7:
			IsOk = reg < 2;
			break;
	}

	if (IsOk) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayNotInline ui3b CheckDataAddrMode(ui3b v)
{
	blnr IsOk;

	switch (mode) {
		case 1:
		default: /* keep compiler happy */
			IsOk = falseblnr;
			break;
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			IsOk = trueblnr;
			break;
		case 7:
			IsOk = reg < 5;
			break;
	}

	if (IsOk) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayNotInline ui3b CheckControlAddrMode(ui3b v)
{
	blnr IsOk;

	switch (mode) {
		case 0:
		case 1:
		case 3:
		case 4:
		default: /* keep compiler happy */
			IsOk = falseblnr;
			break;
		case 2:
		case 5:
		case 6:
			IsOk = trueblnr;
			break;
		case 7:
			IsOk = reg < 4;
			break;
	}

	if (IsOk) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayNotInline ui3b CheckControlAltAddrMode(ui3b v)
{
	blnr IsOk;

	switch (mode) {
		case 0:
		case 1:
		case 3:
		case 4:
		default: /* keep compiler happy */
			IsOk = falseblnr;
			break;
		case 2:
		case 5:
		case 6:
			IsOk = trueblnr;
			break;
		case 7:
			IsOk = reg < 2;
			break;
	}

	if (IsOk) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayNotInline ui3b CheckAltMemAddrMode(ui3b v)
{
	blnr IsOk;

	switch (mode) {
		case 0:
		case 1:
		default: /* keep compiler happy */
			IsOk = falseblnr;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			IsOk = trueblnr;
			break;
		case 7:
			IsOk = reg < 2;
			break;
	}

	if (IsOk) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayNotInline ui3b CheckValidAddrMode(ui3b v)
{
	if (IsValidAddrMode()) {
		return v;
	} else {
		return kIKindIllegal;
	}
}

LOCALFUNC MayInline ui3b DeCode0(void)
{
	ui3b v;

	if (b8 == 1) {
		if (mode == 1) {
			/* MoveP 0000ddd1mm001aaa */
			v = kIKindMoveP;
		} else {
			/* dynamic bit, Opcode = 0000ddd1ttmmmrrr */
			if (mode == 0) {
				v = kIKindBitOpDD;
			} else {
				if (b76 == 0) {
					v = CheckDataAddrMode(kIKindBitOpDM);
				} else {
					v = CheckDataAltAddrMode(kIKindBitOpDM);
				}
			}
		}
	} else {
		if (rg9 == 4) {
			/* static bit 00001010ssmmmrrr */
			if (mode == 0) {
				v = kIKindBitOpND;
			} else {
				if (b76 == 0) {
					if ((mode == 7) && (reg == 4)) {
						v = kIKindIllegal;
					} else {
						v = CheckDataAddrMode(kIKindBitOpNM);
					}
				} else {
					v = CheckDataAltAddrMode(kIKindBitOpNM);
				}
			}
		} else
		if (b76 == 3) {
#if Use68020
			if (rg9 < 3) {
				/* CHK2 or CMP2 00000ss011mmmrrr */
				v = CheckControlAddrMode(kIKindCHK2orCMP2);
			} else
			if (rg9 >= 5) {
				if ((mode == 7) && (reg == 4)) {
					/* CAS2 00001ss011111100 */
					v = kIKindCAS2;
				} else {
					/* CAS  00001ss011mmmrrr */
					v = kIKindCAS;
				}
			} else
			if (rg9 == 3) {
				/* CALLM or RTM 0000011011mmmrrr */
				v = kIKindCallMorRtm;
			} else
#endif
			{
				v = kIKindIllegal;
			}
		} else
		if (rg9 == 6) {
			/* CMPI 00001100ssmmmrrr */
			v = CheckDataAltAddrMode(kIKindCmpI);
		} else if (rg9 == 7) {
#if Use68020
			/* MoveS 00001110ssmmmrrr */
			v = CheckAltMemAddrMode(kIKindMoveS);
#else
			v = kIKindIllegal;
#endif
		} else {
			if ((mode == 7) && (reg == 4)) {
				switch (rg9) {
					case 0:
					case 1:
					case 5:
						v = kIKindBinOpStatusCCR;
						break;
					default:
						v = kIKindIllegal;
						break;
				}
			} else {
				switch (rg9) {
					case 0:
						v = CheckDataAltAddrMode(kIKindOrI);
						break;
					case 1:
						v = CheckDataAltAddrMode(kIKindAndI);
						break;
					case 2:
						v = CheckDataAltAddrMode(kIKindSubI);
						break;
					case 3:
						v = CheckDataAltAddrMode(kIKindAddI);
						break;
					case 5:
						v = CheckDataAltAddrMode(kIKindEorI);
						break;
					default: /* for compiler. should be 0, 1, 2, 3, or 5 */
						v = kIKindIllegal;
						break;
				}
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode1(void)
{
	ui3b v;

	if ((mode == 1) || ! IsValidAddrMode()) {
		v = kIKindIllegal;
	} else if (md6 == 1) { /* MOVEA */
		v = kIKindIllegal;
	} else if (! IsValidDstAddrMode()) {
		v = kIKindIllegal;
	} else {
		v = kIKindMoveB;
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode2(void)
{
	ui3b v;

	if (md6 == 1) { /* MOVEA */
		v = CheckValidAddrMode(kIKindMoveAL);
	} else if (! IsValidAddrMode()) {
		v = kIKindIllegal;
	} else if (! IsValidDstAddrMode()) {
		v = kIKindIllegal;
	} else {
		v = kIKindMoveL;
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode3(void)
{
	ui3b v;

	if (md6 == 1) { /* MOVEA */
		v = CheckValidAddrMode(kIKindMoveAW);
	} else if (! IsValidAddrMode()) {
		v = kIKindIllegal;
	} else if (! IsValidDstAddrMode()) {
		v = kIKindIllegal;
	} else {
		v = kIKindMoveW;
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode4(void)
{
	ui3b v;

	if (b8 != 0) {
		switch (b76) {
			case 0:
#if Use68020
				/* Chk.L 0100ddd100mmmrrr */
				v = CheckDataAddrMode(kIKindChkL);
#else
				v = kIKindIllegal;
#endif
				break;
			case 1:
				v = kIKindIllegal;
				break;
			case 2:
				/* Chk.W 0100ddd110mmmrrr */
				v = CheckDataAddrMode(kIKindChkW);
				break;
			case 3:
			default: /* keep compiler happy */
#if Use68020
				if ((0 == mode) && (4 == rg9)) {
					v = kIKindEXTBL;
				} else
#endif
				{
					/* Lea 0100aaa111mmmrrr */
					v = CheckControlAddrMode(kIKindLea);
				}
				break;
		}
	} else {
		switch (rg9) {
			case 0:
				if (b76 != 3) {
					/* NegX 01000000ssmmmrrr */
					v = CheckDataAltAddrMode(kIKindNegX);
				} else {
#if Use68020
/* reference seems incorrect to say not for 68000 */
#endif
					/* Move from SR 0100000011mmmrrr */
					v = CheckDataAltAddrMode(kIKindMoveSREa);
				}
				break;
			case 1:
				if (b76 != 3) {
					/* Clr 01000010ssmmmrrr */
					v = CheckDataAltAddrMode(kIKindClr);
				} else {
#if Use68020
					/* Move from CCR 0100001011mmmrrr */
					v = CheckDataAltAddrMode(kIKindMoveCCREa);
#else
					v = kIKindIllegal;
#endif
				}
				break;
			case 2:
				if (b76 != 3) {
					/* Neg 01000100ssmmmrrr */
					v = CheckDataAltAddrMode(kIKindNeg);
				} else {
					/* Move to CCR 0100010011mmmrrr */
					v = CheckDataAddrMode(kIKindMoveEaCCR);
				}
				break;
			case 3:
				if (b76 != 3) {
					/* Not 01000110ssmmmrrr */
					v = CheckDataAltAddrMode(kIKindNot);
				} else {
					/* Move from SR 0100011011mmmrrr */
					v = CheckDataAddrMode(kIKindMoveEaSR);
				}
				break;
			case 4:
				switch (b76) {
					case 0:
#if Use68020
						if (mode == 1) {
							/* Link.L 0100100000001rrr */
							v = kIKindLinkL;
						} else
#endif
						{
							/* Nbcd 0100100000mmmrrr */
							v = CheckDataAltAddrMode(kIKindNbcd);
						}
						break;
					case 1:
						if (mode == 0) {
							/* Swap 0100100001000rrr */
							v = kIKindSwap;
						} else
#if Use68020
						if (mode == 1) {
							v = kIKindBkpt;
						} else
#endif
						{
							/* PEA 0100100001mmmrrr */
							v = CheckControlAddrMode(kIKindPEA);
						}
						break;
					case 2:
						if (mode == 0) {
							/* EXT.W */
							v = kIKindEXTW;
						} else {
							/* MOVEM reg to mem 01001d001ssmmmrrr */
							if (mode == 4) {
								v = kIKindMOVEMRmMW;
							} else {
								v = CheckControlAltAddrMode(kIKindMOVEMrm);
							}
						}
						break;
					case 3:
					default: /* keep compiler happy */
						if (mode == 0) {
							/* EXT.L */
							v = kIKindEXTL;
						} else {
							/* MOVEM reg to mem 01001d001ssmmmrrr */
							if (mode == 4) {
								v = kIKindMOVEMRmML;
							} else {
								v = CheckControlAltAddrMode(kIKindMOVEMrm);
							}
						}
						break;
				}
				break;
			case 5:
				if (b76 == 3) {
					if ((mode == 7) && (reg == 4)) {
						/* the ILLEGAL instruction */
						v = kIKindIllegal;
					} else {
						/* Tas 0100101011mmmrrr */
						v = CheckDataAltAddrMode(kIKindTas);
					}
				} else {
					/* Tst  01001010ssmmmrrr */
					if (b76 == 0) {
						v = CheckDataAltAddrMode(kIKindTst);
					} else {
						v = CheckValidAddrMode(kIKindTst);
					}
				}
				break;
			case 6:
				if (((opcode >> 7) & 1) == 1) {
					/* MOVEM mem to reg 0100110011smmmrrr */
					if (mode == 3) {
						if (b76 == 2) {
							v = kIKindMOVEMApRW;
						} else {
							v = kIKindMOVEMApRL;
						}
					} else {
						v = CheckControlAddrMode(kIKindMOVEMmr);
					}
				} else {
#if Use68020
					if (((opcode >> 6) & 1) == 1) {
						/* DIVU 0100110001mmmrrr 0rrr0s0000000rrr */
						/* DIVS 0100110001mmmrrr 0rrr1s0000000rrr */
						v = kIKindDivL;
					} else {
						/* MULU 0100110000mmmrrr 0rrr0s0000000rrr */
						/* MULS 0100110000mmmrrr 0rrr1s0000000rrr */
						v = kIKindMulL;
					}
#else
					v = kIKindIllegal;
#endif
				}
				break;
			case 7:
			default: /* keep compiler happy */
				switch (b76) {
					case 0:
						v = kIKindIllegal;
						break;
					case 1:
						switch (mode) {
							case 0:
							case 1:
								/* Trap 010011100100vvvv */
								v = kIKindTrap;
								break;
							case 2:
								/* Link */
								if (reg == 6) {
									v = kIKindLinkA6;
								} else {
									v = kIKindLink;
								}
								break;
							case 3:
								/* Unlk */
								if (reg == 6) {
									v = kIKindUnlkA6;
								} else {
									v = kIKindUnlk;
								}
								break;
							case 4:
								/* MOVE USP 0100111001100aaa */
								v = kIKindMoveRUSP;
								break;
							case 5:
								/* MOVE USP 0100111001101aaa */
								v = kIKindMoveUSPR;
								break;
							case 6:
								switch (reg) {
									case 0:
										/* Reset 0100111001100000 */
										v = kIKindReset;
										break;
									case 1:
										/* Nop Opcode = 0100111001110001 */
										v = kIKindNop;
										break;
									case 2:
										/* Stop 0100111001110010 */
										v = kIKindStop;
										break;
									case 3:
										/* Rte 0100111001110011 */
										v = kIKindRte;
										break;
									case 4:
										/* Rtd 0100111001110100 */
#if Use68020
										v = kIKindRtd;
#else
										v = kIKindIllegal;
#endif
										break;
									case 5:
										/* Rts 0100111001110101 */
										v = kIKindRts;
										break;
									case 6:
										/* TrapV 0100111001110110 */
										v = kIKindTrapV;
										break;
									case 7:
									default: /* keep compiler happy */
										/* Rtr 0100111001110111 */
										v = kIKindRtr;
										break;
								}
								break;
							case 7:
							default: /* keep compiler happy */
#if Use68020
								/* MOVEC 010011100111101m */
								v = kIKindMoveC;
#else
								v = kIKindIllegal;
#endif
								break;
						}
						break;
					case 2:
						/* Jsr 0100111010mmmrrr */
						v = CheckControlAddrMode(kIKindJsr);
						break;
					case 3:
					default: /* keep compiler happy */
						/* JMP 0100111011mmmrrr */
						v = CheckControlAddrMode(kIKindJmp);
						break;
				}
				break;
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode5(void)
{
	ui3b v;

	if (b76 == 3) {
		if (mode == 1) {
			/* DBcc 0101cccc11001ddd */
			v = kIKindDBcc;
		} else {
#if Use68020
			if ((mode == 7) && (reg >= 2)) {
				/* TRAPcc 0101cccc11111sss */
				v = kIKindTRAPcc;
			} else
#endif
			{
				/* Scc 0101cccc11mmmrrr */
				v = CheckDataAltAddrMode(kIKindScc);
			}
		}
	} else {
		if (mode == 1) {
			if (b8 == 0) {
				v = kIKindAddQA; /* AddQA 0101nnn0ss001rrr */
			} else {
				v = kIKindSubQA; /* SubQA 0101nnn1ss001rrr */
			}
		} else {
			if (b8 == 0) {
				/* AddQ 0101nnn0ssmmmrrr */
				v = CheckDataAltAddrMode(kIKindAddQ);
			} else {
				/* SubQ 0101nnn1ssmmmrrr */
				v = CheckDataAltAddrMode(kIKindSubQ);
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode6(void)
{
	ui3b v;
	ui5b cond = (opcode >> 8) & 15;

	if (cond == 1) {
		/* Bsr 01100001nnnnnnnn */
		v = kIKindBsr;
	} else if (cond == 0) {
		/* Bra 01100000nnnnnnnn */
		v = kIKindBra;
	} else {
		/* Bcc 0110ccccnnnnnnnn */
		v = kIKindBcc;
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode7(void)
{
	ui3b v;

	if (b8 == 0) {
		v = kIKindMoveQ;
	} else {
		v = kIKindIllegal;
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode8(void)
{
	ui3b v;

	if (b76 == 3) {
		if (b8 == 0) {
			/* DivU 1000ddd011mmmrrr */
			v = CheckDataAddrMode(kIKindDivU);
		} else {
			/* DivS 1000ddd111mmmrrr */
			v = CheckDataAddrMode(kIKindDivS);
		}
	} else {
		if (b8 == 0) {
			/* OR 1000ddd0ssmmmrrr */
			v = CheckDataAddrMode(kIKindOrEaD);
		} else {
			if (mode < 2) {
				switch (b76) {
					case 0:
						/* SBCD 1000xxx10000mxxx */
						if (mode == 0) {
							v = kIKindSbcdr;
						} else {
							v = kIKindSbcdm;
						}
						break;
#if Use68020
					case 1:
						/* PACK 1000rrr10100mrrr */
						v = kIKindPack;
						break;
					case 2:
						/* UNPK 1000rrr11000mrrr */
						v = kIKindUnpk;
						break;
#endif
					default:
						v = kIKindIllegal;
						break;
				}
			} else {
				/* OR 1000ddd1ssmmmrrr */
				v = CheckDataAltAddrMode(kIKindOrDEa);
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCode9(void)
{
	ui3b v;

	if (b76 == 3) {
		/* SUBA 1001dddm11mmmrrr */
		v = CheckValidAddrMode(kIKindSubA);
	} else {
		if (b8 == 0) {
			/* SUB 1001ddd0ssmmmrrr */
			v = CheckValidAddrMode(kIKindSubEaR);
		} else {
			if (mode == 0) {
				/* SUBX 1001ddd1ss000rrr */
				v = kIKindSubXd;
			} else if (mode == 1) {
				/* SUBX 1001ddd1ss001rrr */
				v = kIKindSubXm;
			} else {
				/* SUB 1001ddd1ssmmmrrr */
				v = CheckAltMemAddrMode(kIKindSubREa);
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCodeB(void)
{
	ui3b v;

	if (b76 == 3) {
		/* CMPA 1011ddds11mmmrrr */
		v = CheckValidAddrMode(kIKindCmpA);
	} else if (b8 == 1) {
		if (mode == 1) {
			/* CmpM 1011ddd1ss001rrr */
			v = kIKindCmpM;
		} else {
			/* Eor 1011ddd1ssmmmrrr */
			v = CheckDataAltAddrMode(kIKindEor);
		}
	} else {
		/* Cmp 1011ddd0ssmmmrrr */
		v = CheckValidAddrMode(kIKindCmp);
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCodeC(void)
{
	ui3b v;

	if (b76 == 3) {
		if (b8 == 0) {
			/* MulU 1100ddd011mmmrrr */
			v = CheckDataAddrMode(kIKindMulU);
		} else {
			/* MulS 1100ddd111mmmrrr */
			v = CheckDataAddrMode(kIKindMulS);
		}
	} else {
		if (b8 == 0) {
			/* And 1100ddd0ssmmmrrr */
			v = CheckDataAddrMode(kIKindAndEaD);
		} else {
			if (mode < 2) {
				switch (b76) {
					case 0:
						/* ABCD 1100ddd10000mrrr */
						if (mode == 0) {
							v = kIKindAbcdr;
						} else {
							v = kIKindAbcdm;
						}
						break;
					case 1:
						/* Exg 1100ddd10100trrr */
						if (mode == 0) {
							v = kIKindExgdd;
						} else {
							v = kIKindExgaa;
						}
						break;
					case 2:
					default: /* keep compiler happy */
						if (mode == 0) {
							v = kIKindIllegal;
						} else {
							/* Exg 1100ddd110001rrr */
							v = kIKindExgda;
						}
						break;
				}
			} else {
				/* And 1100ddd1ssmmmrrr */
				v = CheckAltMemAddrMode(kIKindAndDEa);
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCodeD(void)
{
	ui3b v;

	if (b76 == 3) {
		/* ADDA 1101dddm11mmmrrr */
		v = CheckValidAddrMode(kIKindAddA);
	} else {
		if (b8 == 0) {
			/* ADD 1101ddd0ssmmmrrr */
			v = CheckValidAddrMode(kIKindAddEaR);
		} else {
			if (mode == 0) {
				v = kIKindAddXd;
			} else if (mode == 1) {
				v = kIKindAddXm;
			} else {
				/* ADD 1101ddd1ssmmmrrr */
				v = CheckAltMemAddrMode(kIKindAddREa);
			}
		}
	}

	return v;
}

LOCALFUNC MayInline ui3b DeCodeE(void)
{
	ui3b v;

	if (b76 == 3) {
		if ((opcode & 0x0800) != 0) {
#if Use68020
			/* 11101???11mmmrrr */
			switch (mode) {
				case 1:
				case 3:
				case 4:
				default: /* keep compiler happy */
					v = kIKindIllegal;
					break;
				case 0:
				case 2:
				case 5:
				case 6:
					v = kIKindBitField;
					break;
				case 7:
					switch (reg) {
						case 0:
						case 1:
							v = kIKindBitField;
							break;
						case 2:
						case 3:
							switch ((opcode >> 8) & 7) {
								case 0: /* BFTST */
								case 1: /* BFEXTU */
								case 3: /* BFEXTS */
								case 5: /* BFFFO */
									v = kIKindBitField;
									break;
								default:
									v = kIKindIllegal;
									break;
							}
							break;
						default:
							v = kIKindIllegal;
							break;
					}
					break;
			}
#else
			v = kIKindIllegal;
#endif
		} else {
			/* 11100ttd11mmmddd */
			v = CheckAltMemAddrMode(kIKindRolopNM);
		}
	} else {
		if (mode < 4) {
			/* 1110cccdss0ttddd */
			v = kIKindRolopND;
		} else {
			/* 1110rrrdss1ttddd */
			v = kIKindRolopDD;
		}
	}

	return v;
}

GLOBALPROC M68KITAB_setup(ui3b *p)
{
	ui5b i;
	ui3b v;

	for (i = 0; i < (ui5b)256 * 256; ++i) {
		opcode = i;
		switch (opcode >> 12) {
			case 0x0:
				v = DeCode0();
				break;
			case 0x1:
				v = DeCode1();
				break;
			case 0x2:
				v = DeCode2();
				break;
			case 0x3:
				v = DeCode3();
				break;
			case 0x4:
				v = DeCode4();
				break;
			case 0x5:
				v = DeCode5();
				break;
			case 0x6:
				v = DeCode6();
				break;
			case 0x7:
				v = DeCode7();
				break;
			case 0x8:
				v = DeCode8();
				break;
			case 0x9:
				v = DeCode9();
				break;
			case 0xA:
				v = kIKindA;
				break;
			case 0xB:
				v = DeCodeB();
				break;
			case 0xC:
				v = DeCodeC();
				break;
			case 0xD:
				v = DeCodeD();
				break;
			case 0xE:
				v = DeCodeE();
				break;
			case 0xF:
			default: /* keep compiler happy */
				v = kIKindF;
				break;
		}

		p[opcode] = v;
	}
}
