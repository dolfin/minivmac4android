/*
	see comment in PICOMMON.h

	This file is automatically generated by the build system,
	which tries to know what options are valid in what
	combinations. Avoid changing this file manually unless
	you know what you're doing.
*/

#define EmClassicKbrd 1
#define EmADB 0
#define EmRTC 1
#define EmPMU 0
#define EmVIA1 1
#define EmVIA2 0
#define Use68020 0
#define EmFPU 0
#define EmMMU 0
#define EmClassicSnd 1
#define EmASC 0

#define CurEmMd kEmMd_128K

#define kMyClockMult 1

#define WantCycByPriOp 1
#define WantCloserCyc 0

#define kAutoSlowSubTicks 16384
#define kAutoSlowTime 34

#define kRAMa_Size 0x00020000
#define kRAMb_Size 0

#define IncludeVidMem 0

#define EmVidCard 0

#define MaxATTListN 16
#define IncludeExtnPbufs 1
#define IncludeExtnHostTextClipExchange 0

#define Sony_SupportDC42 1
#define Sony_SupportTags 0
#define Sony_WantChecksumsUpdated 0
#define Sony_VerifyChecksums 0
#define CaretBlinkTime 0x03
#define SpeakerVol 0x07
#define DoubleClickTime 0x05
#define MenuBlink 0x03
#define AutoKeyThresh 0x06
#define AutoKeyRate 0x03


/* the Wire variables are 1/0, not true/false */

enum {

    Wire_VIA1_iA0_SoundVolb0,
#define SoundVolb0 (Wires[Wire_VIA1_iA0_SoundVolb0])
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_SoundVolb0])

    Wire_VIA1_iA1_SoundVolb1,
#define SoundVolb1 (Wires[Wire_VIA1_iA1_SoundVolb1])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_SoundVolb1])

    Wire_VIA1_iA2_SoundVolb2,
#define SoundVolb2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_SoundVolb2])

    Wire_VIA1_iA4_MemOverlay,
#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy

    Wire_VIA1_iA6_SCRNvPage2,
#define SCRNvPage2 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#define VIA1_iA6 (Wires[Wire_VIA1_iA6_SCRNvPage2])

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

    Wire_VIA1_iA3_SoundBuffer,
#define SoundBuffer (Wires[Wire_VIA1_iA3_SoundBuffer])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SoundBuffer])

    Wire_VIA1_iB3_MouseBtnUp,
#define MouseBtnUp (Wires[Wire_VIA1_iB3_MouseBtnUp])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_MouseBtnUp])

    Wire_VIA1_iB4_MouseX2,
#define MouseX2 (Wires[Wire_VIA1_iB4_MouseX2])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_MouseX2])

    Wire_VIA1_iB5_MouseY2,
#define MouseY2 (Wires[Wire_VIA1_iB5_MouseY2])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_MouseY2])

    Wire_VIA1_iCB2_KybdDat,
#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])
#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy

    Wire_VIA1_iB6_SCRNbeamInVid,
#define SCRNbeamInVid (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCRNbeamInVid])

    Wire_VIA1_iB7_SoundDisable,
#define SoundDisable (Wires[Wire_VIA1_iB7_SoundDisable])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_SoundDisable])

    Wire_VIA1_InterruptRequest,
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy

    Wire_SCCInterruptRequest,
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy

    kNumWires
};


/* VIA configuration */
#define VIA1_ORA_FloatVal 0xFF
#define VIA1_ORB_FloatVal 0xFF
#define VIA1_ORA_CanIn 0x80
#define VIA1_ORA_CanOut 0x7F
#define VIA1_ORB_CanIn 0x79
#define VIA1_ORB_CanOut 0x87
#define VIA1_IER_Never0 (1 << 1)
#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))
#define VIA1_CB2modesAllowed 0x01
#define VIA1_CA2modesAllowed 0x01

#define Mouse_Enabled SCC_InterruptsEnabled

#define VIA1_iCA1_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy
#define Sixtieth_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy

#define VIA1_iCA2_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy
#define RTC_OneSecond_PulseNtfy VIA1_iCA2_RTC_OneSecond_PulseNtfy

#define GetSoundInvertTime VIA1_GetT1InvertTime

#define KYBD_ShiftInData VIA1_ShiftOutData
#define KYBD_ShiftOutData VIA1_ShiftInData

#define kExtn_Block_Base 0x00F40000
#define kExtn_ln2Spc 5

#define kROM_Base 0x00400000
#define kROM_ln2Spc 20

#define WantDisasm 0
#define ExtraAbnormalReports 0
