/*
	GLOBGLUE.h

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

#ifdef GLOBGLUE_H
#error "header already included"
#else
#define GLOBGLUE_H
#endif

/* for use by platform specific code */

EXPORTPROC EmulationReserveAlloc(void);
EXPORTFUNC blnr InitEmulation(void);
EXPORTPROC DoEmulateOneTick(void);
EXPORTPROC DoEmulateExtraTime(void);


EXPORTPROC customreset(void);

#define kEmMd_128K        0
#define kEmMd_512Ke       1
#define kEmMd_Plus        2
#define kEmMd_SE          3
#define kEmMd_SEFDHD      4
#define kEmMd_Classic     5
#define kEmMd_PB100       6
#define kEmMd_II          7
#define kEmMd_IIx         8

#define RAMSafetyMarginFudge 4

#define kRAM_Size (kRAMa_Size + kRAMb_Size)
EXPORTVAR(ui3p, RAM)
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in GLOBGLUE.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#if EmVidCard
EXPORTVAR(ui3p, VidROM)
#endif

#if IncludeVidMem
EXPORTVAR(ui3p, VidMem)
#endif




EXPORTPROC MemOverlay_ChangeNtfy(void);

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
EXPORTPROC Addr32_ChangeNtfy(void);
#endif

/*
	representation of pointer into memory of emulated computer.
*/
typedef ui5b CPTR;

/*
	general purpose access of address space
	of emulated computer. (memory and
	memory mapped hardware.)
*/

GLOBALFUNC ui3r get_vm_byte(CPTR addr);
GLOBALFUNC ui4r get_vm_word(CPTR addr);
GLOBALFUNC ui5r get_vm_long(CPTR addr);

GLOBALPROC put_vm_byte(CPTR addr, ui3r b);
GLOBALPROC put_vm_word(CPTR addr, ui4r w);
GLOBALPROC put_vm_long(CPTR addr, ui5r l);

/*
	mapping of address space to real memory
*/

EXPORTFUNC ui3p get_real_address0(ui5b L, blnr WritableMem, CPTR addr,
	ui5b *actL);
EXPORTFUNC ui3p get_real_address(ui5b L, blnr WritableMem, CPTR addr);

/*
	memory access routines that can use when have address
	that is known to be in RAM (and that is in the first
	copy of the ram, not the duplicates, i.e. < kRAM_Size).
*/

#define get_ram_byte(addr) do_get_mem_byte((addr) + RAM)
#define get_ram_word(addr) do_get_mem_word((addr) + RAM)
#define get_ram_long(addr) do_get_mem_long((addr) + RAM)

#define put_ram_byte(addr, b) do_put_mem_byte((addr) + RAM, (b))
#define put_ram_word(addr, w) do_put_mem_word((addr) + RAM, (w))
#define put_ram_long(addr, l) do_put_mem_long((addr) + RAM, (l))

#define get_ram_address(addr) ((addr) + RAM)


/*
	lower level access of address space
	of emulated computer. for direct access when
	need more efficiency, i.e. for cpu emulation.
*/

#define ln2TotAddrBytes 24

#if kEmMd_PB100 == CurEmMd
#define ln2BytesPerMemBank 15
#else
#define ln2BytesPerMemBank 17
#endif
#define ln2NumMemBanks (ln2TotAddrBytes - ln2BytesPerMemBank)

#define NumMemBanks (1UL << ln2NumMemBanks)
#define BytesPerMemBank  (1UL << ln2BytesPerMemBank)
#define MemBanksMask (NumMemBanks - 1)
#define MemBankAddrMask (BytesPerMemBank - 1)

#define bankindex(addr) ((((CPTR)(addr)) >> ln2BytesPerMemBank) & MemBanksMask)

/*
	accessing addresses that don't map to
	real memory, i.e. memory mapped devices
*/

EXPORTFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr);


#if DetailedAbnormalReport
#define ReportAbnormal DoReportAbnormal
EXPORTPROC DoReportAbnormal(char *s);
#else
#define ReportAbnormal(s) DoReportAbnormal()
EXPORTPROC DoReportAbnormal(void);
#endif

EXPORTPROC VIAorSCCinterruptChngNtfy(void);

EXPORTVAR(blnr, InterruptButton)
EXPORTPROC SetInterruptButton(blnr v);
EXPORTPROC InterruptReset_Update(void);

enum {
#if EmClassicKbrd
	kICT_Kybd_ReceiveCommand,
	kICT_Kybd_ReceiveEndCommand,
#endif
#if EmADB
	kICT_ADB_NewState,
#endif
#if EmPMU
	kICT_PMU_Task,
#endif
	kICT_VIA1_Timer1Check,
	kICT_VIA1_Timer2Check,
#if EmVIA2
	kICT_VIA2_Timer1Check,
	kICT_VIA2_Timer2Check,
#endif

	kNumICTs
};

EXPORTPROC ICT_add(int taskid, ui5b n);

#define iCountt ui5b
EXPORTFUNC iCountt GetCuriCount(void);

EXPORTVAR(ui3b, Wires[kNumWires])

#define InstructionsPerTick 12250
	/*
		This a bit too fast on average, but
		if this was much lower, Concertware wouldn't
		work properly with speed limit on. If this was
		much higher, the initial sounds in Dark Castle
		would have static.
		This can only be an approximation, since on
		a real machine the number of instructions
		executed per time can vary by almost a factor
		of two, because different instructions take
		different times.
	*/

#define kNumSubTicks 16


#define HaveMasterMyEvtQLock EmClassicKbrd
#if HaveMasterMyEvtQLock
EXPORTVAR(ui4r, MasterMyEvtQLock)
#endif

EXPORTFUNC MyEvtQEl * MyEvtQOutP(void);
EXPORTFUNC blnr FindKeyEvent(int *VirtualKey, blnr *KeyDown);


/* minivmac extensions */

#define ExtnDat_checkval 0
#define ExtnDat_extension 2
#define ExtnDat_commnd 4
#define ExtnDat_result 6
#define ExtnDat_params 8

#define kCmndVersion 0
#define ExtnDat_version 8

enum {
	kExtnFindExtn, /* must be first */

	kExtnDisk,
	kExtnSony,
#if EmVidCard
	kExtnVideo,
#endif
#if IncludeExtnPbufs
	kExtnParamBuffers,
#endif
#if IncludeExtnHostTextClipExchange
	kExtnHostTextClipExchange,
#endif

	kNumExtns
};

#define kcom_callcheck 0x5B17

#if IncludePbufs
EXPORTFUNC tMacErr CheckPbuf(tPbuf Pbuf_No);
#endif

EXPORTVAR(ui5r, my_disk_icon_addr)
