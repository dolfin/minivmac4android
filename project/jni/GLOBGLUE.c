/*
	GLOBGLUE.c

	Copyright (C) 2003 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	GLOBal GLUE (or GLOB of GLUE)

	Holds the program together.

	Some code here adapted from "custom.c" in vMac by Philip Cummins,
	in turn descended from code in the Un*x Amiga Emulator by
	Bernd Schmidt.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#include "EMCONFIG.h"
#endif

#include "GLOBGLUE.h"

#if EmRTC
IMPORTFUNC blnr RTC_Init(void);
#endif
IMPORTFUNC blnr ROM_Init(void);
#if EmVidCard
IMPORTFUNC blnr Vid_Init(void);
#endif
FORWARDFUNC blnr AddrSpac_Init(void);

IMPORTPROC VIA1_Zap(void);
#if EmVIA2
IMPORTPROC VIA2_Zap(void);
#endif

IMPORTPROC Sony_EjectAllDisks(void);

IMPORTPROC m68k_reset(void);
IMPORTPROC IWM_Reset(void);
IMPORTPROC SCC_Reset(void);
IMPORTPROC SCSI_Reset(void);
IMPORTPROC VIA1_Reset(void);
#if EmVIA2
IMPORTPROC VIA2_Reset(void);
#endif
IMPORTPROC Sony_Reset(void);
FORWARDPROC Memory_Reset(void);
FORWARDPROC ICT_Zap(void);
FORWARDPROC Extn_Reset(void);

IMPORTPROC Mouse_Update(void);
#if EmClassicKbrd
IMPORTPROC KeyBoard_Update(void);
#endif
#if EmADB
IMPORTPROC ADB_Update(void);
#endif
IMPORTPROC InterruptReset_Update(void);
IMPORTPROC VIA1_iCA1_PulseNtfy(void);
IMPORTPROC Sony_Update(void);
#if EmVidCard
IMPORTPROC Vid_Update(void);
#endif

#if EmRTC
IMPORTPROC RTC_Interrupt(void);
#endif

IMPORTPROC ExtnDisk_Access(CPTR p);
IMPORTPROC ExtnSony_Access(CPTR p);
#if EmVidCard
IMPORTPROC ExtnVideo_Access(CPTR p);
#endif

IMPORTPROC Sony_SetQuitOnEject(void);

IMPORTPROC MacSound_SubTick(int SubTick);

IMPORTPROC VIA1_ExtraTimeBegin(void);
IMPORTPROC VIA1_ExtraTimeEnd(void);

#if EmVIA2
IMPORTPROC VIA2_ExtraTimeBegin(void);
IMPORTPROC VIA2_ExtraTimeEnd(void);
#endif

#if SmallGlobals
IMPORTPROC MINEM68K_ReserveAlloc(void);
#endif

IMPORTPROC m68k_IPLchangeNtfy(void);
IMPORTPROC MINEM68K_Init(ui3b **BankReadAddr, ui3b **BankWritAddr,
	ui3b *fIPL);

IMPORTFUNC ui5b GetInstructionsRemaining(void);
IMPORTPROC SetInstructionsRemaining(ui5b n);

IMPORTPROC m68k_go_nInstructions(ui5b n);

IMPORTFUNC ui5b SCSI_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b SCC_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b IWM_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b VIA1_Access(ui5b Data, blnr WriteMem, CPTR addr);
#if EmVIA2
IMPORTFUNC ui5b VIA2_Access(ui5b Data, blnr WriteMem, CPTR addr);
#endif
#if EmASC
IMPORTFUNC ui5b ASC_Access(ui5b Data, blnr WriteMem, CPTR addr);
#endif
FORWARDPROC Extn_Access(ui5b Data, CPTR addr);

#if EmClassicKbrd
IMPORTPROC DoKybd_ReceiveEndCommand(void);
IMPORTPROC DoKybd_ReceiveCommand(void);
#endif
#if EmADB
IMPORTPROC ADB_DoNewState(void);
#endif
#if EmPMU
IMPORTPROC PMU_DoTask(void);
#endif
IMPORTPROC VIA1_DoTimer1Check(void);
IMPORTPROC VIA1_DoTimer2Check(void);
#if EmVIA2
IMPORTPROC VIA2_DoTimer1Check(void);
IMPORTPROC VIA2_DoTimer2Check(void);
#endif

GLOBALVAR ui5r my_disk_icon_addr;

GLOBALFUNC blnr InitEmulation(void)
{
#if EmRTC
	if (RTC_Init())
#endif
	if (ROM_Init())
#if EmVidCard
	if (Vid_Init())
#endif
	if (AddrSpac_Init())
	{
		return trueblnr;
	}
	return falseblnr;
}

GLOBALPROC EmulationReserveAlloc(void)
{
	ReserveAllocOneBlock((ui3p *)&RAM, kRAM_Size + RAMSafetyMarginFudge, 5, falseblnr);
#if EmVidCard
	ReserveAllocOneBlock((ui3p *)&VidROM, kVidROM_Size, 5, falseblnr);
#endif
#if IncludeVidMem
	ReserveAllocOneBlock((ui3p *)&VidMem, kVidMemRAM_Size + RAMSafetyMarginFudge, 5, trueblnr);
#endif
#if SmallGlobals
	MINEM68K_ReserveAlloc();
#endif
}

LOCALPROC EmulatedHardwareZap(void)
{
	Memory_Reset();
	ICT_Zap();
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA1_Zap();
#if EmVIA2
	VIA2_Zap();
#endif
	Sony_Reset();
	Extn_Reset();
	m68k_reset();
}

GLOBALPROC customreset(void)
{
	IWM_Reset();
	SCC_Reset();
	SCSI_Reset();
	VIA1_Reset();
#if EmVIA2
	VIA2_Reset();
#endif
	Sony_Reset();
	Extn_Reset();
#if CurEmMd <= kEmMd_128K
	WantMacReset = trueblnr;
	/*
		kludge, code in Finder appears
		to do RESET and not expect
		to come back. Maybe asserting
		the RESET somehow causes
		other hardware compenents to
		later reset the 68000.
	*/
#endif
}

LOCALPROC SixtiethSecondNotify(void)
{
#if 0
	DumpACStr("begin new Sixtieth");
	DumpANewLine();
#endif
	Mouse_Update();
	InterruptReset_Update();
#if EmClassicKbrd
	KeyBoard_Update();
#endif
#if EmADB
	ADB_Update();
#endif

	VIA1_iCA1_PulseNtfy(); /* Vertical Blanking Interrupt */
	Sony_Update();

#if EmRTC
	RTC_Interrupt();
#endif
#if EmVidCard
	Vid_Update();
#endif
}

LOCALPROC SubTickNotify(int SubTick)
{
#if 0
	DumpACStr("ending sub tick ");
	DumpANum(KiloInstructionsCounter);
	DumpANewLine();
#endif
#if MySoundEnabled && (CurEmMd != kEmMd_PB100)
	MacSound_SubTick(SubTick);
#else
	UnusedParam(SubTick);
#endif
}

LOCALPROC ExtraTimeBeginNotify(void)
{
#if 0
	DumpACStr("begin extra time");
	DumpANewLine();
#endif
	VIA1_ExtraTimeBegin();
#if EmVIA2
	VIA2_ExtraTimeBegin();
#endif
}

LOCALPROC ExtraTimeEndNotify(void)
{
	VIA1_ExtraTimeEnd();
#if EmVIA2
	VIA2_ExtraTimeEnd();
#endif
#if 0
	DumpACStr("end extra time");
	DumpANewLine();
#endif
}

EXPORTVAR(ui3p, RAM);

#if EmVidCard
GLOBALVAR ui3p VidROM = nullpr;
#endif

#if IncludeVidMem
GLOBALVAR ui3p VidMem = nullpr;
#endif

GLOBALVAR ui3b Wires[kNumWires];

LOCALVAR blnr GotOneAbnormal = falseblnr;

#ifndef ReportAbnormalInterrupt
#define ReportAbnormalInterrupt 0
#endif

#if DetailedAbnormalReport
GLOBALPROC DoReportAbnormal(char *s)
#else
GLOBALPROC DoReportAbnormal(void)
#endif
{
	if (! GotOneAbnormal) {
#if DetailedAbnormalReport
		WarnMsgAbnormal(s);
#else
		WarnMsgAbnormal();
#endif
#if ReportAbnormalInterrupt
		SetInterruptButton(trueblnr);
#endif
		GotOneAbnormal = trueblnr;
	}
}

/* top 8 bits out of 32 are ignored, so total size of address space is 2 ** 24 bytes */

#define TotAddrBytes (1UL << ln2TotAddrBytes)
#define kAddrMask (TotAddrBytes - 1)

/* map of address space */

#define kRAM_Base 0x00000000 /* when overlay off */
#if (CurEmMd == kEmMd_PB100) || (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
#define kRAM_ln2Spc 23
#else
#define kRAM_ln2Spc 22
#endif

#if IncludeVidMem
#if CurEmMd == kEmMd_PB100
#define kVidMem_Base 0x00FA0000
#define kVidMem_ln2Spc 16
#else
#define kVidMem_Base 0x00540000
#define kVidMem_ln2Spc 18
#endif
#endif

#if CurEmMd == kEmMd_PB100
#define kSCSI_Block_Base 0x00F90000
#define kSCSI_ln2Spc 16
#else
#define kSCSI_Block_Base 0x00580000
#define kSCSI_ln2Spc 19
#endif

#define kRAM_Overlay_Base 0x00600000 /* when overlay on */
#define kRAM_Overlay_Top  0x00800000

#if CurEmMd == kEmMd_PB100
#define kSCCRd_Block_Base 0x00FD0000
#define kSCC_ln2Spc 16
#else
#define kSCCRd_Block_Base 0x00800000
#define kSCC_ln2Spc 22
#endif

#if CurEmMd != kEmMd_PB100
#define kSCCWr_Block_Base 0x00A00000
#define kSCCWr_Block_Top  0x00C00000
#endif

#if CurEmMd == kEmMd_PB100
#define kIWM_Block_Base 0x00F60000
#define kIWM_ln2Spc 16
#else
#define kIWM_Block_Base 0x00C00000
#define kIWM_ln2Spc 21
#endif

#if CurEmMd == kEmMd_PB100
#define kVIA1_Block_Base 0x00F70000
#define kVIA1_ln2Spc 16
#else
#define kVIA1_Block_Base 0x00E80000
#define kVIA1_ln2Spc 19
#endif

#if CurEmMd == kEmMd_PB100
#define kASC_Block_Base 0x00FB0000
#define kASC_ln2Spc 16
#endif
#define kASC_Mask 0x00000FFF


/* implementation of read/write for everything but RAM and ROM */

#define kSCC_Mask 0x03

#define kVIA1_Mask 0x00000F
#if EmVIA2
#define kVIA2_Mask 0x00000F
#endif

#define kIWM_Mask 0x00000F /* Allocated Memory Bandwidth for IWM */

#if CurEmMd <= kEmMd_512Ke
#define ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_Plus
#define ROM_CmpZeroMask 0x00020000
#elif CurEmMd <= kEmMd_PB100
#define ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_IIx
#define ROM_CmpZeroMask 0
#else
#error "ROM_CmpZeroMask not defined"
#endif

#if CurEmMd <= kEmMd_512Ke
#define Overlay_ROM_CmpZeroMask 0x00100000
#elif CurEmMd <= kEmMd_Plus
#define Overlay_ROM_CmpZeroMask 0x00020000
#elif CurEmMd <= kEmMd_Classic
#define Overlay_ROM_CmpZeroMask 0x00300000
#elif CurEmMd <= kEmMd_PB100
#define Overlay_ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_IIx
#define Overlay_ROM_CmpZeroMask 0
#else
#error "Overlay_ROM_CmpZeroMask not defined"
#endif

/* devide address space into banks, some of which are mapped to real memory */

LOCALVAR ui3b *BankReadAddr[NumMemBanks];
LOCALVAR ui3b *BankWritAddr[NumMemBanks];
	/* if BankWritAddr[i] != NULL then BankWritAddr[i] == BankReadAddr[i] */

LOCALPROC SetPtrVecToNULL(ui3b **x, ui5b n)
{
	ui5b i;

	for (i = 0; i < n; i++) {
		*x++ = nullpr;
	}
}

LOCALPROC SetUpMemBanks(void)
{
	SetPtrVecToNULL(BankReadAddr, NumMemBanks);
	SetPtrVecToNULL(BankWritAddr, NumMemBanks);
}

LOCALPROC get_RAM_realblock(blnr WriteMem, CPTR addr,
	ui3p *RealStart, ui5b *RealSize)
{
	UnusedParam(WriteMem);

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
	{
		ui5r bankbit = 0x00100000 << (((VIA2_iA7 << 1) | VIA2_iA6) << 1);
		if (0 != (addr & bankbit)) {
#if kRAMb_Size != 0
			*RealStart = kRAMa_Size + RAM;
			*RealSize = kRAMb_Size;
#else
			/* fail */
#endif
		} else {
			*RealStart = RAM;
			*RealSize = kRAMa_Size;
		}
	}
#elif (0 == kRAMb_Size) || (kRAMa_Size == kRAMb_Size)
	UnusedParam(addr);

	*RealStart = RAM;
	*RealSize = kRAM_Size;
#else
	/* unbalanced memory */
	if (0 != (addr & kRAMa_Size)) {
		*RealStart = kRAMa_Size + RAM;
		*RealSize = kRAMb_Size;
	} else {
		*RealStart = RAM;
		*RealSize = kRAMa_Size;
	}
#endif
}

LOCALPROC get_ROM_realblock(blnr WriteMem, CPTR addr,
	ui3p *RealStart, ui5b *RealSize)
{
	UnusedParam(addr);

	if (WriteMem) {
		/* fail */
	} else {
		*RealStart = ROM;
		*RealSize = kROM_Size;
	}
}

LOCALPROC get_address24_realblock(blnr WriteMem, CPTR addr,
	ui3p *RealStart, ui5b *RealSize)
{
	if ((addr >> kRAM_ln2Spc) == (kRAM_Base >> kRAM_ln2Spc)) {
		if (MemOverlay) {
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
			ReportAbnormal("Overlay with 24 bit addressing");
#endif
			if ((addr & Overlay_ROM_CmpZeroMask) != 0) {
				/* fail */
			} else {
				get_ROM_realblock(WriteMem, addr,
					RealStart, RealSize);
			}
		} else {
			get_RAM_realblock(WriteMem, addr,
				RealStart, RealSize);
		}
	} else
#if IncludeVidMem && ((CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx))
	if ((addr >= 0x900000) && ((addr < 0xA00000))) {
		*RealStart = VidMem;
		*RealSize = ((kVidMemRAM_Size - 1) & 0x0FFFFF) + 1;
	} else
#endif
#if IncludeVidMem && (CurEmMd != kEmMd_II) && (CurEmMd != kEmMd_IIx)
	if ((addr >> kVidMem_ln2Spc) == (kVidMem_Base >> kVidMem_ln2Spc)) {
		*RealStart = VidMem;
		*RealSize = kVidMemRAM_Size;
	} else
#endif
	if ((addr >> kROM_ln2Spc) == (kROM_Base >> kROM_ln2Spc)) {
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
		if (MemOverlay) {
			ReportAbnormal("Overlay with 24 bit addressing");
		}
#elif CurEmMd >= kEmMd_SE
		if (MemOverlay != 0) {
			MemOverlay = 0;
			SetUpMemBanks();
		}
#endif
		if ((addr & ROM_CmpZeroMask) != 0) {
			/* fail */
		} else {
			get_ROM_realblock(WriteMem, addr,
				RealStart, RealSize);
		}
	} else
#if (CurEmMd != kEmMd_II) && (CurEmMd != kEmMd_IIx)
	if ((addr >> 19) == (kRAM_Overlay_Base >> 19)) {
		if (MemOverlay) {
			get_RAM_realblock(WriteMem, addr,
				RealStart, RealSize);
		}
	} else
#endif
	{
		/* fail */
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
		ReportAbnormal("bad memory access");
#endif
	}
}

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
LOCALPROC get_address32_realblock(blnr WriteMem, CPTR addr,
	ui3p *RealStart, ui5b *RealSize)
{
	if ((addr >> 30) == (0x00000000 >> 30)) {
		if (MemOverlay) {
			get_ROM_realblock(WriteMem, addr,
				RealStart, RealSize);
		} else {
			get_RAM_realblock(WriteMem, addr,
				RealStart, RealSize);
		}
	} else
	if ((addr >> 28) == (0x40000000 >> 28)) {
		get_ROM_realblock(WriteMem, addr,
			RealStart, RealSize);
	} else
#if 0
	/* haven't persuaded emulated computer to look here yet. */
	if ((addr >> 28) == (0x90000000 >> 28)) {
		/* NuBus super space */
		*RealStart = VidMem;
		*RealSize = kVidMemRAM_Size;
	} else
#endif
	if ((addr >> 28) == (0xF0000000 >> 28)) {
		/* Standard NuBus space */
		if ((addr >> 24) == (0xF9000000 >> 24)) {
			/* if ((addr >= 0xFA000000 - kVidROM_Size) && (addr < 0xFA000000)) */
			/* if (addr >= 0xF9800000) */
			if ((addr >> 20) == (0xF9F00000 >> 20))
			{
				if (WriteMem) {
					/* fail */
				} else {
					*RealStart = VidROM;
					*RealSize = kVidROM_Size;
				}
			} else {
#if kVidMemRAM_Size <= 0x00100000
				*RealStart = VidMem;
				*RealSize = kVidMemRAM_Size;
#else
				/* ugly kludge to allow more 1M of Video Memory */
				int i = (addr >> 20) & 0xF;
				if (i >= 9) {
					i -= 9;
				}
				*RealStart = VidMem + ((i << 20) & (kVidMemRAM_Size - 1));
				*RealSize = 0x00100000;
#endif
			}
		} else {
			/* fail */
		}
	} else
	{
		/* fail */
	}
}
#endif

LOCALPROC GetBankAddr(blnr WriteMem, CPTR addr,
	ui3p *RealStart)
{
	ui5b bi = bankindex(addr);
	ui3b **CurBanks = WriteMem ? BankWritAddr : BankReadAddr;
	ui3p RealStart0 = CurBanks[bi];

	if (RealStart0 == nullpr) {
		ui5b RealSize0;
		get_address24_realblock(WriteMem, addr,
			&RealStart0, &RealSize0);
		if (RealStart0 != nullpr) {
			RealStart0 += ((bi << ln2BytesPerMemBank)
				& (RealSize0 - 1));
			CurBanks[bi] = RealStart0;
		}
	}

	*RealStart = RealStart0;
	/* *RealSize = BytesPerMemBank; */
}

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
LOCALFUNC ui5b MM_IOAccess(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	if ((addr >= 0) && (addr < 0x2000)) {
		if (! ByteSize) {
			ReportAbnormal("access VIA1 word");
		} else if ((addr & 1) != 0) {
			ReportAbnormal("access VIA1 odd");
		} else {
			if ((addr & 0x000001FE) != 0x00000000) {
				ReportAbnormal("access VIA1 nonstandard address");
			}
			Data = VIA1_Access(Data, WriteMem, (addr >> 9) & kVIA1_Mask);
		}
	} else
	if ((addr >= 0x2000) && (addr < 0x4000)) {
		if (! ByteSize) {
			if ((! WriteMem) && ((0x3e00 == addr) || (0x3e02 == addr))) {
				/* for weirdness at offset 0x71E in ROM */
				Data = (VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask) << 8)
					| VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);

			} else {
				ReportAbnormal("access VIA2 word");
			}
		} else if ((addr & 1) != 0) {
			if (0x3FFF == addr) {
				/* for weirdness at offset 0x7C4 in ROM. looks like bug. */
				Data = VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);
			} else {
				ReportAbnormal("access VIA2 odd");
			}
		} else {
			if ((addr & 0x000001FE) != 0x00000000) {
				ReportAbnormal("access VIA2 nonstandard address");
			}
			Data = VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);
		}
	} else
	if ((addr >= 0x4000) && (addr < 0x6000)) {
		if (! ByteSize) {
			ReportAbnormal("Attemped Phase Adjust");
		} else
		{
			if ((addr & 0x1FF9) != 0x00000000) {
				ReportAbnormal("access SCC nonstandard address");
			}
			Data = SCC_Access(Data, WriteMem, (addr >> 1) & kSCC_Mask);
		}
	} else
	if ((addr >= 0x0C000) && (addr < 0x0E000)) {
		if (ByteSize) {
			ReportAbnormal("access Sony byte");
		} else if ((addr & 1) != 0) {
			ReportAbnormal("access Sony odd");
		} else if (! WriteMem) {
			ReportAbnormal("access Sony read");
		} else {
			Extn_Access(Data, (addr >> 1) & 0x0F);
		}
	} else
	if ((addr >= 0x10000) && (addr < 0x12000)) {
		if (! ByteSize) {
			ReportAbnormal("access SCSI word");
		} else {
			if ((addr & 0x1F8F) != 0x00000000) {
				ReportAbnormal("access SCC nonstandard address");
			}
			Data = SCSI_Access(Data, WriteMem, (addr >> 4) & 0x07);
		}
	} else
	if ((addr >= 0x14000) && (addr < 0x16000)) {
		if (! ByteSize) {
			if (WriteMem) {
				(void) ASC_Access((Data >> 8) & 0x00FF, WriteMem, addr & kASC_Mask);
				Data = ASC_Access((Data) & 0x00FF, WriteMem, (addr + 1) & kASC_Mask);
			} else {
				ReportAbnormal("access ASC word");
			}
		} else {
			Data = ASC_Access(Data, WriteMem, addr & kASC_Mask);
		}
	} else
	if ((addr >= 0x16000) && (addr < 0x18000)) {
		if (! ByteSize) {
			ReportAbnormal("access IWM word");
		} else if ((addr & 1) != 0) {
			ReportAbnormal("access IWM odd");
		} else {
			Data = IWM_Access(Data, WriteMem, (addr >> 9) & kIWM_Mask);
		}
	} else
#if 0
	if ((addr >= 0x1C000) && (addr < 0x1E000)) {
		/* fail, nothing supposed to be here, but rom accesses it anyway */
		if ((addr != 0x1DA00) && (addr != 0x1DC00)) {
			ReportAbnormal("another unknown access");
		}
	} else
#endif
	{
		ReportAbnormal("MM_Access fails");
	}
	return Data;
}
#endif

GLOBALFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
	if (Addr32) {
		ui3p RealStart = nullpr;
		ui5b RealSize;
		ui3p m;

		get_address32_realblock(WriteMem, addr,
			&RealStart, &RealSize);
		if (nullpr != RealStart) {
			m = RealStart + (addr & (RealSize - 1));

			if (ByteSize) {
				if (WriteMem) {
					*m = Data;
				} else {
					Data = (si5b)(si3b)*m;
				}
			} else {
				if (WriteMem) {
					do_put_mem_word(m, Data);
				} else {
					Data = (si5b)(si4b)do_get_mem_word(m);
				}
			}
		} else {
			if ((addr >> 24) == (0x50000000 >> 24)) {
				Data = MM_IOAccess(Data, WriteMem, ByteSize, addr & 0x1FFFF);
			} else
			if ((addr >= 0x58000000) && (addr < 0x58000004)) {
				/* test hardware. fail */
			} else
			{
				/* ReportAbnormal("Other IO access"); */
			}
		}
	} else
#endif
	{
		CPTR mAddressBus = addr & kAddrMask;

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
		if ((mAddressBus >> 20) == (0x00F00000 >> 20)) {
			Data = MM_IOAccess(Data, WriteMem, ByteSize, mAddressBus & 0x1FFFF);
		} else
#else
		if ((mAddressBus >> kVIA1_ln2Spc) == (kVIA1_Block_Base >> kVIA1_ln2Spc)) {
			if (! ByteSize) {
				ReportAbnormal("access VIA word");
			} else if ((mAddressBus & 1) != 0) {
				ReportAbnormal("access VIA odd");
			} else {
#if CurEmMd != kEmMd_PB100
				if ((mAddressBus & 0x000FE1FE) != 0x000FE1FE) {
					ReportAbnormal("access VIA nonstandard address");
				}
#endif
				Data = VIA1_Access(Data, WriteMem, (mAddressBus >> 9) & kVIA1_Mask);
			}
		} else
		if ((mAddressBus >> kSCC_ln2Spc) == (kSCCRd_Block_Base >> kSCC_ln2Spc)) {
#if CurEmMd >= kEmMd_SE
			if ((mAddressBus & 0x00100000) == 0) {
				ReportAbnormal("access SCC unassigned address");
			} else
#endif
			if (! ByteSize) {
				ReportAbnormal("Attemped Phase Adjust");
			} else if (WriteMem != ((mAddressBus & 1) != 0)) {
				if (WriteMem) {
#if CurEmMd >= kEmMd_512Ke
#if CurEmMd != kEmMd_PB100
					ReportAbnormal("access SCC even/odd");
					/*
						This happens on boot with 64k ROM.
					*/
#endif
#endif
				} else {
					SCC_Reset();
				}
			} else
#if CurEmMd != kEmMd_PB100
			if (WriteMem != (mAddressBus >= kSCCWr_Block_Base)) {
				ReportAbnormal("access SCC wr/rd base wrong");
			} else
#endif
			{
#if CurEmMd != kEmMd_PB100
				if ((mAddressBus & 0x001FFFF8) != 0x001FFFF8) {
					ReportAbnormal("access SCC nonstandard address");
				}
#endif
				Data = SCC_Access(Data, WriteMem, (mAddressBus >> 1) & kSCC_Mask);
			}
		} else
		if ((mAddressBus >> kExtn_ln2Spc) == (kExtn_Block_Base >> kExtn_ln2Spc)) {
			if (ByteSize) {
				ReportAbnormal("access Sony byte");
			} else if ((mAddressBus & 1) != 0) {
				ReportAbnormal("access Sony odd");
			} else if (! WriteMem) {
				ReportAbnormal("access Sony read");
			} else {
				Extn_Access(Data, (mAddressBus >> 1) & 0x0F);
			}
		} else
#if CurEmMd == kEmMd_PB100
		if ((mAddressBus >> kASC_ln2Spc) == (kASC_Block_Base >> kASC_ln2Spc)) {
			if (! ByteSize) {
				ReportAbnormal("access ASC word");
			} else {
				Data = ASC_Access(Data, WriteMem, mAddressBus & kASC_Mask);
			}
		} else
#endif
		if ((mAddressBus >> kSCSI_ln2Spc) == (kSCSI_Block_Base >> kSCSI_ln2Spc)) {
			if (! ByteSize) {
				ReportAbnormal("access SCSI word");
			} else if (WriteMem != ((mAddressBus & 1) != 0)) {
				ReportAbnormal("access SCSI even/odd");
			} else {
				Data = SCSI_Access(Data, WriteMem, (mAddressBus >> 4) & 0x07);
			}
		} else
		if ((mAddressBus >> kIWM_ln2Spc) == (kIWM_Block_Base >> kIWM_ln2Spc)) {
#if CurEmMd >= kEmMd_SE
			if ((mAddressBus & 0x00100000) == 0) {
				ReportAbnormal("access IWM unassigned address");
			} else
#endif
			if (! ByteSize) {
#if ExtraAbnormalReports
				ReportAbnormal("access IWM word");
				/*
					This happens when quitting 'Glider 3.1.2'.
					perhaps a bad handle is being disposed of.
				*/
#endif
			} else if ((mAddressBus & 1) == 0) {
				ReportAbnormal("access IWM even");
			} else {
#if CurEmMd != kEmMd_PB100
				if ((mAddressBus & 0x001FE1FF) != 0x001FE1FF) {
					ReportAbnormal("access IWM nonstandard address");
				}
#endif
				Data = IWM_Access(Data, WriteMem, (mAddressBus >> 9) & kIWM_Mask);
			}
		} else
#endif
		{
			ui3p RealStart;

			GetBankAddr(WriteMem, mAddressBus, &RealStart);
			if (nullpr != RealStart) {
				ui3p m = RealStart + (mAddressBus & (BytesPerMemBank - 1));
				if (ByteSize) {
					if (WriteMem) {
						*m = Data;
					} else {
						Data = (si5b)(si3b)*m;
					}
				} else {
					if (WriteMem) {
						do_put_mem_word(m, Data);
					} else {
						Data = (si5b)(si4b)do_get_mem_word(m);
					}
				}
			}
		}
	}
	return Data;
}

GLOBALFUNC ui3r get_vm_byte(CPTR addr)
{
	ui3p ba = BankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return *m;
	} else {
		return (ui3b) MM_Access(0, falseblnr, trueblnr, addr);
	}
}

GLOBALFUNC ui4r get_vm_word(CPTR addr)
{
	ui3p ba = BankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return do_get_mem_word(m);
	} else {
		return (ui4b) MM_Access(0, falseblnr, falseblnr, addr);
	}
}

GLOBALFUNC ui5r get_vm_long(CPTR addr)
{
	ui3p ba = BankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return do_get_mem_long(m);
	} else {
		ui5r hi = get_vm_word(addr);
		ui5r lo = get_vm_word(addr + 2);
		return (ui5r) ((hi << 16) & 0xFFFF0000)
			| (lo & 0x0000FFFF);
	}
}

GLOBALPROC put_vm_byte(CPTR addr, ui3r b)
{
	ui3p ba = BankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		*m = b;
	} else {
		(void) MM_Access(b & 0x00FF, trueblnr, trueblnr, addr);
	}
}

GLOBALPROC put_vm_word(CPTR addr, ui4r w)
{
	ui3p ba = BankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		do_put_mem_word(m, w);
	} else {
		(void) MM_Access(w & 0x0000FFFF, trueblnr, falseblnr, addr);
	}
}

GLOBALPROC put_vm_long(CPTR addr, ui5r l)
{
	ui3p ba = BankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		do_put_mem_long(m, l);
	} else {
		put_vm_word(addr, l >> 16);
		put_vm_word(addr + 2, l);
	}
}

GLOBALPROC MemOverlay_ChangeNtfy(void)
{
#if CurEmMd <= kEmMd_Plus
	SetUpMemBanks();
#endif
}

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
GLOBALPROC Addr32_ChangeNtfy(void)
{
	SetUpMemBanks();
}
#endif


/*
	unlike in the real Mac Plus, Mini vMac
	will allow misaligned memory access,
	since it is easier to allow it than
	it is to correctly simulate a bus error
	and back out of the current instruction.
*/

LOCALPROC get_address_realblock(blnr WriteMem, CPTR addr,
	ui3p *RealStart, ui5b *RealSize)
{
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
	if (Addr32) {
		*RealStart = nullpr;
		get_address32_realblock(WriteMem, addr,
			RealStart, RealSize);
	} else
#endif
	{
		*RealSize = BytesPerMemBank;
		GetBankAddr(WriteMem, addr & kAddrMask, RealStart);
	}
}

GLOBALFUNC ui3p get_real_address0(ui5b L, blnr WritableMem, CPTR addr,
	ui5b *actL)
{
	ui3p RealStart;
	ui5b RealSize;
	ui3p p;

	get_address_realblock(WritableMem, addr,
		&RealStart, &RealSize);
	if (nullpr == RealStart) {
		*actL = 0;
		p = nullpr;
	} else {
		ui5b bankoffset = addr & (RealSize - 1);
		ui5b bankleft = RealSize - bankoffset;
		p = bankoffset + RealStart;
		if (bankleft >= L) {
			/* this block is big enough (by far the most common case) */
			*actL = L;
		} else {
			/*
				not big enough, look if following block
				is contiguous in real memory.
			*/
			ui3p bankend;
			ui5b n = L;
label_1:
			addr += bankleft;
			n -= bankleft;
			bankend = RealSize + RealStart;

			get_address_realblock(WritableMem, addr,
				&RealStart, &RealSize);
			if ((nullpr == RealStart)
				|| (RealStart != bankend))
			{
				/* following block not contiguous */
				*actL = L - n;
			} else if (RealSize >= n) {
				/* following block is contiguous and big enough */
				*actL = L; /* ok */
			} else {
				bankoffset = addr & (RealSize - 1);
				if (bankoffset != 0) {
					ReportAbnormal("problem with get_address_realblock");
				}
				bankleft = RealSize;
				goto label_1;
			}
		}
	}

	return p;
}

GLOBALFUNC ui3p get_real_address(ui5b L, blnr WritableMem, CPTR addr)
{
	ui5b actL;
	ui3p p = get_real_address0(L, WritableMem, addr, &actL);

	return (L == actL) ? p : nullpr;
}

GLOBALVAR blnr InterruptButton = falseblnr;

GLOBALPROC SetInterruptButton(blnr v)
{
	if (InterruptButton != v) {
		InterruptButton = v;
		VIAorSCCinterruptChngNtfy();
	}
}

LOCALPROC DoMacReset(void)
{
	Sony_EjectAllDisks();
	EmulatedHardwareZap();
}

GLOBALPROC InterruptReset_Update(void)
{
	SetInterruptButton(falseblnr);
		/*
			in case has been set. so only stays set
			for 60th of a second.
		*/

	if (WantMacInterrupt) {
		SetInterruptButton(trueblnr);
		WantMacInterrupt = falseblnr;
	}
	if (WantMacReset) {
		DoMacReset();
		WantMacReset = falseblnr;
	}
}

LOCALVAR ui3b CurIPL = 0;

GLOBALPROC VIAorSCCinterruptChngNtfy(void)
{
#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
	ui3b NewIPL;

	if (InterruptButton) {
		NewIPL = 7;
	} else if (SCCInterruptRequest) {
		NewIPL = 4;
	} else if (VIA2_InterruptRequest) {
		NewIPL = 2;
	} else if (VIA1_InterruptRequest) {
		NewIPL = 1;
	} else {
		NewIPL = 0;
	}
#else
	ui3b VIAandNotSCC = VIA1_InterruptRequest
		& ~ SCCInterruptRequest;
	ui3b NewIPL = VIAandNotSCC
		| (SCCInterruptRequest << 1)
		| (InterruptButton << 2);
#endif
	if (NewIPL != CurIPL) {
		CurIPL = NewIPL;
		m68k_IPLchangeNtfy();
	}
}

LOCALFUNC blnr AddrSpac_Init(void)
{
	int i;

	for (i = 0; i < kNumWires; i++) {
		Wires[i] = 1;
	}

	MINEM68K_Init(BankReadAddr, BankWritAddr,
		&CurIPL);
	EmulatedHardwareZap();
	return trueblnr;
}

LOCALPROC Memory_Reset(void)
{
	MemOverlay = 1;
	SetUpMemBanks();
}

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
EXPORTPROC PowerOff_ChangeNtfy(void);
GLOBALPROC PowerOff_ChangeNtfy(void)
{
	if (! VIA2_iB2) {
		ForceMacOff = trueblnr;
	}
}
#endif

/* extension mechanism */

#if IncludeExtnPbufs
LOCALFUNC tMacErr PbufTransferVM(CPTR Buffera,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	tMacErr result;
	ui5b contig;
	ui3p Buffer;

label_1:
	if (0 == count) {
		result = mnvm_noErr;
	} else {
		Buffer = get_real_address0(count, ! IsWrite, Buffera, &contig);
		if (0 == contig) {
			result = mnvm_miscErr;
		} else {
			PbufTransfer(Buffer, i, offset, contig, IsWrite);
			offset += contig;
			Buffera += contig;
			count -= contig;
			goto label_1;
		}
	}

	return result;
}
#endif

#if IncludePbufs
GLOBALFUNC tMacErr CheckPbuf(tPbuf Pbuf_No)
{
	tMacErr result;

	if (Pbuf_No >= NumPbufs) {
		result = mnvm_nsDrvErr;
	} else if (! PbufIsAllocated(Pbuf_No)) {
		result = mnvm_offLinErr;
	} else {
		result = mnvm_noErr;
	}

	return result;
}
#endif

#if IncludeExtnPbufs
#define kCmndPbufFeatures 1
#define kCmndPbufNew 2
#define kCmndPbufDispose 3
#define kCmndPbufGetSize 4
#define kCmndPbufTransfer 5
#endif

#if IncludeExtnPbufs
LOCALPROC ExtnParamBuffers_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndPbufFeatures:
			put_vm_long(p + ExtnDat_params + 0, 0);
			result = mnvm_noErr;
			break;
		case kCmndPbufNew:
			{
				tPbuf Pbuf_No;
				ui5b count = get_vm_long(p + ExtnDat_params + 4);
				/* reserved word at offset 2, should be zero */
				result = PbufNew(count, &Pbuf_No);
				put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
			}
			break;
		case kCmndPbufDispose:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */
				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					PbufDispose(Pbuf_No);
				}
			}
			break;
		case kCmndPbufGetSize:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */

				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					put_vm_long(p + ExtnDat_params + 4, PbufSize[Pbuf_No]);
				}
			}
			break;
		case kCmndPbufTransfer:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */
				ui5r offset = get_vm_long(p + ExtnDat_params + 4);
				ui5r count = get_vm_long(p + ExtnDat_params + 8);
				CPTR Buffera = get_vm_long(p + ExtnDat_params + 12);
				blnr IsWrite = (get_vm_word(p + ExtnDat_params + 16) != 0);
				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					ui5r endoff = offset + count;
					if ((endoff < offset) /* overflow */
						|| (endoff > PbufSize[Pbuf_No]))
					{
						result = mnvm_eofErr;
					} else {
						result = PbufTransferVM(Buffera,
							Pbuf_No, offset, count, IsWrite);
					}
				}
			}
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}
#endif

#if IncludeExtnHostTextClipExchange
#define kCmndHTCEFeatures 1
#define kCmndHTCEExport 2
#define kCmndHTCEImport 3
#endif

#if IncludeExtnHostTextClipExchange
LOCALPROC ExtnHostTextClipExchange_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndHTCEFeatures:
			put_vm_long(p + ExtnDat_params + 0, 0);
			result = mnvm_noErr;
			break;
		case kCmndHTCEExport:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);

				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					result = HTCEexport(Pbuf_No);
				}
			}
			break;
		case kCmndHTCEImport:
			{
				tPbuf Pbuf_No;
				result = HTCEimport(&Pbuf_No);
				put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
			}
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}
#endif

#define kFindExtnExtension 0x64E1F58A
#define kDiskDriverExtension 0x4C9219E6
#if IncludeExtnPbufs
#define kHostParamBuffersExtension 0x314C87BF
#endif
#if IncludeExtnHostTextClipExchange
#define kHostClipExchangeExtension 0x27B130CA
#endif

#define kCmndFindExtnFind 1
#define kCmndFindExtnId2Code 2
#define kCmndFindExtnCount 3

#define kParamFindExtnTheExtn 8
#define kParamFindExtnTheId 12

LOCALPROC ExtnFind_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndFindExtnFind:
			{
				ui5b extn = get_vm_long(p + kParamFindExtnTheExtn);

				if (extn == kDiskDriverExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnDisk);
					result = mnvm_noErr;
				} else
#if IncludeExtnPbufs
				if (extn == kHostParamBuffersExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnParamBuffers);
					result = mnvm_noErr;
				} else
#endif
#if IncludeExtnHostTextClipExchange
				if (extn == kHostClipExchangeExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnHostTextClipExchange);
					result = mnvm_noErr;
				} else
#endif
				if (extn == kFindExtnExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnFindExtn);
					result = mnvm_noErr;
				} else
				{
					/* not found */
				}
			}
			break;
		case kCmndFindExtnId2Code:
			{
				ui4r extn = get_vm_word(p + kParamFindExtnTheId);

				if (extn == kExtnDisk) {
					put_vm_long(p + kParamFindExtnTheExtn, kDiskDriverExtension);
					result = mnvm_noErr;
				} else
#if IncludeExtnPbufs
				if (extn == kExtnParamBuffers) {
					put_vm_long(p + kParamFindExtnTheExtn, kHostParamBuffersExtension);
					result = mnvm_noErr;
				} else
#endif
#if IncludeExtnHostTextClipExchange
				if (extn == kExtnHostTextClipExchange) {
					put_vm_long(p + kParamFindExtnTheExtn, kHostClipExchangeExtension);
					result = mnvm_noErr;
				} else
#endif
				if (extn == kExtnFindExtn) {
					put_vm_long(p + kParamFindExtnTheExtn, kFindExtnExtension);
					result = mnvm_noErr;
				} else
				{
					/* not found */
				}
			}
			break;
		case kCmndFindExtnCount:
			put_vm_word(p + kParamFindExtnTheId, kNumExtns);
			result = mnvm_noErr;
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}

#define kDSK_Params_Hi 0
#define kDSK_Params_Lo 1
#define kDSK_QuitOnEject 3 /* obsolete */

LOCALVAR ui4b ParamAddrHi;

LOCALPROC Extn_Access(ui5b Data, CPTR addr)
{
	switch (addr) {
		case kDSK_Params_Hi:
			ParamAddrHi = Data;
			break;
		case kDSK_Params_Lo:
			{
				CPTR p = ParamAddrHi << 16 | Data;

				ParamAddrHi = (ui4b) - 1;
				if (kcom_callcheck == get_vm_word(p + ExtnDat_checkval)) {
					put_vm_word(p + ExtnDat_checkval, 0);

					switch (get_vm_word(p + ExtnDat_extension)) {
						case kExtnFindExtn:
							ExtnFind_Access(p);
							break;
#if EmVidCard
						case kExtnVideo:
							ExtnVideo_Access(p);
							break;
#endif
#if IncludeExtnPbufs
						case kExtnParamBuffers:
							ExtnParamBuffers_Access(p);
							break;
#endif
#if IncludeExtnHostTextClipExchange
						case kExtnHostTextClipExchange:
							ExtnHostTextClipExchange_Access(p);
							break;
#endif
						case kExtnDisk:
							ExtnDisk_Access(p);
							break;
						case kExtnSony:
							ExtnSony_Access(p);
							break;
						default:
							put_vm_word(p + ExtnDat_result, mnvm_controlErr);
							break;
					}
				}
			}
			break;
		case kDSK_QuitOnEject:
			/* obsolete, kept for compatibility */
			Sony_SetQuitOnEject();
			break;
	}
}

LOCALPROC Extn_Reset(void)
{
	ParamAddrHi = (ui4b) - 1;
}

/* user event queue utilities */

#if HaveMasterMyEvtQLock
GLOBALVAR ui4r MasterMyEvtQLock = 0;
	/*
		Takes a few ticks to process button event because
		of debounce code of Mac. So have this mechanism
		to prevent processing further events meanwhile.
	*/
#endif

GLOBALFUNC MyEvtQEl * MyEvtQOutP(void)
{
	MyEvtQEl *p = nullpr;
	if (MyEvtQIn != MyEvtQOut) {
		p = &MyEvtQA[MyEvtQOut & MyEvtQIMask];
	}
	return p;
}

GLOBALFUNC blnr FindKeyEvent(int *VirtualKey, blnr *KeyDown)
{
	MyEvtQEl *p;

	if (
#if HaveMasterMyEvtQLock
		(0 == MasterMyEvtQLock) &&
#endif
		(nullpr != (p = MyEvtQOutP())))
	{
		if (MyEvtQElKindKey == p->kind) {
			*VirtualKey = p->u.press.key;
			*KeyDown = p->u.press.down;
			++MyEvtQOut;
			return trueblnr;
		}
	}

	return falseblnr;
}

/* task management */

LOCALPROC ICT_DoTask(int taskid)
{
	switch (taskid) {
#if EmClassicKbrd
		case kICT_Kybd_ReceiveEndCommand:
			DoKybd_ReceiveEndCommand();
			break;
		case kICT_Kybd_ReceiveCommand:
			DoKybd_ReceiveCommand();
			break;
#endif
#if EmADB
		case kICT_ADB_NewState:
			ADB_DoNewState();
			break;
#endif
#if EmPMU
		case kICT_PMU_Task:
			PMU_DoTask();
			break;
#endif
		case kICT_VIA1_Timer1Check:
			VIA1_DoTimer1Check();
			break;
		case kICT_VIA1_Timer2Check:
			VIA1_DoTimer2Check();
			break;
#if EmVIA2
		case kICT_VIA2_Timer1Check:
			VIA2_DoTimer1Check();
			break;
		case kICT_VIA2_Timer2Check:
			VIA2_DoTimer2Check();
			break;
#endif
		default:
			ReportAbnormal("unknown taskid in ICT_DoTask");
			break;
	}
}

#ifdef _VIA_Debug
#include <stdio.h>
#endif

LOCALVAR blnr ICTactive[kNumICTs];
LOCALVAR iCountt ICTwhen[kNumICTs];

LOCALPROC ICT_Zap(void)
{
	int i;

	for (i = 0; i < kNumICTs; i++) {
		ICTactive[i] = falseblnr;
	}
}

LOCALPROC InsertICT(int taskid, iCountt when)
{
	ICTwhen[taskid] = when;
	ICTactive[taskid] = trueblnr;
}

LOCALVAR iCountt NextiCount = 0;

GLOBALFUNC iCountt GetCuriCount(void)
{
	return NextiCount - GetInstructionsRemaining();
}

GLOBALPROC ICT_add(int taskid, ui5b n)
{
	/* n must be > 0 */
	ui5b x = GetInstructionsRemaining();
	ui5b when = NextiCount - x + n;

#ifdef _VIA_Debug
	fprintf(stderr, "ICT_add: %d, %d, %d\n", when, taskid, n);
#endif
	InsertICT(taskid, when);

	if (x > n) {
		SetInstructionsRemaining(n);
		NextiCount = when;
	}
}

LOCALPROC ICT_DoCurrentTasks(void)
{
	int i;

	for (i = 0; i < kNumICTs; i++) {
		if (ICTactive[i]) {
			if (ICTwhen[i] == NextiCount) {
				ICTactive[i] = falseblnr;
#ifdef _VIA_Debug
				fprintf(stderr, "doing task %d, %d\n", NextiCount, i);
#endif
				ICT_DoTask(i);

				/*
					A Task may set the time of
					any task, including itself.
					But it cannot set any task
					to execute immediately, so
					one pass is sufficient.
				*/
			}
		}
	}
}

LOCALFUNC ui5b ICT_DoGetNext(ui5b maxn)
{
	int i;
	ui5b v = maxn;

	for (i = 0; i < kNumICTs; i++) {
		if (ICTactive[i]) {
			ui5b d = ICTwhen[i] - NextiCount;
			/* at this point d must be > 0 */
			if (d < v) {
#ifdef _VIA_Debug
				fprintf(stderr, "coming task %d, %d, %d\n", NextiCount, i, d);
#endif
				v = d;
			}
		}
	}
	return v;
}

LOCALPROC m68k_go_nInstructions_1(ui5b n)
{
	ui5b n2;
	ui5b StopiCount = NextiCount + n;
	do {
		ICT_DoCurrentTasks();
		n2 = ICT_DoGetNext(n);
		NextiCount += n2;
		m68k_go_nInstructions(n2);
		n = StopiCount - NextiCount;
	} while (n != 0);
}

#define InstructionsPerSubTick (InstructionsPerTick / kNumSubTicks)

LOCALVAR ui5b ExtraSubTicksToDo = 0;

GLOBALPROC DoEmulateOneTick(void)
{
	long KiloInstructionsCounter = 0;

	SixtiethSecondNotify();

	do {
		m68k_go_nInstructions_1(InstructionsPerSubTick);
		SubTickNotify(KiloInstructionsCounter);
		++KiloInstructionsCounter;
	} while (KiloInstructionsCounter < kNumSubTicks);

	if (! SpeedLimit) {
		ExtraSubTicksToDo = (ui5b) -1;
	} else {
		ui5b ExtraAdd = (kNumSubTicks << SpeedValue) - kNumSubTicks;
		ui5b ExtraLimit = ExtraAdd << 3;

		ExtraSubTicksToDo += ExtraAdd;
		if (ExtraSubTicksToDo > ExtraLimit) {
			ExtraSubTicksToDo = ExtraLimit;
		}
	}
}

GLOBALPROC DoEmulateExtraTime(void)
{
	if (ExtraTimeNotOver() && (ExtraSubTicksToDo > 0)) {
		ExtraTimeBeginNotify();
		do {
			m68k_go_nInstructions_1(InstructionsPerSubTick);
			--ExtraSubTicksToDo;
		} while (ExtraTimeNotOver() && (ExtraSubTicksToDo > 0));
		ExtraTimeEndNotify();
	}
}
