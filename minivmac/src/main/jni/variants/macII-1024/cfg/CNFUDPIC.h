/*
	see comment in PICOMMON.h

	This file is automatically generated by the build system,
	which tries to know what options are valid in what
	combinations. Avoid changing this file manually unless
	you know what you're doing.
*/

#define EmClassicKbrd 0
#define EmADB 1
#define EmRTC 1
#define EmPMU 0
#define EmVIA1 1
#define EmVIA2 1
#define Use68020 1
#define EmFPU 1
#define EmMMU 0
#define EmClassicSnd 0
#define EmASC 1

#define CurEmMd kEmMd_II

#define kMyClockMult 2

#define WantCycByPriOp 1
#define WantCloserCyc 0

#define kAutoSlowSubTicks 16384
#define kAutoSlowTime 60

#define kRAMa_Size 0x00400000
#define kRAMb_Size 0x00400000

#define IncludeVidMem 1
#define kVidMemRAM_Size 0x00100000

#define EmVidCard 1
#define kVidROM_Size 0x000800

#define MaxATTListN 20
#define IncludeExtnPbufs 1
#define IncludeExtnHostTextClipExchange 1

#define Sony_SupportDC42 1
#define Sony_SupportTags 0
#define Sony_WantChecksumsUpdated 0
#define Sony_VerifyChecksums 0
#define CaretBlinkTime 0x08
#define SpeakerVol 0x07
#define DoubleClickTime 0x08
#define MenuBlink 0x03
#define AutoKeyThresh 0x06
#define AutoKeyRate 0x03
#define pr_HilColRed 0x0000
#define pr_HilColGreen 0x0000
#define pr_HilColBlue 0x0000


/* the Wire variables are 1/0, not true/false */

enum {

    Wire_unknown_SoundDisable,
#define SoundDisable (Wires[Wire_unknown_SoundDisable])

    Wire_unknown_SoundVolb0,
#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])

    Wire_unknown_SoundVolb1,
#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])

    Wire_unknown_SoundVolb2,
#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])

    Wire_VIA1_iA0_unknown,
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])

    Wire_VIA1_iA1_unknown,
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])

    Wire_VIA1_iA2_unknown,
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])

    Wire_VIA1_iB7_unknown, /* for compatibility with SoundDisable */
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])

    Wire_VIA2_InterruptRequest,
#define VIA2_InterruptRequest (Wires[Wire_VIA2_InterruptRequest])
#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy

    Wire_VIA2_iA7_unknown,
#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])
#define VIA2_iA7_ChangeNtfy Addr32_ChangeNtfy

    Wire_VIA2_iA6_unknown,
#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])
#define VIA2_iA6_ChangeNtfy Addr32_ChangeNtfy

    Wire_VIA2_iB7_unknown,
#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])

    Wire_VIA2_iCB2_unknown,
#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])

    Wire_VIA2_iB2_PowerOff,
#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])
#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy

    Wire_VIA2_iB3_Addr32,
#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])
#define Addr32 (Wires[Wire_VIA2_iB3_Addr32])
#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy

    Wire_VIA1_iA4_MemOverlay,
#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy

    Wire_VIA1_iA5_IWMvSel,
#define IWMvSel (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])

    Wire_VIA1_iA7_SCCwaitrq,
#define SCCwaitrq (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])

    Wire_VIA1_iB0_RTCdataLine,
#define RTCdataLine (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy

    Wire_VIA1_iB1_RTCclock,
#define RTCclock (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy

    Wire_VIA1_iB2_RTCunEnabled,
#define RTCunEnabled (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy

    Wire_VIA1_iA3_SCCvSync,
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])

    Wire_VIA1_iB3_ADB_Int,
#define ADB_Int (Wires[Wire_VIA1_iB3_ADB_Int])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_ADB_Int])

    Wire_VIA1_iB4_ADB_st0,
#define ADB_st0 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy

    Wire_VIA1_iB5_ADB_st1,
#define ADB_st1 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy

    Wire_VIA1_iCB2_ADB_Data,
#define ADB_Data (Wires[Wire_VIA1_iCB2_ADB_Data])
#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_ADB_Data])
#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy

    Wire_VIA1_InterruptRequest,
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy

    Wire_SCCInterruptRequest,
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy

    Wire_ADBMouseDisabled,
#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])

    Wire_VBLinterrupt,
#define Vid_VBLinterrupt (Wires[Wire_VBLinterrupt])
#define VIA2_iA0 (Wires[Wire_VBLinterrupt])

    Wire_VBLintunenbl,
#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])

    kNumWires
};


/* VIA configuration */
#define VIA1_ORA_FloatVal 0xBF
/* bit 6 used to check version of hardware */
#define VIA1_ORB_FloatVal 0xFF
#define VIA1_ORA_CanIn 0x80
#define VIA1_ORA_CanOut 0x3F
#define VIA1_ORB_CanIn 0x09
#define VIA1_ORB_CanOut 0xB7
#define VIA1_IER_Never0 0x00
#define VIA1_IER_Never1 0x58
#define VIA1_CB2modesAllowed 0x01
#define VIA1_CA2modesAllowed 0x01

/* VIA 2 configuration */
#define VIA2_ORA_FloatVal 0xFF
#define VIA2_ORB_FloatVal 0xFF
#define VIA2_ORA_CanIn 0x01
#define VIA2_ORA_CanOut 0xC0
#define VIA2_ORB_CanIn 0x00
#define VIA2_ORB_CanOut 0x8C
#define VIA2_IER_Never0 0x00
#define VIA2_IER_Never1 0xED
#define VIA2_CB2modesAllowed 0x01
#define VIA2_CA2modesAllowed 0x01

#define Mouse_Enabled() (! ADBMouseDisabled)

#define VIA1_iCA1_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy
#define Sixtieth_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy

#define VIA1_iCA2_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy
#define RTC_OneSecond_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy

#define VIA2_iCA1_PulseNtfy VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy
#define Vid_VBLinterrupt_PulseNotify VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy

#define VIA2_iCB1_PulseNtfy VIA2_iCB1_ASC_interrupt_PulseNtfy
#define ASC_interrupt_PulseNtfy VIA2_iCB1_ASC_interrupt_PulseNtfy

#define GetSoundInvertTime VIA1_GetT1InvertTime

#define ADB_ShiftInData VIA1_ShiftOutData
#define ADB_ShiftOutData VIA1_ShiftInData

#define kExtn_Block_Base 0x50F0C000
#define kExtn_ln2Spc 5

#define kROM_Base 0x00800000
#define kROM_ln2Spc 20

#define WantDisasm 0
#define ExtraAbnormalReports 0
