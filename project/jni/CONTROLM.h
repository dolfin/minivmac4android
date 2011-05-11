/*
	CONTROLM.h

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
	CONTROL Mode
*/

#ifdef CONTROLM_H
#error "header already included"
#else
#define CONTROLM_H
#endif

enum {
#if EnableAltKeysMode
	SpclModeAltKeyText,
#endif
#if UseActvCode
	SpclModeActvCode,
#endif
	SpclModeMessage,
#if UseControlKeys
	SpclModeControl,
#endif

	kNumSpclModes
};

LOCALVAR uimr SpecialModes = 0;

LOCALVAR blnr NeedWholeScreenDraw = falseblnr;

#define SpecialModeSet(i) SpecialModes |= (1 << (i))
#define SpecialModeClr(i) SpecialModes &= ~ (1 << (i))
#define SpecialModeTst(i) (0 != (SpecialModes & (1 << (i))))

#define MacMsgDisplayed SpecialModeTst(SpclModeMessage)

#ifndef NeedIntlChars
#define NeedIntlChars 0
#endif

/* master copy of private font data */
/*
	Data in commments:
	Mini vMac Cell name
	Mac Roman
	windows-1252 code page
	Unicode
	plain ascii
	ClStrAppendSubstCStr encoding
	HTML character entity
*/
LOCALVAR const ui3b CellData[] = {
	/* kCellUpA 101 0x41 0x0041 'A' 'A' A */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x7E,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpB 102 0x42 0x0042 'B' 'B' B */
	0x00, 0x00, 0x00, 0x7C, 0x42, 0x42, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpC 103 0x43 0x0043 'C' 'C' C */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x40, 0x40,
	0x40, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpD 104 0x44 0x0044 'D' 'D' D */
	0x00, 0x00, 0x00, 0x7C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpE 105 0x45 0x0045 'E' 'E' E */
	0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpF 106 0x46 0x0046 'F' 'F' F */
	0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpG 107 0x47 0x0047 'G' 'G' G */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x4E,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpH 110 0x48 0x0048 'H' 'H' H */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x7E,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpI 111 0x49 0x0049 'I' 'I' I */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpJ 112 0x4A 0x004A 'J' 'J' J */
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpK 113 0x4B 0x004B 'K' 'K' K */
	0x00, 0x00, 0x00, 0x42, 0x44, 0x48, 0x50, 0x60,
	0x50, 0x48, 0x44, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpL 114 0x4C 0x004C 'L' 'L' L */
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpM 115 0x4D 0x004D 'M' 'M' M */
	0x00, 0x00, 0x00, 0x82, 0xC6, 0xAA, 0x92, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpN 116 0x4E 0x004E 'N' 'N' N */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x62, 0x52, 0x4A,
	0x46, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpO 117 0x4F 0x004F 'O' 'O' O */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpP 120 0x50 0x0050 'P' 'P' P */
	0x00, 0x00, 0x00, 0x7C, 0x42, 0x42, 0x42, 0x7C,
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpQ 121 0x51 0x0051 'Q' 'Q' Q */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x10, 0x0C, 0x00, 0x00,
	/* kCellUpR 122 0x52 0x0052 'R' 'R' R */
	0x00, 0x00, 0x00, 0x7C, 0x42, 0x42, 0x42, 0x7C,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpS 123 0x53 0x0053 'S' 'S' S */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x3C,
	0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpT 124 0x54 0x0054 'T' 'T' T */
	0x00, 0x00, 0x00, 0x7F, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpU 125 0x55 0x0055 'U' 'U' U */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpV 126 0x56 0x0056 'V' 'V' V */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x44,
	0x48, 0x50, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpW 127 0x57 0x0057 'W' 'W' W */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x5A, 0x66, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpX 130 0x58 0x0058 'X' 'X' X */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x18,
	0x24, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpY 131 0x59 0x0059 'Y' 'Y' Y */
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x14,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpZ 132 0x5A 0x005A 'Z' 'Z' Z */
	0x00, 0x00, 0x00, 0x7E, 0x02, 0x04, 0x08, 0x10,
	0x20, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoA 141 0x61 0x0061 'a' 'a' a */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoB 142 0x62 0x0062 'b' 'b' b */
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoC 143 0x63 0x0063 'c' 'c' c */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42,
	0x40, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoD 144 0x64 0x0064 'd' 'd' d */
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x3E, 0x42,
	0x42, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoE 145 0x65 0x0065 'e' 'e' e */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42,
	0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoF 146 0x66 0x0066 'f' 'f' f */
	0x00, 0x00, 0x00, 0x0E, 0x10, 0x10, 0x3C, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoG 147 0x67 0x0067 'g' 'g' g */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C, 0x00,
	/* kCellLoH 150 0x68 0x0068 'h' 'h' h */
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoI 151 0x69 0x0069 'i' 'i' i */
	0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoJ 152 0x6A 0x006A 'j' 'j' j */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x70, 0x00,
	/* kCellLoK 153 0x6B 0x006B 'k' 'k' k */
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x44, 0x48,
	0x70, 0x48, 0x44, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoL 154 0x6C 0x006C 'l' 'l' l */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoM 155 0x6D 0x006D 'm' 'm' m */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x92,
	0x92, 0x92, 0x92, 0x92, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoN 156 0x6E 0x006E 'n' 'n' n */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoO 157 0x6F 0x006F 'o' 'o' o */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoP 160 0x70 0x0070 'p' 'p' p */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x00, 0x00,
	/* kCellLoQ 161 0x71 0x0071 'q' 'q' q */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x42, 0x3E, 0x02, 0x02, 0x00, 0x00,
	/* kCellLoR 162 0x72 0x0072 'r' 'r' r */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x62,
	0x42, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoS 163 0x73 0x0073 's' 's' s */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42,
	0x3C, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoT 164 0x74 0x0074 't' 't' t */
	0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x78, 0x20,
	0x20, 0x20, 0x20, 0x1C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoU 165 0x75 0x0075 'u' 'u' u */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoV 166 0x76 0x0076 'v' 'v' v */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x44,
	0x48, 0x50, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoW 167 0x77 0x0077 'w' 'w' w */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x92,
	0x92, 0x92, 0x92, 0x6C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoX 170 0x78 0x0078 'x' 'x' x */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x24,
	0x18, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoY 171 0x79 0x0079 'y' 'y' y */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C, 0x00,
	/* kCellLoZ 172 0x7A 0x007A 'z' 'z' z */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x04,
	0x08, 0x10, 0x20, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit0 060 0x30 0x0030 '0' '0' 0 */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit1 061 0x31 0x0031 '1' '1' 1 */
	0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit2 062 0x32 0x0032 '2' '2' 2 */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x02, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit3 063 0x33 0x0033 '3' '3' 3 */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x02, 0x0C, 0x02,
	0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit4 064 0x34 0x0034 '4' '4' 4 */
	0x00, 0x00, 0x00, 0x04, 0x0C, 0x14, 0x24, 0x7E,
	0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit5 065 0x35 0x0035 '5' '5' 5 */
	0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x7C, 0x02,
	0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit6 066 0x36 0x0036 '6' '6' 6 */
	0x00, 0x00, 0x00, 0x1C, 0x20, 0x40, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit7 067 0x37 0x0037 '7' '7' 7 */
	0x00, 0x00, 0x00, 0x7E, 0x02, 0x02, 0x04, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit8 070 0x38 0x0038 '8' '8' 8 */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellDigit9 071 0x39 0x0039 '9' '9' 9 */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x3E, 0x02, 0x04, 0x38, 0x00, 0x00, 0x00, 0x00,
	/* kCellExclamation 041 0x21 0x0021 '!' '!' ! */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellAmpersand 046 0x26 0x0026 '&' '&' amp */
	0x00, 0x00, 0x00, 0x30, 0x48, 0x48, 0x50, 0x20,
	0x50, 0x4A, 0x44, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellApostrophe 047 0x27 0x0027 '\047' ';la' apos */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellLeftParen 050 0x28 0x0028 '(' '(' ( */
	0x00, 0x00, 0x04, 0x08, 0x08, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x08, 0x08, 0x04, 0x00, 0x00, 0x00,
	/* kCellRightParen 051 0x29 0x0029 ')' ')' ) */
	0x00, 0x00, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00,
	/* kCellComma 054 0x2C 0x002C ',' ',' , */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x10, 0x00, 0x00,
	/* kCellHyphen 055 0x2D 0x002D '-' '-' - */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellPeriod 056 0x2E 0x002E '.' '.' . */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellSlash 057 0x2F 0x002F '/' '/' / */
	0x00, 0x00, 0x00, 0x02, 0x04, 0x04, 0x08, 0x08,
	0x10, 0x10, 0x20, 0x20, 0x40, 0x00, 0x00, 0x00,
	/* kCellColon 072 0x3A 0x003A ':' ':' : */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
	0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
	/* kCellSemicolon 073 0x3B 0x003B ';' ';ls' #59 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x10, 0x00, 0x00,
	/* kCellQuestion 077 0x3F 0x003F '?' '?' ? */
	0x00, 0x00, 0x00, 0x38, 0x44, 0x04, 0x08, 0x10,
	0x10, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellEllipsis 311 0x85 0x2026 '_' ';ll' #8230 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00,
	/* kCellUnderscore 137 0x5F 0x005F '_' '_' _ */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
	/* kCellLeftDQuote 322 0x93 0x201C '"' ';[' ldquo */
	0x00, 0x00, 0x00, 0x24, 0x48, 0x6C, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellRightDQuote 323 0x94 0x201D '"' ';{' rdquo */
	0x00, 0x00, 0x00, 0x36, 0x12, 0x24, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellLeftSQuote 324 0x91 0x2018 '\047' ';]' lsquo */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellRightSQuote 325 0x92 0x2019  '\047' ';}' rsquo */
	0x00, 0x00, 0x00, 0x18, 0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellCopyright 251 0xA9 0x00A9 'c' ';g' copy */
	0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x9A, 0xA2,
	0xA2, 0x9A, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
	/* kCellSpace 040 0x20 0x0020 '\040' '' #32 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

#if NeedIntlChars
	/* kCellUpADiaeresis 200 0xC4 0x00C4 'A' ';uA' Auml */
	0x00, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x7E,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpARing 201 0xC5 0x00C5 'A' ';A' Aring */
	0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x7E,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpCCedilla 202 0xC7 0x00C7 'C' ';C' Ccedil */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x40, 0x40,
	0x40, 0x40, 0x42, 0x3C, 0x08, 0x08, 0x10, 0x00,
	/* kCellUpEAcute 203 0xC9 0x00C9 'E' ';eE' Eacute */
	0x08, 0x10, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpNTilde 204 0xD1 0x00D1 'N' ';nN' Ntilde */
	0x32, 0x4C, 0x00, 0x42, 0x42, 0x62, 0x52, 0x4A,
	0x46, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpODiaeresis 205 0xD6 0x00D6 'O' ';uO' Ouml */
	0x00, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpUDiaeresis 206 0xDC 0x00DC 'U' ';uU' Uuml */
	0x00, 0x24, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoAAcute 207 0xE1 0x00E1 'a' ';ea' aacute */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoAGrave 210 0xE0 0x00E0 'a' ';`a' agrave */
	0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoACircumflex 211 0xE2 0x00E2 'a' ';ia' acirc */
	0x00, 0x00, 0x00, 0x18, 0x24, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoADiaeresis 212 0xE4 0x00E4 'a' ';ua' auml */
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoATilde 213 0xE3 0x00E3 'a' ';na' atilde */
	0x00, 0x00, 0x00, 0x32, 0x4C, 0x00, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoARing 214 0xE5 0x00E5 'a' ';a' aring */
	0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x3E, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoCCedilla 215 0xE7 0x00E7 'c' ';c' ccedil */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42,
	0x40, 0x40, 0x42, 0x3C, 0x08, 0x08, 0x10, 0x00,
	/* kCellLoEAcute 216 0xE9 0x00E9 'e' ';ee' eacute */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x3C, 0x42,
	0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoEGrave 217 0xE8 0x00E8 'e' ';`e' egrave */
	0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x3C, 0x42,
	0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoECircumflex 220 0xEA 0x00EA 'e' ';ie' ecirc */
	0x00, 0x00, 0x00, 0x18, 0x24, 0x00, 0x3C, 0x42,
	0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoEDiaeresis 221 0xEB 0x00EB 'e' ';ue' euml */
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x3C, 0x42,
	0x7E, 0x40, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoIAcute 222 0xED 0x00ED 'i' ';ei' iacute */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoIGrave 223 0xEC 0x00EC 'i' ';`i' igrave */
	0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoICircumflex 224 0xEE 0x00EE 'i' ';ii' icirc */
	0x00, 0x00, 0x00, 0x10, 0x28, 0x00, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoIDiaeresis 225 0xEF 0x00EF 'i' ';ui' iuml */
	0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoNTilde 226 0xF1 0x00F1 'n' ';nn' ntilde */
	0x00, 0x00, 0x00, 0x32, 0x4C, 0x00, 0x7C, 0x42,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoOAcute 227 0xF3 0x00F3 'o' ';eo' oacute */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoOGrave 230 0xF2 0x00F2 'o' ';`o' ograve */
	0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoOCircumflex 231 0xF4 0x00F4 'o' ';io' ocirc */
	0x00, 0x00, 0x00, 0x18, 0x24, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoODiaeresis 232 0xF6 0x00F6 'o' ';uo' ouml */
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoOTilde 233 0xF5 0x00F5 'o' ';no' otilde */
	0x00, 0x00, 0x00, 0x32, 0x4C, 0x00, 0x3C, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoUAcute 234 0xFA 0x00FA 'u' ';eu' uacute */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoUGrave 235 0xF9 0x00F9 'u' ';`u' ugrave */
	0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoUCircumflex 236 0xFB 0x00FB 'u' ';iu' ucirc */
	0x00, 0x00, 0x00, 0x18, 0x24, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoUDiaeresis 237 0xFC 0x00FC 'u' ';uu' uuml */
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00,

	/* kCellUpAE 256 0xC6 0x00C6 '?' ';lE' AElig */
	0x00, 0x00, 0x00, 0x3E, 0x48, 0x48, 0x48, 0x7C,
	0x48, 0x48, 0x48, 0x4E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpOStroke 257 0xD8 0x00D8 'O' ';O' Oslash */
	0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x46, 0x5A,
	0x62, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,

	/* kCellLoAE 276 0xE6 0x00E6 '?' ';le' aelig */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x52,
	0x5E, 0x50, 0x52, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoOStroke 277 0xF8 0x00F8 'o' ';o' oslash */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x46,
	0x5A, 0x62, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellInvQuestion 300 0xBF 0x00BF '?' ';?' iquest */
	0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10,
	0x20, 0x40, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
	/* kCellInvExclam 301 0xA1 0x00A1 '!' ';1' iexcl */
	0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,

	/* kCellUpAGrave 313 0xC0 0x00C0 'A' ';`A' Agrave */
	0x10, 0x08, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x7E, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpATilde 314 0xC3 0x00C3 'A' ';nA' Atilde */
	0x32, 0x4C, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x7E, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpOTilde 315 0xD5 0x00D5 'O' ';nO' Otilde */
	0x32, 0x4C, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpLigatureOE 316 0x8C 0x0152 '?' ';Q' OElig */
	0x00, 0x00, 0x00, 0x3E, 0x48, 0x48, 0x48, 0x4E,
	0x48, 0x48, 0x48, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellLoLigatureOE 317 0x9C 0x0153 '?' ';q' oelig */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x52,
	0x5E, 0x50, 0x52, 0x2C, 0x00, 0x00, 0x00, 0x00,

	/* kCellLoYDiaeresis 330 0xFF 0x00FF 'y' ';uy' yuml */
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C, 0x00,
	/* kCellUpYDiaeresis 331 0x9F 0x0178 'Y' ';uY' Yuml */
	0x00, 0x14, 0x00, 0x22, 0x22, 0x22, 0x22, 0x14,
	0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,

	/* kCellUpACircumflex 345 0xC2 0x00C2 'A' ';iA' Acirc */
	0x18, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x7E,
	0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpECircumflex 346 0xCA 0x00CA 'E' ';iE' Ecirc */
	0x18, 0x24, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpAAcute 347 0xC1 0x00C1 'A' ';eA' Aacute */
	0x08, 0x10, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x7E, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpEDiaeresis 350 0xCB 0x00CB 'E' ';uE' Euml */
	0x00, 0x24, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpEGrave 351 0xC8 0x00C8 'E' ';`E' Egrave */
	0x10, 0x08, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x7C,
	0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpIAcute 352 0xCD 0x00CD 'A' ';eI' Iacute */
	0x04, 0x08, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpICircumflex 353 0xCE 0x00CE 'I' ';iI' Icirc */
	0x08, 0x14, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpIDiaeresis 354 0xCF 0x00CF 'I' ';uI' Iuml */
	0x00, 0x14, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpIGrave 355 0xCC 0x00CC 'I' ';`I' Igrave */
	0x10, 0x08, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpOAcute 356 0xD3 0x00D3 'O' ';eO' Oacute */
	0x08, 0x10, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpOCircumflex 357 0xD4 0x00D4 'O' ';iO' Ocirc */
	0x18, 0x24, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,

	/* kCellUpOGrave 361 0xD2 0x00D2 'O' ';`O' Ograve */
	0x10, 0x08, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpUAcute 362 0xDA 0x00DA 'U' ';eU' Uacute */
	0x08, 0x10, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpUCircumflex 363 0xDB 0x00DB 'U' ';iU' Ucirc */
	0x18, 0x24, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellUpUGrave 364 0xD9 0x00D9 'U' ';`U' Ugrave */
	0x10, 0x08, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42,
	0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00,
	/* kCellSharpS 247 0xDF 0x00DF 'B' ';s' szlig */
	0x00, 0x00, 0x00, 0x1C, 0x22, 0x42, 0x44, 0x44,
	0x42, 0x42, 0x42, 0x5C, 0x40, 0x00, 0x00, 0x00,
#endif

	/* kCellUpperLeft */
	0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	/* kCellUpperMiddle */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellMiddleLeft */
	0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	/* kCellMiddleLeft */
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	/* kCellMiddleRight */
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	/* kCellLowerLeft */
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF,
	/* kCellLowerMiddle */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	/* kCellLowerRight */
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF,
	/* kCellGraySep */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellIcon00 */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09,
	/* kCellIcon01 */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x18, 0x18,
	/* kCellIcon02 */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x18, 0x30,
	/* kCellIcon03 */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xE0, 0x10, 0x10, 0x10, 0x90, 0x90, 0x90,
	/* kCellIcon10 */
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* kCellIcon11 */
	0x18, 0x18, 0x19, 0x1B, 0x1E, 0x1C, 0x18, 0x10,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellIcon12 */
	0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
	/* kCellIcon13 */
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	/* kCellIcon20 */
	0x08, 0x08, 0x08, 0x07, 0x04, 0x04, 0x04, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellIcon21 */
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellIcon22 */
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* kCellIcon23 */
	0x10, 0x10, 0x10, 0xE0, 0x20, 0x20, 0x20, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if EnableAltKeysMode
	/* kInsertText00 */
	0xFF, 0x80, 0x80, 0x80, 0x80, 0x83, 0x80, 0x80,
	0x80, 0x80, 0x83, 0x80, 0x80, 0x80, 0x80, 0xFF,
	/* kInsertText01 */
	0xFF, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x19,
	0x1B, 0x1E, 0x1C, 0x18, 0x10, 0x00, 0x00, 0xFF,
	/* kInsertText02 */
	0xFF, 0x00, 0x00, 0x18, 0x30, 0x60, 0xC0, 0x80,
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0xFF,
	/* kInsertText03 */
	0xFF, 0x01, 0x01, 0x01, 0x01, 0xC1, 0x01, 0x01,
	0x01, 0x01, 0xC1, 0x01, 0x01, 0x01, 0x01, 0xFF,
	/* kInsertText04 */
	0xFF, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x00, 0x00,
	0x00, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0xFF,
#endif

	0x00 /* just so last above line can end in ',' */
};

enum {
	kCellUpA,
	kCellUpB,
	kCellUpC,
	kCellUpD,
	kCellUpE,
	kCellUpF,
	kCellUpG,
	kCellUpH,
	kCellUpI,
	kCellUpJ,
	kCellUpK,
	kCellUpL,
	kCellUpM,
	kCellUpN,
	kCellUpO,
	kCellUpP,
	kCellUpQ,
	kCellUpR,
	kCellUpS,
	kCellUpT,
	kCellUpU,
	kCellUpV,
	kCellUpW,
	kCellUpX,
	kCellUpY,
	kCellUpZ,
	kCellLoA,
	kCellLoB,
	kCellLoC,
	kCellLoD,
	kCellLoE,
	kCellLoF,
	kCellLoG,
	kCellLoH,
	kCellLoI,
	kCellLoJ,
	kCellLoK,
	kCellLoL,
	kCellLoM,
	kCellLoN,
	kCellLoO,
	kCellLoP,
	kCellLoQ,
	kCellLoR,
	kCellLoS,
	kCellLoT,
	kCellLoU,
	kCellLoV,
	kCellLoW,
	kCellLoX,
	kCellLoY,
	kCellLoZ,
	kCellDigit0,
	kCellDigit1,
	kCellDigit2,
	kCellDigit3,
	kCellDigit4,
	kCellDigit5,
	kCellDigit6,
	kCellDigit7,
	kCellDigit8,
	kCellDigit9,
	kCellExclamation,
	kCellAmpersand,
	kCellApostrophe,
	kCellLeftParen,
	kCellRightParen,
	kCellComma,
	kCellHyphen,
	kCellPeriod,
	kCellSlash,
	kCellColon,
	kCellSemicolon,
	kCellQuestion,
	kCellEllipsis,
	kCellUnderscore,
	kCellLeftDQuote,
	kCellRightDQuote,
	kCellLeftSQuote,
	kCellRightSQuote,
	kCellCopyright,
	kCellSpace,

#if NeedIntlChars
	kCellUpADiaeresis,
	kCellUpARing,
	kCellUpCCedilla,
	kCellUpEAcute,
	kCellUpNTilde,
	kCellUpODiaeresis,
	kCellUpUDiaeresis,
	kCellLoAAcute,
	kCellLoAGrave,
	kCellLoACircumflex,
	kCellLoADiaeresis,
	kCellLoATilde,
	kCellLoARing,
	kCellLoCCedilla,
	kCellLoEAcute,
	kCellLoEGrave,
	kCellLoECircumflex,
	kCellLoEDiaeresis,
	kCellLoIAcute,
	kCellLoIGrave,
	kCellLoICircumflex,
	kCellLoIDiaeresis,
	kCellLoNTilde,
	kCellLoOAcute,
	kCellLoOGrave,
	kCellLoOCircumflex,
	kCellLoODiaeresis,
	kCellLoOTilde,
	kCellLoUAcute,
	kCellLoUGrave,
	kCellLoUCircumflex,
	kCellLoUDiaeresis,

	kCellUpAE,
	kCellUpOStroke,

	kCellLoAE,
	kCellLoOStroke,
	kCellInvQuestion,
	kCellInvExclam,

	kCellUpAGrave,
	kCellUpATilde,
	kCellUpOTilde,
	kCellUpLigatureOE,
	kCellLoLigatureOE,

	kCellLoYDiaeresis,
	kCellUpYDiaeresis,

	kCellUpACircumflex,
	kCellUpECircumflex,
	kCellUpAAcute,
	kCellUpEDiaeresis,
	kCellUpEGrave,
	kCellUpIAcute,
	kCellUpICircumflex,
	kCellUpIDiaeresis,
	kCellUpIGrave,
	kCellUpOAcute,
	kCellUpOCircumflex,

	kCellUpOGrave,
	kCellUpUAcute,
	kCellUpUCircumflex,
	kCellUpUGrave,
	kCellSharpS,
#endif

	kCellUpperLeft,
	kCellUpperMiddle,
	kCellUpperRight,
	kCellMiddleLeft,
	kCellMiddleRight,
	kCellLowerLeft,
	kCellLowerMiddle,
	kCellLowerRight,
	kCellGraySep,
	kCellIcon00,
	kCellIcon01,
	kCellIcon02,
	kCellIcon03,
	kCellIcon10,
	kCellIcon11,
	kCellIcon12,
	kCellIcon13,
	kCellIcon20,
	kCellIcon21,
	kCellIcon22,
	kCellIcon23,
#if EnableAltKeysMode
	kInsertText00,
	kInsertText01,
	kInsertText02,
	kInsertText03,
	kInsertText04,
#endif

	kNumCells
};

#ifndef NeedCell2MacAsciiMap
#if UseActvCode
#define NeedCell2MacAsciiMap 1
#else
#define NeedCell2MacAsciiMap 0
#endif
#endif

#if NeedCell2MacAsciiMap
/* Mac Roman character set */
LOCALVAR const char Cell2MacAsciiMap[] = {
	'\101', /* kCellUpA */
	'\102', /* kCellUpB */
	'\103', /* kCellUpC */
	'\104', /* kCellUpD */
	'\105', /* kCellUpE */
	'\106', /* kCellUpF */
	'\107', /* kCellUpG */
	'\110', /* kCellUpH */
	'\111', /* kCellUpI */
	'\112', /* kCellUpJ */
	'\113', /* kCellUpK */
	'\114', /* kCellUpL */
	'\115', /* kCellUpM */
	'\116', /* kCellUpN */
	'\117', /* kCellUpO */
	'\120', /* kCellUpP */
	'\121', /* kCellUpQ */
	'\122', /* kCellUpR */
	'\123', /* kCellUpS */
	'\124', /* kCellUpT */
	'\125', /* kCellUpU */
	'\126', /* kCellUpV */
	'\127', /* kCellUpW */
	'\130', /* kCellUpX */
	'\131', /* kCellUpY */
	'\132', /* kCellUpZ */
	'\141', /* kCellLoA */
	'\142', /* kCellLoB */
	'\143', /* kCellLoC */
	'\144', /* kCellLoD */
	'\145', /* kCellLoE */
	'\146', /* kCellLoF */
	'\147', /* kCellLoG */
	'\150', /* kCellLoH */
	'\151', /* kCellLoI */
	'\152', /* kCellLoJ */
	'\153', /* kCellLoK */
	'\154', /* kCellLoL */
	'\155', /* kCellLoM */
	'\156', /* kCellLoN */
	'\157', /* kCellLoO */
	'\160', /* kCellLoP */
	'\161', /* kCellLoQ */
	'\162', /* kCellLoR */
	'\163', /* kCellLoS */
	'\164', /* kCellLoT */
	'\165', /* kCellLoU */
	'\166', /* kCellLoV */
	'\167', /* kCellLoW */
	'\170', /* kCellLoX */
	'\171', /* kCellLoY */
	'\172', /* kCellLoZ */
	'\060', /* kCellDigit0 */
	'\061', /* kCellDigit1 */
	'\062', /* kCellDigit2 */
	'\063', /* kCellDigit3 */
	'\064', /* kCellDigit4 */
	'\065', /* kCellDigit5 */
	'\066', /* kCellDigit6 */
	'\067', /* kCellDigit7 */
	'\070', /* kCellDigit8 */
	'\071', /* kCellDigit9 */
	'\041', /* kCellExclamation */
	'\046', /* kCellAmpersand */
	'\047', /* kCellApostrophe */
	'\050', /* kCellLeftParen */
	'\051', /* kCellRightParen */
	'\054', /* kCellComma */
	'\055', /* kCellHyphen */
	'\056', /* kCellPeriod */
	'\057', /* kCellSlash */
	'\072', /* kCellColon */
	'\073', /* kCellSemicolon */
	'\077', /* kCellQuestion */
	'\311', /* kCellEllipsis */
	'\137', /* kCellUnderscore */
	'\322', /* kCellLeftDQuote */
	'\323', /* kCellRightDQuote */
	'\324', /* kCellLeftSQuote */
	'\325', /* kCellRightSQuote */
	'\251', /* kCellCopyright */
	'\040', /* kCellSpace */

#if NeedIntlChars
	'\200', /* kCellUpADiaeresis */
	'\201', /* kCellUpARing */
	'\202', /* kCellUpCCedilla */
	'\203', /* kCellUpEAcute */
	'\204', /* kCellUpNTilde */
	'\205', /* kCellUpODiaeresis */
	'\206', /* kCellUpUDiaeresis */
	'\207', /* kCellLoAAcute */
	'\210', /* kCellLoAGrave */
	'\211', /* kCellLoACircumflex */
	'\212', /* kCellLoADiaeresis */
	'\213', /* kCellLoATilde */
	'\214', /* kCellLoARing */
	'\215', /* kCellLoCCedilla */
	'\216', /* kCellLoEAcute */
	'\217', /* kCellLoEGrave */
	'\220', /* kCellLoECircumflex */
	'\221', /* kCellLoEDiaeresis */
	'\222', /* kCellLoIAcute */
	'\223', /* kCellLoIGrave */
	'\224', /* kCellLoICircumflex */
	'\225', /* kCellLoIDiaeresis */
	'\226', /* kCellLoNTilde */
	'\227', /* kCellLoOAcute */
	'\230', /* kCellLoOGrave */
	'\231', /* kCellLoOCircumflex */
	'\232', /* kCellLoODiaeresis */
	'\233', /* kCellLoOTilde */
	'\234', /* kCellLoUAcute */
	'\235', /* kCellLoUGrave */
	'\236', /* kCellLoUCircumflex */
	'\237', /* kCellLoUDiaeresis */

	'\256', /* kCellUpAE */
	'\257', /* kCellUpOStroke */

	'\276', /* kCellLoAE */
	'\277', /* kCellLoOStroke */
	'\300', /* kCellInvQuestion */
	'\301', /* kCellInvExclam */

	'\313', /* kCellUpAGrave */
	'\314', /* kCellUpATilde */
	'\315', /* kCellUpOTilde */
	'\316', /* kCellUpLigatureOE */
	'\317', /* kCellLoLigatureOE */

	'\330', /* kCellLoYDiaeresis */
	'\331', /* kCellUpYDiaeresis */

	'\345', /* kCellUpACircumflex */
	'\346', /* kCellUpECircumflex */
	'\347', /* kCellUpAAcute */
	'\350', /* kCellUpEDiaeresis */
	'\351', /* kCellUpEGrave */
	'\352', /* kCellUpIAcute */
	'\353', /* kCellUpICircumflex */
	'\354', /* kCellUpIDiaeresis */
	'\355', /* kCellUpIGrave */
	'\356', /* kCellUpOAcute */
	'\357', /* kCellUpOCircumflex */

	'\361', /* kCellUpOGrave */
	'\362', /* kCellUpUAcute */
	'\363', /* kCellUpUCircumflex */
	'\364', /* kCellUpUGrave */
	'\247', /* kCellSharpS */
#endif

	'\0' /* just so last above line can end in ',' */
};
#endif

#ifndef NeedCell2WinAsciiMap
#define NeedCell2WinAsciiMap 0
#endif

#if NeedCell2WinAsciiMap
/* Windows character set (windows-1252 code page) */
LOCALVAR const ui3b Cell2WinAsciiMap[] = {
	0x41, /* kCellUpA */
	0x42, /* kCellUpB */
	0x43, /* kCellUpC */
	0x44, /* kCellUpD */
	0x45, /* kCellUpE */
	0x46, /* kCellUpF */
	0x47, /* kCellUpG */
	0x48, /* kCellUpH */
	0x49, /* kCellUpI */
	0x4A, /* kCellUpJ */
	0x4B, /* kCellUpK */
	0x4C, /* kCellUpL */
	0x4D, /* kCellUpM */
	0x4E, /* kCellUpN */
	0x4F, /* kCellUpO */
	0x50, /* kCellUpP */
	0x51, /* kCellUpQ */
	0x52, /* kCellUpR */
	0x53, /* kCellUpS */
	0x54, /* kCellUpT */
	0x55, /* kCellUpU */
	0x56, /* kCellUpV */
	0x57, /* kCellUpW */
	0x58, /* kCellUpX */
	0x59, /* kCellUpY */
	0x5A, /* kCellUpZ */
	0x61, /* kCellLoA */
	0x62, /* kCellLoB */
	0x63, /* kCellLoC */
	0x64, /* kCellLoD */
	0x65, /* kCellLoE */
	0x66, /* kCellLoF */
	0x67, /* kCellLoG */
	0x68, /* kCellLoH */
	0x69, /* kCellLoI */
	0x6A, /* kCellLoJ */
	0x6B, /* kCellLoK */
	0x6C, /* kCellLoL */
	0x6D, /* kCellLoM */
	0x6E, /* kCellLoN */
	0x6F, /* kCellLoO */
	0x70, /* kCellLoP */
	0x71, /* kCellLoQ */
	0x72, /* kCellLoR */
	0x73, /* kCellLoS */
	0x74, /* kCellLoT */
	0x75, /* kCellLoU */
	0x76, /* kCellLoV */
	0x77, /* kCellLoW */
	0x78, /* kCellLoX */
	0x79, /* kCellLoY */
	0x7A, /* kCellLoZ */
	0x30, /* kCellDigit0 */
	0x31, /* kCellDigit1 */
	0x32, /* kCellDigit2 */
	0x33, /* kCellDigit3 */
	0x34, /* kCellDigit4 */
	0x35, /* kCellDigit5 */
	0x36, /* kCellDigit6 */
	0x37, /* kCellDigit7 */
	0x38, /* kCellDigit8 */
	0x39, /* kCellDigit9 */
	0x21, /* kCellExclamation */
	0x26, /* kCellAmpersand */
	0x27, /* kCellApostrophe */
	0x28, /* kCellLeftParen */
	0x29, /* kCellRightParen */
	0x2C, /* kCellComma */
	0x2D, /* kCellHyphen */
	0x2E, /* kCellPeriod */
	0x2F, /* kCellSlash */
	0x3A, /* kCellColon */
	0x3B, /* kCellSemicolon */
	0x3F, /* kCellQuestion */
	0x85, /* kCellEllipsis */
	0x5F, /* kCellUnderscore */
	0x93, /* kCellLeftDQuote */
	0x94, /* kCellRightDQuote */
	0x91, /* kCellLeftSQuote */
	0x92, /* kCellRightSQuote */
	0xA9, /* kCellCopyright */
	0x20, /* kCellSpace */

#if NeedIntlChars
	0xC4, /* kCellUpADiaeresis */
	0xC5, /* kCellUpARing */
	0xC7, /* kCellUpCCedilla */
	0xC9, /* kCellUpEAcute */
	0xD1, /* kCellUpNTilde */
	0xD6, /* kCellUpODiaeresis */
	0xDC, /* kCellUpUDiaeresis */
	0xE1, /* kCellLoAAcute */
	0xE0, /* kCellLoAGrave */
	0xE2, /* kCellLoACircumflex */
	0xE4, /* kCellLoADiaeresis */
	0xE3, /* kCellLoATilde */
	0xE5, /* kCellLoARing */
	0xE7, /* kCellLoCCedilla */
	0xE9, /* kCellLoEAcute */
	0xE8, /* kCellLoEGrave */
	0xEA, /* kCellLoECircumflex */
	0xEB, /* kCellLoEDiaeresis */
	0xED, /* kCellLoIAcute */
	0xEC, /* kCellLoIGrave */
	0xEE, /* kCellLoICircumflex */
	0xEF, /* kCellLoIDiaeresis */
	0xF1, /* kCellLoNTilde */
	0xF3, /* kCellLoOAcute */
	0xF2, /* kCellLoOGrave */
	0xF4, /* kCellLoOCircumflex */
	0xF6, /* kCellLoODiaeresis */
	0xF5, /* kCellLoOTilde */
	0xFA, /* kCellLoUAcute */
	0xF9, /* kCellLoUGrave */
	0xFB, /* kCellLoUCircumflex */
	0xFC, /* kCellLoUDiaeresis */

	0xC6, /* kCellUpAE */
	0xD8, /* kCellUpOStroke */

	0xE6, /* kCellLoAE */
	0xF8, /* kCellLoOStroke */
	0xBF, /* kCellInvQuestion */
	0xA1, /* kCellInvExclam */

	0xC0, /* kCellUpAGrave */
	0xC3, /* kCellUpATilde */
	0xD5, /* kCellUpOTilde */
	0x8C, /* kCellUpLigatureOE */
	0x9C, /* kCellLoLigatureOE */

	0xFF, /* kCellLoYDiaeresis */
	0x9F, /* kCellUpYDiaeresis */

	0xC2, /* kCellUpACircumflex */
	0xCA, /* kCellUpECircumflex */
	0xC1, /* kCellUpAAcute */
	0xCB, /* kCellUpEDiaeresis */
	0xC8, /* kCellUpEGrave */
	0xCD, /* kCellUpIAcute */
	0xCE, /* kCellUpICircumflex */
	0xCF, /* kCellUpIDiaeresis */
	0xCC, /* kCellUpIGrave */
	0xD3, /* kCellUpOAcute */
	0xD4, /* kCellUpOCircumflex */

	0xD2, /* kCellUpOGrave */
	0xDA, /* kCellUpUAcute */
	0xDB, /* kCellUpUCircumflex */
	0xD9, /* kCellUpUGrave */
	0xDF, /* kCellSharpS */
#endif

	'\0' /* just so last above line can end in ',' */
};
#endif

#ifndef NeedCell2PlainAsciiMap
#define NeedCell2PlainAsciiMap 0
#endif

#if NeedCell2PlainAsciiMap
/* Plain ascii - remove accents when possible */
LOCALVAR const char Cell2PlainAsciiMap[] = {
	'A', /* kCellUpA */
	'B', /* kCellUpB */
	'C', /* kCellUpC */
	'D', /* kCellUpD */
	'E', /* kCellUpE */
	'F', /* kCellUpF */
	'G', /* kCellUpG */
	'H', /* kCellUpH */
	'I', /* kCellUpI */
	'J', /* kCellUpJ */
	'K', /* kCellUpK */
	'L', /* kCellUpL */
	'M', /* kCellUpM */
	'N', /* kCellUpN */
	'O', /* kCellUpO */
	'P', /* kCellUpP */
	'Q', /* kCellUpQ */
	'R', /* kCellUpR */
	'S', /* kCellUpS */
	'T', /* kCellUpT */
	'U', /* kCellUpU */
	'V', /* kCellUpV */
	'W', /* kCellUpW */
	'X', /* kCellUpX */
	'Y', /* kCellUpY */
	'Z', /* kCellUpZ */
	'a', /* kCellLoA */
	'b', /* kCellLoB */
	'c', /* kCellLoC */
	'd', /* kCellLoD */
	'e', /* kCellLoE */
	'f', /* kCellLoF */
	'g', /* kCellLoG */
	'h', /* kCellLoH */
	'i', /* kCellLoI */
	'j', /* kCellLoJ */
	'k', /* kCellLoK */
	'l', /* kCellLoL */
	'm', /* kCellLoM */
	'n', /* kCellLoN */
	'o', /* kCellLoO */
	'p', /* kCellLoP */
	'q', /* kCellLoQ */
	'r', /* kCellLoR */
	's', /* kCellLoS */
	't', /* kCellLoT */
	'u', /* kCellLoU */
	'v', /* kCellLoV */
	'w', /* kCellLoW */
	'x', /* kCellLoX */
	'y', /* kCellLoY */
	'z', /* kCellLoZ */
	'0', /* kCellDigit0 */
	'1', /* kCellDigit1 */
	'2', /* kCellDigit2 */
	'3', /* kCellDigit3 */
	'4', /* kCellDigit4 */
	'5', /* kCellDigit5 */
	'6', /* kCellDigit6 */
	'7', /* kCellDigit7 */
	'8', /* kCellDigit8 */
	'9', /* kCellDigit9 */
	'!', /* kCellExclamation */
	'&', /* kCellAmpersand */
	'\047', /* kCellApostrophe */
	'(', /* kCellLeftParen */
	')', /* kCellRightParen */
	',', /* kCellComma */
	'-', /* kCellHyphen */
	'.', /* kCellPeriod */
	'/', /* kCellSlash */
	':', /* kCellColon */
	';', /* kCellSemicolon */
	'?', /* kCellQuestion */
	'_', /* kCellEllipsis */
	'_', /* kCellUnderscore */
	'"', /* kCellLeftDQuote */
	'"', /* kCellRightDQuote */
	'\047', /* kCellLeftSQuote */
	'\047', /* kCellRightSQuote */
	'c', /* kCellCopyright */
	' ', /* kCellSpace */

#if NeedIntlChars
	'A', /* kCellUpADiaeresis */
	'A', /* kCellUpARing */
	'C', /* kCellUpCCedilla */
	'E', /* kCellUpEAcute */
	'N', /* kCellUpNTilde */
	'O', /* kCellUpODiaeresis */
	'U', /* kCellUpUDiaeresis */
	'a', /* kCellLoAAcute */
	'a', /* kCellLoAGrave */
	'a', /* kCellLoACircumflex */
	'a', /* kCellLoADiaeresis */
	'a', /* kCellLoATilde */
	'a', /* kCellLoARing */
	'c', /* kCellLoCCedilla */
	'e', /* kCellLoEAcute */
	'e', /* kCellLoEGrave */
	'e', /* kCellLoECircumflex */
	'e', /* kCellLoEDiaeresis */
	'i', /* kCellLoIAcute */
	'i', /* kCellLoIGrave */
	'i', /* kCellLoICircumflex */
	'i', /* kCellLoIDiaeresis */
	'n', /* kCellLoNTilde */
	'o', /* kCellLoOAcute */
	'o', /* kCellLoOGrave */
	'o', /* kCellLoOCircumflex */
	'o', /* kCellLoODiaeresis */
	'o', /* kCellLoOTilde */
	'u', /* kCellLoUAcute */
	'u', /* kCellLoUGrave */
	'u', /* kCellLoUCircumflex */
	'u', /* kCellLoUDiaeresis */

	'?', /* kCellUpAE */
	'O', /* kCellUpOStroke */

	'?', /* kCellLoAE */
	'o', /* kCellLoOStroke */
	'?', /* kCellInvQuestion */
	'!', /* kCellInvExclam */

	'A', /* kCellUpAGrave */
	'A', /* kCellUpATilde */
	'O', /* kCellUpOTilde */
	'?', /* kCellUpLigatureOE */
	'?', /* kCellLoLigatureOE */

	'y', /* kCellLoYDiaeresis */
	'Y', /* kCellUpYDiaeresis */

	'A', /* kCellUpACircumflex */
	'E', /* kCellUpECircumflex */
	'A', /* kCellUpAAcute */
	'E', /* kCellUpEDiaeresis */
	'E', /* kCellUpEGrave */
	'A', /* kCellUpIAcute */
	'I', /* kCellUpICircumflex */
	'I', /* kCellUpIDiaeresis */
	'I', /* kCellUpIGrave */
	'O', /* kCellUpOAcute */
	'O', /* kCellUpOCircumflex */

	'O', /* kCellUpOGrave */
	'U', /* kCellUpUAcute */
	'U', /* kCellUpUCircumflex */
	'U', /* kCellUpUGrave */
	'B', /* kCellSharpS */
#endif

	'\0' /* just so last above line can end in ',' */
};
#endif

#ifndef NeedCell2UnicodeMap
#define NeedCell2UnicodeMap 0
#endif

#if NeedCell2UnicodeMap
/* Unicode character set */
LOCALVAR const ui4b Cell2UnicodeMap[] = {
	0x0041, /* kCellUpA */
	0x0042, /* kCellUpB */
	0x0043, /* kCellUpC */
	0x0044, /* kCellUpD */
	0x0045, /* kCellUpE */
	0x0046, /* kCellUpF */
	0x0047, /* kCellUpG */
	0x0048, /* kCellUpH */
	0x0049, /* kCellUpI */
	0x004A, /* kCellUpJ */
	0x004B, /* kCellUpK */
	0x004C, /* kCellUpL */
	0x004D, /* kCellUpM */
	0x004E, /* kCellUpN */
	0x004F, /* kCellUpO */
	0x0050, /* kCellUpP */
	0x0051, /* kCellUpQ */
	0x0052, /* kCellUpR */
	0x0053, /* kCellUpS */
	0x0054, /* kCellUpT */
	0x0055, /* kCellUpU */
	0x0056, /* kCellUpV */
	0x0057, /* kCellUpW */
	0x0058, /* kCellUpX */
	0x0059, /* kCellUpY */
	0x005A, /* kCellUpZ */
	0x0061, /* kCellLoA */
	0x0062, /* kCellLoB */
	0x0063, /* kCellLoC */
	0x0064, /* kCellLoD */
	0x0065, /* kCellLoE */
	0x0066, /* kCellLoF */
	0x0067, /* kCellLoG */
	0x0068, /* kCellLoH */
	0x0069, /* kCellLoI */
	0x006A, /* kCellLoJ */
	0x006B, /* kCellLoK */
	0x006C, /* kCellLoL */
	0x006D, /* kCellLoM */
	0x006E, /* kCellLoN */
	0x006F, /* kCellLoO */
	0x0070, /* kCellLoP */
	0x0071, /* kCellLoQ */
	0x0072, /* kCellLoR */
	0x0073, /* kCellLoS */
	0x0074, /* kCellLoT */
	0x0075, /* kCellLoU */
	0x0076, /* kCellLoV */
	0x0077, /* kCellLoW */
	0x0078, /* kCellLoX */
	0x0079, /* kCellLoY */
	0x007A, /* kCellLoZ */
	0x0030, /* kCellDigit0 */
	0x0031, /* kCellDigit1 */
	0x0032, /* kCellDigit2 */
	0x0033, /* kCellDigit3 */
	0x0034, /* kCellDigit4 */
	0x0035, /* kCellDigit5 */
	0x0036, /* kCellDigit6 */
	0x0037, /* kCellDigit7 */
	0x0038, /* kCellDigit8 */
	0x0039, /* kCellDigit9 */
	0x0021, /* kCellExclamation */
	0x0026, /* kCellAmpersand */
	0x0027, /* kCellApostrophe */
	0x0028, /* kCellLeftParen */
	0x0029, /* kCellRightParen */
	0x002C, /* kCellComma */
	0x002D, /* kCellHyphen */
	0x002E, /* kCellPeriod */
	0x002F, /* kCellSlash */
	0x003A, /* kCellColon */
	0x003B, /* kCellSemicolon */
	0x003F, /* kCellQuestion */
	0x2026, /* kCellEllipsis */
	0x005F, /* kCellUnderscore */
	0x201C, /* kCellLeftDQuote */
	0x201D, /* kCellRightDQuote */
	0x2018, /* kCellLeftSQuote */
	0x2019, /* kCellRightSQuote */
	0x00A9, /* kCellCopyright */
	0x0020, /* kCellSpace */

#if NeedIntlChars
	0x00C4, /* kCellUpADiaeresis */
	0x00C5, /* kCellUpARing */
	0x00C7, /* kCellUpCCedilla */
	0x00C9, /* kCellUpEAcute */
	0x00D1, /* kCellUpNTilde */
	0x00D6, /* kCellUpODiaeresis */
	0x00DC, /* kCellUpUDiaeresis */
	0x00E1, /* kCellLoAAcute */
	0x00E0, /* kCellLoAGrave */
	0x00E2, /* kCellLoACircumflex */
	0x00E4, /* kCellLoADiaeresis */
	0x00E3, /* kCellLoATilde */
	0x00E5, /* kCellLoARing */
	0x00E7, /* kCellLoCCedilla */
	0x00E9, /* kCellLoEAcute */
	0x00E8, /* kCellLoEGrave */
	0x00EA, /* kCellLoECircumflex */
	0x00EB, /* kCellLoEDiaeresis */
	0x00ED, /* kCellLoIAcute */
	0x00EC, /* kCellLoIGrave */
	0x00EE, /* kCellLoICircumflex */
	0x00EF, /* kCellLoIDiaeresis */
	0x00F1, /* kCellLoNTilde */
	0x00F3, /* kCellLoOAcute */
	0x00F2, /* kCellLoOGrave */
	0x00F4, /* kCellLoOCircumflex */
	0x00F6, /* kCellLoODiaeresis */
	0x00F5, /* kCellLoOTilde */
	0x00FA, /* kCellLoUAcute */
	0x00F9, /* kCellLoUGrave */
	0x00FB, /* kCellLoUCircumflex */
	0x00FC, /* kCellLoUDiaeresis */

	0x00C6, /* kCellUpAE */
	0x00D8, /* kCellUpOStroke */

	0x00E6, /* kCellLoAE */
	0x00F8, /* kCellLoOStroke */
	0x00BF, /* kCellInvQuestion */
	0x00A1, /* kCellInvExclam */

	0x00C0, /* kCellUpAGrave */
	0x00C3, /* kCellUpATilde */
	0x00D5, /* kCellUpOTilde */
	0x0152, /* kCellUpLigatureOE */
	0x0153, /* kCellLoLigatureOE */

	0x00FF, /* kCellLoYDiaeresis */
	0x0178, /* kCellUpYDiaeresis */

	0x00C2, /* kCellUpACircumflex */
	0x00CA, /* kCellUpECircumflex */
	0x00C1, /* kCellUpAAcute */
	0x00CB, /* kCellUpEDiaeresis */
	0x00C8, /* kCellUpEGrave */
	0x00CD, /* kCellUpIAcute */
	0x00CE, /* kCellUpICircumflex */
	0x00CF, /* kCellUpIDiaeresis */
	0x00CC, /* kCellUpIGrave */
	0x00D3, /* kCellUpOAcute */
	0x00D4, /* kCellUpOCircumflex */

	0x00D2, /* kCellUpOGrave */
	0x00DA, /* kCellUpUAcute */
	0x00DB, /* kCellUpUCircumflex */
	0x00D9, /* kCellUpUGrave */
	0x00DF, /* kCellSharpS */
#endif

	'\0' /* just so last above line can end in ',' */
};
#endif

LOCALVAR char *CntrlDisplayBuff = nullpr;

LOCALPROC DrawCell(unsigned int h, unsigned int v, int x)
{
#if 1
	/* safety check */
	if ((h < ((long)vMacScreenWidth / 8 - 2)) && (v < (vMacScreenHeight / 16 - 1)))
#endif
	{
		int i;
		ui3p p0 = ((ui3p)CellData) + 16 * x;

#if 0 != vMacScreenDepth
		if (UseColorMode) {
			ui3p p = ((ui3p)CntrlDisplayBuff) + ((h + 1) << vMacScreenDepth) + (v * 16 + 11) * vMacScreenByteWidth;

			for (i = 16; --i >= 0; ) {
#if 1 == vMacScreenDepth
				int k;
				ui3b t0 = *p0;
				ui3p p2 = p;
				for (k = 2; --k >= 0; ) {
					*p2++ = (((t0) & 0x80) ? 0xC0 : 0x00)
						| (((t0) & 0x40) ? 0x30 : 0x00)
						| (((t0) & 0x20) ? 0x0C : 0x00)
						| (((t0) & 0x10) ? 0x03 : 0x00);
						/* black RRGGBBAA, white RRGGBBAA */
					t0 <<= 4;
				}
#elif 2 == vMacScreenDepth
				int k;
				ui3b t0 = *p0;
				ui3p p2 = p;
				for (k = 4; --k >= 0; ) {
					*p2++ = (((t0) & 0x40) ? 0x0F : 0x00)
						| (((t0) & 0x80) ? 0xF0 : 0x00);
						/* black RRGGBBAA, white RRGGBBAA */
					t0 <<= 2;
				}
#elif 3 == vMacScreenDepth
				int k;
				ui3b t0 = *p0;
				ui3p p2 = p;
				for (k = 8; --k >= 0; ) {
					*p2++ = ((t0 >> k) & 0x01) ? 0xFF : 0x00;
						/* black RRGGBBAA, white RRGGBBAA */
				}
#elif 4 == vMacScreenDepth
				int k;
				ui4r v;
				ui3b t0 = *p0;
				ui3p p2 = p;
				for (k = 8; --k >= 0; ) {
					v = ((t0 >> k) & 0x01) ? 0x0000 : 0x7FFF;
						/* black RRGGBBAA, white RRGGBBAA */
					/* *((ui4b *)p2)++ = v; need big endian, so : */
					*p2++ = v >> 8;
					*p2++ = v;
				}
#elif 5 == vMacScreenDepth
				int k;
				ui5r v;
				ui3b t0 = *p0;
				ui3p p2 = p;
				for (k = 8; --k >= 0; ) {
					v = ((t0 >> k) & 0x01) ? 0x00000000 : 0x00FFFFFF;
						/* black RRGGBBAA, white RRGGBBAA */
					/* *((ui5b *)p2)++ = v; need big endian, so : */
					*p2++ = v >> 24;
					*p2++ = v >> 16;
					*p2++ = v >> 8;
					*p2++ = v;
				}
#endif
				p += vMacScreenByteWidth;
				p0 ++;
			}
		} else
#endif
		{
			ui3p p = ((ui3p)CntrlDisplayBuff) + (h + 1) + (v * 16 + 11) * vMacScreenMonoByteWidth;

			for (i = 16; --i >= 0; ) {
				*p = *p0;
				p += vMacScreenMonoByteWidth;
				p0 ++;
			}
		}
	}
}

#define ControlBoxh0 0
#define ControlBoxw 62
#define ControlBoxv0 0

#define hLimit (ControlBoxh0 + ControlBoxw - 1)
#define hStart (ControlBoxh0 + 1)


LOCALVAR int CurCellh0;
LOCALVAR int CurCellv0;

LOCALPROC DrawCellsBeginLine(void)
{
	DrawCell(ControlBoxh0, CurCellv0, kCellMiddleLeft);
	CurCellh0 = hStart;
}

LOCALPROC DrawCellsEndLine(void)
{
	int i;

	for (i = CurCellh0; i < hLimit; ++i) {
		DrawCell(i, CurCellv0, kCellSpace);
	}
	DrawCell(hLimit, CurCellv0, kCellMiddleRight);
	CurCellv0++;
}

LOCALPROC DrawCellsBottomLine(void)
{
	int i;

	DrawCell(ControlBoxh0 + 0, CurCellv0, kCellLowerLeft);
	for (i = hStart; i < hLimit; ++i) {
		DrawCell(i, CurCellv0, kCellLowerMiddle);
	}
	DrawCell(hLimit, CurCellv0, kCellLowerRight);
}

LOCALPROC DrawCellAdvance(int x)
{
	DrawCell(CurCellh0, CurCellv0, x);
	CurCellh0++;
}

LOCALPROC DrawCellsBlankLine(void)
{
	DrawCellsBeginLine();
	DrawCellsEndLine();
}

LOCALVAR blnr SpeedStopped = falseblnr;

LOCALVAR blnr RunInBackground = (WantInitRunInBackground != 0);

#if EnableFullScreen
LOCALVAR blnr WantFullScreen = (WantInitFullScreen != 0);
#endif

#if EnableMagnify
LOCALVAR blnr WantMagnify = (WantInitMagnify != 0);
#endif

#if NeedRequestInsertDisk
LOCALVAR blnr RequestInsertDisk = falseblnr;
#endif

#if UseControlKeys
LOCALVAR blnr ControlKeyPressed = falseblnr;
#endif

LOCALFUNC char * GetSubstitutionStr(char x)
{
	char *s;

	switch (x) {
		case 'w':
			s = kStrHomePage;
			break;
		case 'y':
			s = kStrCopyrightYear;
			break;
		case 'p':
			s = kStrAppName;
			break;
		case 'v':
			s = kAppVariationStr;
			break;
		case 'e':
			s = kEmuName;
			break;
		case 'm':
			s = kMaintainerName;
			break;
		case 'r':
			s = RomFileName;
			break;
#if UseControlKeys
		case 'k':
			if (ControlKeyPressed) {
				s = kStrPressed;
			} else {
				s = kStrReleased;
			}
			break;
#endif
#if EnableMagnify
		case 'g':
			if (WantMagnify) {
				s = kStrOn;
			} else {
				s = kStrOff;
			}
			break;
#endif
#if EnableFullScreen
		case 'f':
			if (WantFullScreen) {
				s = kStrOn;
			} else {
				s = kStrOff;
			}
			break;
#endif
		case 'b':
			if (RunInBackground) {
				s = kStrOn;
			} else {
				s = kStrOff;
			}
			break;
		case 'h':
			if (SpeedStopped) {
				s = kStrStoppedOn;
			} else {
				s = kStrStoppedOff;
			}
			break;
		case 's':
			if (SpeedLimit) {
				switch (SpeedValue) {
					case 1:
						s = "2x";
						break;
					case 2:
						s = "4x";
						break;
					case 3:
						s = "8x";
						break;
					case 4:
						s = "16x";
						break;
					case 5:
						s = "32x";
						break;
					case 0:
					default:
						s = "1x";
						break;
				}
			} else {
				s = kStrSpeedValueAllOut;
			}
			break;
		default:
			s = "???";
			break;
	}
	return s;
}

#define ClStrMaxLength 512

LOCALPROC ClStrAppendChar(int *L0, ui3b *r, ui3b c)
{
	unsigned short L = *L0;

	if (ClStrMaxLength != L) {
		r[L] = c;
		L++;
		*L0 = L;
	}
}

LOCALPROC ClStrAppendSubstCStr(int *L, ui3b *r, char *s)
{
	char *p = s;
	char c;
	ui3b x;

	while ((c = *p++) != 0) {
		if (c == '^') {
			if ((c = *p++) == 0) {
				return; /* oops, unexpected end of string, abort */
			} else if (c == '^') {
				ClStrAppendChar(L, r, c);
			} else {
				ClStrAppendSubstCStr(L, r, GetSubstitutionStr(c));
			}
		} else if (c == ';') {
			switch (*p++) {
				case 'g': x = kCellCopyright; break;
				case 'l':
					switch (*p++) {
						case 'a': x = kCellApostrophe; break;
						case 'l': x = kCellEllipsis; break;
						case 's': x = kCellSemicolon; break;
#if NeedIntlChars
						case 'E': x = kCellUpAE; break;
						case 'e': x = kCellLoAE; break;
#endif
						default: return; break;
					}
					break;
				case '[': x = kCellLeftDQuote; break;
				case '{': x = kCellRightDQuote; break;
				case ']': x = kCellLeftSQuote; break;
				case '}': x = kCellRightSQuote; break;
#if NeedIntlChars
				case '?': x = kCellInvQuestion; break;
				case 'A': x = kCellUpARing; break;
				case 'C': x = kCellUpCCedilla; break;
				case 'O': x = kCellUpOStroke; break;
				case 'Q': x = kCellUpLigatureOE; break;
				case '`':
					switch (*p++) {
						case 'A': x = kCellUpAGrave; break;
						case 'E': x = kCellUpEGrave; break;
						case 'I': x = kCellUpIGrave; break;
						case 'O': x = kCellUpOGrave; break;
						case 'U': x = kCellUpUGrave; break;
						case 'a': x = kCellLoAGrave; break;
						case 'e': x = kCellLoEGrave; break;
						case 'i': x = kCellLoIGrave; break;
						case 'o': x = kCellLoOGrave; break;
						case 'u': x = kCellLoUGrave; break;
						default: return; break;
					}
					break;
				case 'a': x = kCellLoARing; break;
				case 'c': x = kCellLoCCedilla; break;
				case 'e':
					switch (*p++) {
						case 'A': x = kCellUpAAcute; break;
						case 'E': x = kCellUpEAcute; break;
						case 'I': x = kCellUpIAcute; break;
						case 'O': x = kCellUpOAcute; break;
						case 'U': x = kCellUpUAcute; break;
						case 'a': x = kCellLoAAcute; break;
						case 'e': x = kCellLoEAcute; break;
						case 'i': x = kCellLoIAcute; break;
						case 'o': x = kCellLoOAcute; break;
						case 'u': x = kCellLoUAcute; break;
						default: return; break;
					}
					break;
				case 'i':
					switch (*p++) {
						case 'A': x = kCellUpACircumflex; break;
						case 'E': x = kCellUpECircumflex; break;
						case 'I': x = kCellUpICircumflex; break;
						case 'O': x = kCellUpOCircumflex; break;
						case 'U': x = kCellUpUCircumflex; break;
						case 'a': x = kCellLoACircumflex; break;
						case 'e': x = kCellLoECircumflex; break;
						case 'i': x = kCellLoICircumflex; break;
						case 'o': x = kCellLoOCircumflex; break;
						case 'u': x = kCellLoUCircumflex; break;
						default: return; break;
					}
					break;
				case 'n':
					switch (*p++) {
						case 'A': x = kCellUpATilde; break;
						case 'N': x = kCellUpNTilde; break;
						case 'O': x = kCellUpOTilde; break;
						case 'a': x = kCellLoATilde; break;
						case 'n': x = kCellLoNTilde; break;
						case 'o': x = kCellLoOTilde; break;
						default: return; break;
					}
					break;
				case 'o': x = kCellLoOStroke; break;
				case 'q': x = kCellLoLigatureOE; break;
				case 's': x = kCellSharpS; break;
				case 'u':
					switch (*p++) {
						case 'A': x = kCellUpADiaeresis; break;
						case 'E': x = kCellUpEDiaeresis; break;
						case 'I': x = kCellUpIDiaeresis; break;
						case 'O': x = kCellUpODiaeresis; break;
						case 'U': x = kCellUpUDiaeresis; break;
						case 'Y': x = kCellUpYDiaeresis; break;
						case 'a': x = kCellLoADiaeresis; break;
						case 'e': x = kCellLoEDiaeresis; break;
						case 'i': x = kCellLoIDiaeresis; break;
						case 'o': x = kCellLoODiaeresis; break;
						case 'u': x = kCellLoUDiaeresis; break;
						case 'y': x = kCellLoYDiaeresis; break;
						default: return; break;
					}
					break;
#endif
				default:
					return; /* oops, unexpected char, maybe end of string, abort */
					break;
			}
			ClStrAppendChar(L, r, x);
		} else {
			switch (c) {
				case 'A': x = kCellUpA; break;
				case 'B': x = kCellUpB; break;
				case 'C': x = kCellUpC; break;
				case 'D': x = kCellUpD; break;
				case 'E': x = kCellUpE; break;
				case 'F': x = kCellUpF; break;
				case 'G': x = kCellUpG; break;
				case 'H': x = kCellUpH; break;
				case 'I': x = kCellUpI; break;
				case 'J': x = kCellUpJ; break;
				case 'K': x = kCellUpK; break;
				case 'L': x = kCellUpL; break;
				case 'M': x = kCellUpM; break;
				case 'N': x = kCellUpN; break;
				case 'O': x = kCellUpO; break;
				case 'P': x = kCellUpP; break;
				case 'Q': x = kCellUpQ; break;
				case 'R': x = kCellUpR; break;
				case 'S': x = kCellUpS; break;
				case 'T': x = kCellUpT; break;
				case 'U': x = kCellUpU; break;
				case 'V': x = kCellUpV; break;
				case 'W': x = kCellUpW; break;
				case 'X': x = kCellUpX; break;
				case 'Y': x = kCellUpY; break;
				case 'Z': x = kCellUpZ; break;
				case 'a': x = kCellLoA; break;
				case 'b': x = kCellLoB; break;
				case 'c': x = kCellLoC; break;
				case 'd': x = kCellLoD; break;
				case 'e': x = kCellLoE; break;
				case 'f': x = kCellLoF; break;
				case 'g': x = kCellLoG; break;
				case 'h': x = kCellLoH; break;
				case 'i': x = kCellLoI; break;
				case 'j': x = kCellLoJ; break;
				case 'k': x = kCellLoK; break;
				case 'l': x = kCellLoL; break;
				case 'm': x = kCellLoM; break;
				case 'n': x = kCellLoN; break;
				case 'o': x = kCellLoO; break;
				case 'p': x = kCellLoP; break;
				case 'q': x = kCellLoQ; break;
				case 'r': x = kCellLoR; break;
				case 's': x = kCellLoS; break;
				case 't': x = kCellLoT; break;
				case 'u': x = kCellLoU; break;
				case 'v': x = kCellLoV; break;
				case 'w': x = kCellLoW; break;
				case 'x': x = kCellLoX; break;
				case 'y': x = kCellLoY; break;
				case 'z': x = kCellLoZ; break;
				case '0': x = kCellDigit0; break;
				case '1': x = kCellDigit1; break;
				case '2': x = kCellDigit2; break;
				case '3': x = kCellDigit3; break;
				case '4': x = kCellDigit4; break;
				case '5': x = kCellDigit5; break;
				case '6': x = kCellDigit6; break;
				case '7': x = kCellDigit7; break;
				case '8': x = kCellDigit8; break;
				case '9': x = kCellDigit9; break;
				case '!': x = kCellExclamation; break;
				case '&': x = kCellAmpersand; break;
				case '(': x = kCellLeftParen; break;
				case ')': x = kCellRightParen; break;
				case ',': x = kCellComma; break;
				case '-': x = kCellHyphen; break;
				case '.': x = kCellPeriod; break;
				case '/': x = kCellSlash; break;
				case ':': x = kCellColon; break;
				case ';': x = kCellSemicolon; break;
				case '?': x = kCellQuestion; break;
				case '_': x = kCellUnderscore; break;
				default: x = kCellSpace; break;
			}
			ClStrAppendChar(L, r, x);
		}
	}
}

LOCALPROC ClStrFromSubstCStr(int *L, ui3b *r, char *s)
{
	*L = 0;
	ClStrAppendSubstCStr(L, r, s);
}

LOCALPROC DrawCellsFromStr(char *s)
{
	ui3b ps[ClStrMaxLength];
	ui3b cs;
	int L;
	int i;
	int j;
	int w;

	ClStrFromSubstCStr(&L, ps, s);

	i = 0;

	while (i < L) {
		cs = ps[i];
		i++;
		if (CurCellh0 < hLimit) {
			DrawCellAdvance(cs);
		} else {
			/* line is too wide, wrap */
			if (kCellSpace != cs) {
				--i; /* back up one char, at least */

				/* now try backing up to beginning of word */
				j = i;
				w = CurCellh0 - hStart;

				while ((w > 0) && (j > 0)
					&& (ps[j - 1] != kCellSpace))
				{
					--j;
					--w;
				}
				if (w != 0) {
					i = j;
					CurCellh0 = hStart + w;
				}
				/*
					else if w == 0, then have backed up to
					beginning of line, so just let the word
					be split.
				*/
			}
			/*
				else if cs == kCellSpace, just lose the space.
			*/
			DrawCellsEndLine();
				/*
					draw white space over the part of
					the word that have already drawn
				*/
			DrawCellsBeginLine();
		}
	}
}

LOCALPROC DrawCellsOneLineStr(char *s)
{
	DrawCellsBeginLine();
	DrawCellsFromStr(s);
	DrawCellsEndLine();
}

LOCALPROC DrawCellsKeyCommand(char *k, char *s)
{
	DrawCellsBeginLine();
	DrawCellsFromStr("'");
	DrawCellsFromStr(k);
	DrawCellsFromStr("' - ");
	DrawCellsFromStr(s);
	DrawCellsEndLine();
}

typedef void (*SpclModeBody) (void);

LOCALPROC DrawSpclMode0(char *Title, SpclModeBody Body)
{
	int i;
	int k;

	CurCellv0 = ControlBoxv0 + 0;
	DrawCell(ControlBoxh0 + 0, CurCellv0, kCellUpperLeft);
	k = kCellIcon00;
	for (i = hStart; i < hStart + 4; ++i) {
		DrawCell(i, CurCellv0, k);
		k++;
	}
	for (i = hStart + 4; i < hLimit; ++i) {
		DrawCell(i, CurCellv0, kCellUpperMiddle);
	}
	DrawCell(hLimit, CurCellv0, kCellUpperRight);
	++CurCellv0;

	DrawCellsBeginLine();
	for (i = hStart; i < hStart + 4; ++i) {
		DrawCellAdvance(k);
		k++;
	}
	DrawCellAdvance(kCellSpace);
	DrawCellsFromStr(Title);
	DrawCellsEndLine();

	DrawCellsBeginLine();
	for (i = hStart; i < hStart + 4; ++i) {
		DrawCellAdvance(k);
		k++;
	}
	for (i = hStart + 4; i < hLimit; ++i) {
		DrawCellAdvance(kCellGraySep);
	}
	DrawCellsEndLine();

	if (nullpr != Body) {
		Body();
	}

	DrawCellsBottomLine();
}

#if EnableAltKeysMode
#include "ALTKEYSM.h"
#else
#define Keyboard_UpdateKeyMap1 Keyboard_UpdateKeyMap
#define DisconnectKeyCodes1 DisconnectKeyCodes
#endif

LOCALVAR char *SavedBriefMsg = nullpr;
LOCALVAR char *SavedLongMsg;
LOCALVAR blnr SavedFatalMsg;

LOCALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	if (nullpr != SavedBriefMsg) {
		/*
			ignore the new message, only display the
			first error.
		*/
	} else {
		SavedBriefMsg = briefMsg;
		SavedLongMsg = longMsg;
		SavedFatalMsg = fatal;
	}
}

LOCALPROC DrawCellsMessageModeBody(void)
{
	DrawCellsOneLineStr(SavedBriefMsg);
	DrawCellsBlankLine();
	DrawCellsOneLineStr(SavedLongMsg);
}

LOCALPROC DrawMessageMode(void)
{
	DrawSpclMode0(kStrModeMessage, DrawCellsMessageModeBody);
}

LOCALPROC MacMsgDisplayOff(void)
{
	SpecialModeClr(SpclModeMessage);
	SavedBriefMsg = nullpr;
	NeedWholeScreenDraw = trueblnr;
}

LOCALPROC MacMsgDisplayOn(void)
{
	NeedWholeScreenDraw = trueblnr;
	DisconnectKeyCodes1(kKeepMaskControl | kKeepMaskCapsLock); /* command */
	SpecialModeSet(SpclModeMessage);
}

LOCALPROC DoMessageModeKey(int key)
{
	if (MKC_C == key) {
		MacMsgDisplayOff();
	}
}

LOCALPROC MacMsgOverride(char *briefMsg, char *longMsg)
{
	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}
	MacMsg(briefMsg, longMsg, falseblnr);
}

#if NeedDoMoreCommandsMsg
LOCALPROC DoMoreCommandsMsg(void)
{
	MacMsgOverride(kStrMoreCommandsTitle,
		kStrMoreCommandsMessage);
}
#endif

#if NeedDoAboutMsg
LOCALPROC DoAboutMsg(void)
{
	MacMsgOverride(kStrAboutTitle,
		kStrAboutMessage);
}
#endif

GLOBALPROC WarnMsgCorruptedROM(void)
{
	MacMsg(kStrCorruptedROMTitle, kStrCorruptedROMMessage, falseblnr);
}

GLOBALPROC WarnMsgUnsupportedROM(void)
{
	MacMsg(kStrUnsupportedROMTitle, kStrUnsupportedROMMessage, falseblnr);
}

#if DetailedAbnormalReport
GLOBALPROC WarnMsgAbnormal(char *s)
#else
GLOBALPROC WarnMsgAbnormal(void)
#endif
{
	MacMsg(kStrReportAbnormalTitle,
#if DetailedAbnormalReport
		s,
#else
		kStrReportAbnormalMessage,
#endif
		falseblnr);
}

#if UseControlKeys

LOCALVAR blnr LastControlKey = falseblnr;
LOCALVAR int CurControlMode = 0;
LOCALVAR int ControlMessage = 0;

enum {
	kCntrlModeOff,
	kCntrlModeBase,
	kCntrlModeConfirmReset,
	kCntrlModeConfirmInterrupt,
	kCntrlModeConfirmQuit,
	kCntrlModeSpeedControl,

	kNumCntrlModes
};

enum {
	kCntrlMsgBaseStart,
#if EnableMagnify
	kCntrlMsgMagnify,
#endif
#if EnableFullScreen
	kCntrlMsgFullScreen,
#endif
	kCntrlMsgConfirmResetStart,
	kCntrlMsgHaveReset,
	kCntrlMsgResetCancelled,
	kCntrlMsgConfirmInterruptStart,
	kCntrlMsgHaveInterrupted,
	kCntrlMsgInterruptCancelled,
	kCntrlMsgConfirmQuitStart,
	kCntrlMsgQuitCancelled,
	kCntrlMsgEmCntrl,
	kCntrlMsgSpeedControlStart,
	kCntrlMsgNewSpeed,
	kCntrlMsgNewStopped,
	kCntrlMsgNewRunInBack,
	kCntrlMsgAbout,
	kCntrlMsgHelp,
#if UseActvCode
	kCntrlMsgRegStrCopied,
#endif

	kNumCntrlMsgs
};

LOCALPROC DoEnterControlMode(void)
{
	CurControlMode = kCntrlModeBase;
	ControlMessage = kCntrlMsgBaseStart;
	NeedWholeScreenDraw = trueblnr;
	DisconnectKeyCodes1(kKeepMaskControl | kKeepMaskCapsLock);
	SpecialModeSet(SpclModeControl);
}

LOCALPROC DoLeaveControlMode(void)
{
	SpecialModeClr(SpclModeControl);
	CurControlMode = kCntrlModeOff;
	NeedWholeScreenDraw = trueblnr;
}

LOCALPROC Keyboard_UpdateControlKey(blnr down)
{
	if (down != LastControlKey) {
		LastControlKey = down;
		if (down) {
			DoEnterControlMode();
		} else {
			DoLeaveControlMode();
		}
	}
}

LOCALPROC SetSpeedValue(ui3b i)
{
	SpeedLimit = trueblnr;
	SpeedValue = i;
	CurControlMode = kCntrlModeBase;
	ControlMessage = kCntrlMsgNewSpeed;
}

#if EnableFullScreen
FORWARDPROC ToggleWantFullScreen(void);
#endif
#if UseActvCode
FORWARDPROC CopyRegistrationStr(void);
#endif

LOCALPROC DoControlModeKey(int key)
{
	switch (CurControlMode) {
		case kCntrlModeBase:
			switch (key) {
				case MKC_K:
					ControlKeyPressed = ! ControlKeyPressed;
					ControlMessage = kCntrlMsgEmCntrl;
					Keyboard_UpdateKeyMap1(MKC_Control, ControlKeyPressed);
					break;
				case MKC_S:
					CurControlMode = kCntrlModeSpeedControl;
					ControlMessage = kCntrlMsgSpeedControlStart;
					break;
				case MKC_I:
					CurControlMode = kCntrlModeConfirmInterrupt;
					ControlMessage = kCntrlMsgConfirmInterruptStart;
					break;
				case MKC_R:
					if (! AnyDiskInserted()) {
						WantMacReset = trueblnr;
						ControlMessage = kCntrlMsgHaveReset;
					} else {
						CurControlMode = kCntrlModeConfirmReset;
						ControlMessage = kCntrlMsgConfirmResetStart;
					}
					break;
				case MKC_Q:
					if (! AnyDiskInserted()) {
						ForceMacOff = trueblnr;
					} else {
						CurControlMode = kCntrlModeConfirmQuit;
						ControlMessage = kCntrlMsgConfirmQuitStart;
					}
					break;
				case MKC_A:
					ControlMessage = kCntrlMsgAbout;
					break;
				case MKC_H:
					ControlMessage = kCntrlMsgHelp;
					break;
#if NeedRequestInsertDisk
				case MKC_O:
					RequestInsertDisk = trueblnr;
					break;
#endif
#if EnableMagnify
				case MKC_M:
					WantMagnify = ! WantMagnify;
					ControlMessage = kCntrlMsgMagnify;
					break;
#endif
#if EnableFullScreen
				case MKC_F:
					ToggleWantFullScreen();
					ControlMessage = kCntrlMsgFullScreen;
					break;
#endif
#if UseActvCode
				case MKC_P:
					CopyRegistrationStr();
					ControlMessage = kCntrlMsgRegStrCopied;
					break;
#endif
			}
			break;
		case kCntrlModeConfirmReset:
			switch (key) {
				case MKC_Y:
					WantMacReset = trueblnr;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgHaveReset;
					break;
				case MKC_N:
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgResetCancelled;
					break;
			}
			break;
		case kCntrlModeConfirmInterrupt:
			switch (key) {
				case MKC_Y:
					WantMacInterrupt = trueblnr;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgHaveInterrupted;
					break;
				case MKC_N:
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgInterruptCancelled;
					break;
			}
			break;
		case kCntrlModeConfirmQuit:
			switch (key) {
				case MKC_Y:
					ForceMacOff = trueblnr;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgBaseStart;
						/* shouldn't see this message since quitting */
					break;
				case MKC_N:
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgQuitCancelled;
					break;
			}
			break;
		case kCntrlModeSpeedControl:
			switch (key) {
				case MKC_E:
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgBaseStart;
					break;
				case MKC_B:
					RunInBackground = ! RunInBackground;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgNewRunInBack;
					break;
				case MKC_D:
					SpeedStopped = ! SpeedStopped;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgNewStopped;
					break;
				case MKC_Z:
					SetSpeedValue(0);
					break;
				case MKC_1:
					SetSpeedValue(1);
					break;
				case MKC_2:
					SetSpeedValue(2);
					break;
				case MKC_3:
					SetSpeedValue(3);
					break;
				case MKC_4:
					SetSpeedValue(4);
					break;
				case MKC_5:
					SetSpeedValue(5);
					break;
				case MKC_A:
					SpeedLimit = falseblnr;
					CurControlMode = kCntrlModeBase;
					ControlMessage = kCntrlMsgNewSpeed;
					break;
			}
			break;
	}
	NeedWholeScreenDraw = trueblnr;
}

LOCALFUNC char * ControlMode2TitleStr(void)
{
	char *s;

	switch (CurControlMode) {
		case kCntrlModeConfirmReset:
			s = kStrModeConfirmReset;
			break;
		case kCntrlModeConfirmInterrupt:
			s = kStrModeConfirmInterrupt;
			break;
		case kCntrlModeConfirmQuit:
			s = kStrModeConfirmQuit;
			break;
		case kCntrlModeSpeedControl:
			s = kStrModeSpeedControl;
			break;
		case kCntrlModeBase:
		default:
			if (kCntrlMsgHelp == ControlMessage) {
				s = kStrModeControlHelp;
			} else {
				s = kStrModeControlBase;
			}
			break;
	}

	return s;
}

LOCALPROC DrawCellsControlModeBody(void)
{
	switch (ControlMessage) {
		case kCntrlMsgAbout:
			DrawCellsOneLineStr(kStrProgramInfo);
			DrawCellsBlankLine();
			DrawCellsOneLineStr(kStrWorkOfMany);
			DrawCellsOneLineStr(kStrMaintainerIs);
			DrawCellsOneLineStr("^m");
			DrawCellsOneLineStr(kStrForMoreInfo);
			DrawCellsOneLineStr("^w");
			DrawCellsBlankLine();
			DrawCellsOneLineStr(kStrLicense);
			DrawCellsBlankLine();
			DrawCellsOneLineStr(kStrDisclaimer);
			break;
		case kCntrlMsgHelp:
			DrawCellsOneLineStr(kStrHowToLeaveControl);
			DrawCellsOneLineStr(kStrHowToPickACommand);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("A", kStrCmdAbout);
#if NeedRequestInsertDisk
			DrawCellsKeyCommand("O", kStrCmdOpenDiskImage);
#endif
			DrawCellsKeyCommand("Q", kStrCmdQuit);
			DrawCellsKeyCommand("S", kStrCmdSpeedControl);
#if EnableMagnify
			DrawCellsKeyCommand("M", kStrCmdMagnifyToggle);
#endif
#if EnableFullScreen
			DrawCellsKeyCommand("F", kStrCmdFullScrnToggle);
#endif
			DrawCellsKeyCommand("K", kStrCmdCtrlKeyToggle);
			DrawCellsKeyCommand("R", kStrCmdReset);
			DrawCellsKeyCommand("I", kStrCmdInterrupt);
			DrawCellsKeyCommand("H", kStrCmdHelp);
			break;
		case kCntrlMsgSpeedControlStart:
			DrawCellsOneLineStr(kStrCurrentSpeed);
			DrawCellsKeyCommand("Z", "1x");
			DrawCellsKeyCommand("1", "2x");
			DrawCellsKeyCommand("2", "4x");
			DrawCellsKeyCommand("3", "8x");
			DrawCellsKeyCommand("4", "16x");
			DrawCellsKeyCommand("5", "32x");
			DrawCellsKeyCommand("A", kStrSpeedAllOut);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("D", kStrSpeedStopped);
			DrawCellsKeyCommand("B", kStrSpeedBackToggle);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("E", kStrSpeedExit);
			break;
		case kCntrlMsgNewSpeed:
			DrawCellsOneLineStr(kStrNewSpeed);
			break;
		case kCntrlMsgNewRunInBack:
			DrawCellsOneLineStr(kStrNewRunInBack);
			break;
		case kCntrlMsgNewStopped:
			DrawCellsOneLineStr(kStrNewStopped);
			break;
#if EnableMagnify
		case kCntrlMsgMagnify:
			DrawCellsOneLineStr(kStrNewMagnify);
			break;
#endif
#if EnableFullScreen
		case kCntrlMsgFullScreen:
			DrawCellsOneLineStr(kStrNewFullScreen);
			break;
#endif
#if UseActvCode
		case kCntrlMsgRegStrCopied:
			DrawCellsOneLineStr("Registration String copied.");
			break;
#endif
		case kCntrlMsgConfirmResetStart:
			DrawCellsOneLineStr(kStrConfirmReset);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("Y", kStrResetDo);
			DrawCellsKeyCommand("N", kStrResetNo);
			break;
		case kCntrlMsgHaveReset:
			DrawCellsOneLineStr(kStrHaveReset);
			break;
		case kCntrlMsgResetCancelled:
			DrawCellsOneLineStr(kStrCancelledReset);
			break;
		case kCntrlMsgConfirmInterruptStart:
			DrawCellsOneLineStr(kStrConfirmInterrupt);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("Y", kStrInterruptDo);
			DrawCellsKeyCommand("N", kStrInterruptNo);
			break;
		case kCntrlMsgHaveInterrupted:
			DrawCellsOneLineStr(kStrHaveInterrupted);
			break;
		case kCntrlMsgInterruptCancelled:
			DrawCellsOneLineStr(kStrCancelledInterrupt);
			break;
		case kCntrlMsgConfirmQuitStart:
			DrawCellsOneLineStr(kStrConfirmQuit);
			DrawCellsBlankLine();
			DrawCellsKeyCommand("Y", kStrQuitDo);
			DrawCellsKeyCommand("N", kStrQuitNo);
			break;
		case kCntrlMsgQuitCancelled:
			DrawCellsOneLineStr(kStrCancelledQuit);
			break;
		case kCntrlMsgEmCntrl:
			DrawCellsOneLineStr(kStrNewCntrlKey);
			break;
		case kCntrlMsgBaseStart:
		default:
			DrawCellsOneLineStr(kStrHowToLeaveControl);
			break;
	}
}

LOCALPROC DrawControlMode(void)
{
	DrawSpclMode0(ControlMode2TitleStr(), DrawCellsControlModeBody);
}

#endif /* UseControlKeys */

#if UseActvCode
#include "ACTVCODE.h"
#endif

LOCALPROC DrawSpclMode(void)
{
#if UseControlKeys
	if (SpecialModeTst(SpclModeControl)) {
		DrawControlMode();
	} else
#endif
	if (SpecialModeTst(SpclModeMessage)) {
		DrawMessageMode();
	} else
#if UseActvCode
	if (SpecialModeTst(SpclModeActvCode)) {
		DrawActvCodeMode();
	} else
#endif
#if EnableAltKeysMode
	if (SpecialModeTst(SpclModeAltKeyText)) {
		DrawAltKeyMode();
	} else
#endif
	{
		/* should not get here */
	}
}

LOCALFUNC char * GetCurDrawBuff(void)
{
	char *p = screencomparebuff;

	if (0 != SpecialModes) {
		MyMoveBytes((anyp)p, (anyp)CntrlDisplayBuff,
#if 0 != vMacScreenDepth
			UseColorMode ? vMacScreenNumBytes :
#endif
				vMacScreenMonoNumBytes
			);
		p = CntrlDisplayBuff;

		DrawSpclMode();
	}

	return p;
}

LOCALPROC Keyboard_UpdateKeyMap2(int key, blnr down)
{
#ifndef MKC_formac_Control
#if SwapCommandControl
#define MKC_formac_Control MKC_Command
#else
#define MKC_formac_Control MKC_Control
#endif
#endif
#if MKC_formac_Control != MKC_Control
	if (MKC_Control == key) {
		key = MKC_formac_Control;
	} else
#endif

#ifndef MKC_formac_Command
#if SwapCommandControl
#define MKC_formac_Command MKC_Control
#else
#define MKC_formac_Command MKC_Command
#endif
#endif
#if MKC_formac_Command != MKC_Command
	if (MKC_Command == key) {
		key = MKC_formac_Command;
	} else
#endif

#ifndef MKC_formac_Option
#define MKC_formac_Option MKC_Option
#endif
#if MKC_formac_Option != MKC_Option
	if (MKC_Option == key) {
		key = MKC_formac_Option;
	} else
#endif

#ifndef MKC_formac_F1
#define MKC_formac_F1 MKC_Option
#endif
#if MKC_formac_F1 != MKC_F1
	if (MKC_F1 == key) {
		key = MKC_formac_F1;
	} else
#endif

#ifndef MKC_formac_F2
#define MKC_formac_F2 MKC_Command
#endif
#if MKC_formac_F2 != MKC_F2
	if (MKC_F2 == key) {
		key = MKC_formac_F2;
	} else
#endif

	{
	}


#if UseControlKeys
	if (MKC_Control == key) {
		Keyboard_UpdateControlKey(down);
	} else
#endif
	if ((0 == SpecialModes)
#if EnableAltKeysMode
			|| (0 == (SpecialModes & ~ (1 << SpclModeAltKeyText)))
#endif
			|| (MKC_CapsLock == key)
		)
	{
		/* pass through */
		Keyboard_UpdateKeyMap1(key, down);
	} else {
		if (down) {
#if UseControlKeys
			if (SpecialModeTst(SpclModeControl)) {
				DoControlModeKey(key);
			} else
#endif
			if (SpecialModeTst(SpclModeMessage)) {
				DoMessageModeKey(key);
			} else
#if UseActvCode
			if (SpecialModeTst(SpclModeActvCode)) {
				DoActvCodeModeKey(key);
			} else
#endif
			{
			}
		} /* else if not down ignore */
	}
}

LOCALPROC DisconnectKeyCodes2(void)
{
	DisconnectKeyCodes1(kKeepMaskControl | kKeepMaskCapsLock);
#if UseControlKeys
	Keyboard_UpdateControlKey(falseblnr);
#endif
}
