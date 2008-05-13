#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "common.h"

//#define CDR_LOG printf

#define MAKEID(a,b,c,d) ( a + (b << 8) + (c << 16) + (d << 24) )

static u8 US_Cmd25P22[8]={ 0x66,0x6F,0x72,0x20,0x55,0x53,0x2F,0x41 }; // "for US/A"
static u8 US_Cmd25P23[8]={ 0x43,0x58,0x44,0x31,0x37,0x38,0x32,0x42 }; // "CXD1782B"
static u8 US_Cmd25P24[8]={ 0x43,0x58,0x44,0x32,0x35,0x31,0x36,0x51 }; // "CXD2516Q"
static u8 US_Cmd25P25[8]={ 0x43,0x58,0x44,0x31,0x31,0x39,0x39,0x42 }; // "CXD1199B"
static u8 US_SCEx[6] = { 0x09, 0xA9, 0x3D, 0x2B, 0xA5, 0xF4 };  // SCEA U.S.
static u32 US_SCExID = MAKEID('S','C','E','A');

#define CdlSync         0
#define CdlNop	        1
#define CdlSetloc		2
#define CdlPlay	        3
#define CdlForward		4
#define CdlBackward		5
#define CdlReadN		6
#define CdlStandby		7
#define CdlStop	        8
#define CdlPause        9
#define CdlInit 		10
#define CdlMute	        11
#define CdlDemute		12
#define CdlSetfilter	13
#define CdlSetmode		14
#define CdlGetmode      15
#define CdlGetlocL		16
#define CdlGetlocP		17
#define Cdl18     		18
#define CdlGetTN		19
#define CdlGetTD		20
#define CdlSeekL		21
#define CdlSeekP		22
#define CdlTest 		25
#define CdlID   		26
#define CdlReadS		27
#define CdlReset		28
#define CdlReadToc      30

#define AUTOPAUSE		249
#define READ_ACK		250
#define READ			251
#define REPPLAY_ACK		252
#define REPPLAY			253
#define ASYNC			254
/* don't set 255, it's reserved */

char *CmdName[0x100]= {
	"CdlSync",    "CdlNop",       "CdlSetloc",  "CdlPlay",
	"CdlForward", "CdlBackward",  "CdlReadN",   "CdlStandby",
	"CdlStop",    "CdlPause",     "CdlInit",    "CdlMute",
	"CdlDemute",  "CdlSetfilter", "CdlSetmode", "CdlGetmode",
	"CdlGetlocL", "CdlGetlocP",   "Cdl18",      "CdlGetTN",
	"CdlGetTD",   "CdlSeekL",     "CdlSeekP",   NULL,
	NULL,         "CdlTest",      "CdlID",      "CdlReadS",
	"CdlReset",   NULL,           "CDlReadToc", NULL
};

unsigned char Test04[] = { 0 };
unsigned char Test05[] = { 0 };
unsigned char Test20[] = { 0x98, 0x06, 0x10, 0xC3 };
unsigned char Test22[] = { 0x66, 0x6F, 0x72, 0x20, 0x45, 0x75, 0x72, 0x6F };
unsigned char Test23[] = { 0x43, 0x58, 0x44, 0x32, 0x39 ,0x34, 0x30, 0x51 };

// 1x = 75 sectors per second
// PSXCLK = 1 sec in the ps
// so (PSXCLK / 75) / BIAS = cdr read time (linuzappz)
#define cdReadTime (PsxCDClock)

#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to u_char */
#define itob(i)		((i)/10*16 + (i)%10)		/* u_char to BCD */

static struct CdrStat cdrstat;
//static struct SubQ *subq;
cdrStruct cdr;

extern int iSoundMuted;

#define CDR_INT(eCycle) { \
	psxRegs->interrupt|= 0x4; \
	psxRegs->intCycle[2+1] = eCycle; \
	psxRegs->intCycle[2] = psxRegs->cycle; } \


#define CDREAD_INT(eCycle) { \
	psxRegs->interrupt|= 0x40000; \
	psxRegs->intCycle[2+16+1] = eCycle; \
	psxRegs->intCycle[2+16] = psxRegs->cycle; } \

#define StartReading(type) { \
   	cdr.Reading = type; \
  	cdr.FirstSector = 1; \
  	cdr.Readed = 0xff; \
	AddIrqQueue(READ_ACK, 0x800); \
}	

#define StopReading() { \
	if (cdr.Reading) { \
		cdr.Reading = 0; \
		psxRegs->interrupt&=~0x40000; \
	} \
}

#define StopCdda() { \
	if (cdr.Play) { \
		if (!Config.Cdda) CDR_stop(); \
		cdr.StatP&=~0x80; \
		cdr.Play = 0; \
	} \
}

#define SetResultSize(size) { \
    cdr.ResultP = 0; \
	cdr.ResultC = size; \
	cdr.ResultReady = 1; \
}

void ReadTrack() {

	cdr.Prev[0] = itob(cdr.SetSector[0]);
	cdr.Prev[1] = itob(cdr.SetSector[1]);
	cdr.Prev[2] = itob(cdr.SetSector[2]);

#ifdef CDR_LOG
	CDR_LOG("KEY *** %x:%x:%x\n", cdr.Prev[0], cdr.Prev[1], cdr.Prev[2]);
#endif
	cdr.RErr = CDR_readTrack(cdr.SetSector);
}

// cdr.Stat:
#define NoIntr		0
#define DataReady	1
#define Complete	2
#define Acknowledge	3
#define DataEnd		4
#define DiskError	5

void AddIrqQueue(unsigned char irq, unsigned long ecycle) {
	cdr.Irq = irq;
	if (cdr.Stat) {
		cdr.eCycle = ecycle;
	} else {
		CDR_INT(ecycle);
	}
}

void cdrInterrupt() {
	int i;
	unsigned char Irq = cdr.Irq;

	if (cdr.Stat) {
		CDR_INT(0x800);
		return;
	}

#ifdef CDR_LOG
	CDR_LOG("CD INT IRQ %u\n", Irq);
#endif

	cdr.Irq = 0xff;
	cdr.Ctrl&=~0x80;

	switch (Irq) {
    	case CdlSync:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge; 
			break;

    	case CdlNop:
			SetResultSize(1);
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			i = cdrstat.Status;
        	if (CDR_getStatus(&cdrstat) != -1) {
				if (cdrstat.Type == 0xff) cdr.Stat = DiskError;
				if (cdrstat.Status & 0x10) {
					cdr.Stat = DiskError;
					cdr.Result[0]|= 0x11;
					cdr.Result[0]&=~0x02;
				}
				else if (i & 0x10) {
					cdr.StatP |= 0x2;
					cdr.Result[0]|= 0x2;
					CheckCdrom();
				}
			}
			break;
			
		case CdlSetloc:
			cdr.CmdProcess = 0;
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			break;

		case CdlPlay:
			cdr.CmdProcess = 0;
			SetResultSize(1);
			cdr.StatP|= 0x2;
			cdr.Result[0] = cdr.StatP;
			cdr.Stat = Acknowledge;
			cdr.StatP|= 0x80;
//			if ((cdr.Mode & 0x5) == 0x5) AddIrqQueue(REPPLAY, cdReadTime);
			break;

    	case CdlForward:
			cdr.CmdProcess = 0;
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

    	case CdlBackward:
			cdr.CmdProcess = 0;
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

    	case CdlStandby:
			cdr.CmdProcess = 0;
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

		case CdlStop:
			cdr.CmdProcess = 0;
			SetResultSize(1);
        	cdr.StatP&=~0x2;
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
//        	cdr.Stat = Acknowledge;
			break;

		case CdlPause:
			SetResultSize(1);
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			AddIrqQueue(CdlPause + 0x20, 0x800);
			cdr.Ctrl|= 0x80;
			break;

		case CdlPause + 0x20:
			SetResultSize(1);
        	cdr.StatP&=~0x20;
			cdr.StatP|= 0x2;
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

    	case CdlInit:
			SetResultSize(1);
        	cdr.StatP = 0x2;
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
//			if (!cdr.Init) {
				AddIrqQueue(CdlInit + 0x20, 0x800);
//			}
        	break;

		case CdlInit + 0x20:
			SetResultSize(1);
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			cdr.Init = 1;
			break;

    	case CdlMute:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			break;

    	case CdlDemute:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			break;

    	case CdlSetfilter:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge; 
        	break;

		case CdlSetmode:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
        	break;

    	case CdlGetmode:
			SetResultSize(6);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Result[1] = cdr.Mode;
        	cdr.Result[2] = cdr.File;
        	cdr.Result[3] = cdr.Channel;
        	cdr.Result[4] = 0;
        	cdr.Result[5] = 0;
        	cdr.Stat = Acknowledge;
        	break;

    	case CdlGetlocL:
			SetResultSize(8);
//        	for (i=0; i<8; i++) cdr.Result[i] = itob(cdr.Transfer[i]);
        	for (i=0; i<8; i++) cdr.Result[i] = cdr.Transfer[i];
        	cdr.Stat = Acknowledge;
        	break;

    	case CdlGetlocP:
			SetResultSize(8);
#if 0
			subq = (struct SubQ*) CDR_getBufferSub();
			if (subq != NULL) {
				cdr.Result[0] = subq->TrackNumber;
				cdr.Result[1] = subq->IndexNumber;
		    	memcpy(cdr.Result+2, subq->TrackRelativeAddress, 3);
		    	memcpy(cdr.Result+5, subq->AbsoluteAddress, 3);
			} 
			else
#endif
			{
	        	cdr.Result[0] = 1;
	        	cdr.Result[1] = 1;
	        	cdr.Result[2] = cdr.Prev[0];
	        	cdr.Result[3] = itob((btoi(cdr.Prev[1])) - 2);
	        	cdr.Result[4] = cdr.Prev[2];
		    	memcpy(cdr.Result+5, cdr.Prev, 3);
			}
        	cdr.Stat = Acknowledge;
        	break;

    	case CdlGetTN:
			cdr.CmdProcess = 0;
			SetResultSize(3);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	if (CDR_getTN(cdr.ResultTN) == -1) {
				cdr.Stat = DiskError;
				cdr.Result[0]|= 0x01;
        	} else {
        		cdr.Stat = Acknowledge;
        	    cdr.Result[1] = itob(cdr.ResultTN[0]);
        	    cdr.Result[2] = itob(cdr.ResultTN[1]);
        	}
        	break;

    	case CdlGetTD:
			cdr.CmdProcess = 0;
        	cdr.Track = btoi(cdr.Param[0]);
			SetResultSize(4);
			cdr.StatP|= 0x2;
        	if (CDR_getTD(cdr.Track, cdr.ResultTD) == -1) {
				cdr.Stat = DiskError;
				cdr.Result[0]|= 0x01;
        	} else {
        		cdr.Stat = Acknowledge;
				cdr.Result[0] = cdr.StatP;
	    		cdr.Result[1] = itob(cdr.ResultTD[2]);
        	    cdr.Result[2] = itob(cdr.ResultTD[1]);
				cdr.Result[3] = itob(cdr.ResultTD[0]);
	    	}
			break;

    	case CdlSeekL:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
			cdr.StatP|= 0x40;
        	cdr.Stat = Acknowledge;
			cdr.Seeked = 1;
			AddIrqQueue(CdlSeekL + 0x20, 0x800);
			break;

    	case CdlSeekL + 0x20:
			SetResultSize(1);
			cdr.StatP|= 0x2;
			cdr.StatP&=~0x40;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

    	case CdlSeekP:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
			cdr.StatP|= 0x40;
        	cdr.Stat = Acknowledge;
			AddIrqQueue(CdlSeekP + 0x20, 0x800);
			break;

    	case CdlSeekP + 0x20:
			SetResultSize(1);
			cdr.StatP|= 0x2;
			cdr.StatP&=~0x40;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;
#if 0
		case CdlTest:
        	cdr.Stat = Acknowledge;
        	switch (cdr.Param[0]) {
        	    case 0x20: // System Controller ROM Version
					SetResultSize(4);
					memcpy(cdr.Result, Test20, 4);
					break;
				case 0x22:
					SetResultSize(8);
					memcpy(cdr.Result, Test22, 4);
					break;
				case 0x23: case 0x24:
					SetResultSize(8);
					memcpy(cdr.Result, Test23, 4);
					break;
        	}
			break;

    	case CdlID:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			AddIrqQueue(CdlID + 0x20, 0x800);
			break;

		case CdlID + 0x20:
			SetResultSize(8);
        	if (CDR_getStatus(&cdrstat) == -1) {
        		cdr.Result[0] = 0x00; // 0x08 and cdr.Result[1]|0x10 : audio cd, enters cd player
                cdr.Result[1] = 0x00; // 0x80 leads to the menu in the bios, else loads CD
        	}
        	else {
                if (cdrstat.Type == 2) {
                	cdr.Result[0] = 0x08;
                    cdr.Result[1] = 0x10;
	        	}
	        	else {
                    cdr.Result[0] = 0x00;
                    cdr.Result[1] = 0x00;
	        	}
        	}
        	if (!LoadCdBios) cdr.Result[1] |= 0x80;

        	cdr.Result[2] = 0x00;
        	cdr.Result[3] = 0x00;
			strncpy((char *)&cdr.Result[4], "PCSX", 4);
			cdr.Stat = Complete;
			break;
#else
    case CdlTest:
        cdr.Stat = Acknowledge;
        switch (cdr.Param[0]) {
        case 0x20:
        		SetResultSize(4);
            cdr.Result[0] = 0x94;    // BIOS Timestamps
            cdr.Result[1] = 0x09;
            cdr.Result[2] = 0x19;
            cdr.Result[3] = 0xC0;
            break;
        case 0x22:
        		SetResultSize(8);
            memcpy(cdr.Result,US_Cmd25P22,8);
            break;
        case 0x23:
        		SetResultSize(8);
            memcpy(cdr.Result,US_Cmd25P23,8);
            break;
        case 0x24:
        		SetResultSize(8);
            memcpy(cdr.Result,US_Cmd25P24,8);
            break;
        case 0x25:
        		SetResultSize(8);
            memcpy(cdr.Result,US_Cmd25P25,8);
            break;
        }
        break;

    case CdlID:
        cdr.Result[0] = 0x08;  // 0x10=Audio CD    0x08=Data Tracks
                              // 0x00=Don't Known
        cdr.Result[1] = 0x00;  // 0x80=Copied CD   else Original
        cdr.Result[2] = 0x00;
        cdr.Result[3] = 0;
        *(u32 *)(cdr.Result + 4) = US_SCExID;
				SetResultSize(8);
        cdr.Stat = Complete;
        break;
#endif
		case CdlReset:
			SetResultSize(1);
        	cdr.StatP = 0x2;
			cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			break;

    	case CdlReadToc:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Acknowledge;
			AddIrqQueue(CdlReadToc + 0x20, 0x800);
			break;

    	case CdlReadToc + 0x20:
			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
        	cdr.Stat = Complete;
			break;

		case AUTOPAUSE:
			cdr.OCUP = 0;
/*			SetResultSize(1);
			StopCdda();
			StopReading();
			cdr.OCUP = 0;
        	cdr.StatP&=~0x20;
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
    		cdr.Stat = DataEnd;
*/			AddIrqQueue(CdlPause, 0x400);
			break;

		case READ_ACK:
			if (!cdr.Reading) return;

			SetResultSize(1);
			cdr.StatP|= 0x2;
        	cdr.Result[0] = cdr.StatP;
			if (cdr.Seeked == 0) {
				cdr.Seeked = 1;
				cdr.StatP|= 0x40;
			}
			cdr.StatP|= 0x20;
        	cdr.Stat = Acknowledge;

			ReadTrack();

			CDREAD_INT((cdr.Mode & 0x80) ? (cdReadTime / 2) : cdReadTime);
//			CDREAD_INT(0x40000);
			break;

		case REPPLAY_ACK:
			cdr.Stat = Acknowledge;
			cdr.Result[0] = cdr.StatP;
			SetResultSize(1);
			AddIrqQueue(REPPLAY, cdReadTime);
			break;

		case REPPLAY: 
			if ((cdr.Mode & 5) != 5) break;
/*			if (CDR_getStatus(&stat) == -1) {
				cdr.Result[0] = 0;
				cdr.Result[1] = 0;
				cdr.Result[2] = 0;
				cdr.Result[3] = 0;
				cdr.Result[4] = 0;
				cdr.Result[5] = 0;
				cdr.Result[6] = 0;
				cdr.Result[7] = 0;
			} else memcpy(cdr.Result, &stat.Track, 8);
			cdr.Stat = 1;
			SetResultSize(8);
			AddIrqQueue(REPPLAY_ACK, cdReadTime);
*/			break;

		case 0xff:
			return;

		default:
			cdr.Stat = Complete;
			break;
	}

	if (cdr.Stat != NoIntr && cdr.Reg2 != 0x18) {
		psxHu32(0x1070)|= SWAP32((u32)0x4);
		psxRegs->interrupt|= 0x80000000;
	}

#ifdef CDR_LOG
	CDR_LOG("Cdr Interrupt %x\n", Irq);
#endif
}

void cdrReadInterrupt() {
	u8 *buf;

	if (!cdr.Reading) return;

	if (cdr.Stat) {
		CDREAD_INT((cdr.Mode & 0x80) ? (cdReadTime / 2) : cdReadTime);
		return;
	}

#ifdef CDR_LOG
	CDR_LOG("KEY END");
#endif

    cdr.OCUP = 1;
	SetResultSize(1);
	cdr.StatP|= 0x22;
	cdr.StatP&=~0x40;
    cdr.Result[0] = cdr.StatP;

	buf = CDR_getBuffer();
#if 0
	if (buf == NULL) cdr.RErr = -1;

	if (cdr.RErr == -1) {
#ifdef CDR_LOG
		fprintf(emuLog, " err\n");
#endif
		memset(cdr.Transfer, 0, 2340);
		cdr.Stat = DiskError;
		cdr.Result[0]|= 0x01;
		ReadTrack();
		CDREAD_INT((cdr.Mode & 0x80) ? (cdReadTime / 2) : cdReadTime);
		return;
	}
#endif
	memcpy(cdr.Transfer, buf, 2340);
    cdr.Stat = DataReady;

#ifdef CDR_LOG
	fprintf(emuLog, " %x:%x:%x\n", cdr.Transfer[0], cdr.Transfer[1], cdr.Transfer[2]);
#endif

#ifndef NOSOUND
	if ((iSoundMuted == 0) && (cdr.Muted == 1) && (cdr.Mode & 0x40) && (!Config.Xa) && (cdr.FirstSector != -1)) { // CD-XA
		if ((cdr.Transfer[4+2] & 0x4) &&
			((cdr.Mode&0x8) ? (cdr.Transfer[4+1] == cdr.Channel) : 1) &&
			(cdr.Transfer[4+0] == cdr.File)) {
			int ret = xa_decode_sector(&cdr.Xa, cdr.Transfer+4, cdr.FirstSector);

			if (!ret) {
				SPU_playADPCMchannel(&cdr.Xa);
				cdr.FirstSector = 0;
			}
			else cdr.FirstSector = -1;
		}
	}
#endif

	cdr.SetSector[2]++;
    if (cdr.SetSector[2] == 75) {
        cdr.SetSector[2] = 0;
        cdr.SetSector[1]++;
        if (cdr.SetSector[1] == 60) {
            cdr.SetSector[1] = 0;
            cdr.SetSector[0]++;
        }
    }

    cdr.Readed = 0;

	if ((cdr.Transfer[4+2] & 0x80) && (cdr.Mode & 0x2)) { // EOF
#ifdef CDR_LOG
		CDR_LOG("AutoPausing Read\n");
#endif
//		AddIrqQueue(AUTOPAUSE, 0x800);
		AddIrqQueue(CdlPause, 0x800);
	}
	else {
		ReadTrack();
		CDREAD_INT((cdr.Mode & 0x80) ? (cdReadTime / 2) : cdReadTime);
	}
	psxHu32ref(0x1070)|= SWAP32((u32)0x4);
	psxRegs->interrupt|= 0x80000000;
}

/*
cdrRead0:
	bit 0 - 0 REG1 command send / 1 REG1 data read
	bit 1 - 0 data transfer finish / 1 data transfer ready/in progress
	bit 2 - unknown
	bit 3 - unknown
	bit 4 - unknown
	bit 5 - 1 result ready
	bit 6 - 1 dma ready
	bit 7 - 1 command being processed
*/

unsigned char cdrRead0(void) {
	if (cdr.ResultReady) cdr.Ctrl|= 0x20;
	else cdr.Ctrl&=~0x20;

    if (cdr.OCUP) cdr.Ctrl|= 0x40;
//	else cdr.Ctrl&=~0x40;

    // what means the 0x10 and the 0x08 bits? i only saw it used by the bios
    cdr.Ctrl|=0x18;

#ifdef CDR_LOG
	CDR_LOG("CD0 Read: %x\n", cdr.Ctrl);
#endif
	return psxHu8(0x1800) = cdr.Ctrl;
}

/*
cdrWrite0:
	0 - to send a command / 1 - to get the result
*/

void cdrWrite0(unsigned char rt) {
#ifdef CDR_LOG
	CDR_LOG("CD0 write: %x\n", rt);
#endif
	cdr.Ctrl = rt | (cdr.Ctrl & ~0x3);

    if (rt == 0) {
		cdr.ParamP = 0;
		cdr.ParamC = 0;
		cdr.ResultReady = 0;
	}
}

unsigned char cdrRead1(void) {
#ifdef CDR_LOG
	CDR_LOG("CD1 read START\n");
#endif

    if (cdr.ResultReady) { // && cdr.Ctrl & 0x1) {
		psxHu8(0x1801) = cdr.Result[cdr.ResultP++];
		if (cdr.ResultP == cdr.ResultC) cdr.ResultReady = 0;
	} else psxHu8(0x1801) = 0;
#ifdef CDR_LOG
	CDR_LOG("CD1 Read: %x\n", psxHu8(0x1801));
#endif
	return psxHu8(0x1801);
}

void cdrWrite1(unsigned char rt) {
	int i;

#ifdef CDR_LOG
	CDR_LOG("CD1 write: %u\n", rt);
#endif
//	psxHu8(0x1801) = rt;
    cdr.Cmd = rt;
	cdr.OCUP = 0;

#ifdef CDRCMD_DEBUG
	SysPrintf("CD1 write: %x (%s)", rt, CmdName[rt]);
	if (cdr.ParamC) {
		SysPrintf(" Param[%d] = {", cdr.ParamC);
		for (i=0;i<cdr.ParamC;i++) SysPrintf(" %x,", cdr.Param[i]);
		SysPrintf("}\n");
	} else SysPrintf("\n");
#endif

	if (cdr.Ctrl & 0x1) return;

    switch(cdr.Cmd) {
    	case CdlSync:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlNop:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlSetloc:
			StopReading();
			cdr.Seeked = 0;
        	for (i=0; i<3; i++) cdr.SetSector[i] = btoi(cdr.Param[i]);
        	cdr.SetSector[3] = 0;
/*        	if ((cdr.SetSector[0] | cdr.SetSector[1] | cdr.SetSector[2]) == 0) {
				*(u32 *)cdr.SetSector = *(u32 *)cdr.SetSectorSeek;
			}*/
			cdr.Ctrl|= 0x80;
        	cdr.Stat = NoIntr;
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlPlay:
        	if (!cdr.SetSector[0] & !cdr.SetSector[1] & !cdr.SetSector[2]) {
            	if (CDR_getTN(cdr.ResultTN) != -1) {
	                if (cdr.CurTrack > cdr.ResultTN[1]) cdr.CurTrack = cdr.ResultTN[1];
                    if (CDR_getTD((unsigned char)(cdr.CurTrack), cdr.ResultTD) != -1) {
		               	int tmp = cdr.ResultTD[2];
                        cdr.ResultTD[2] = cdr.ResultTD[0];
						cdr.ResultTD[0] = tmp;
	                    if (!Config.Cdda) CDR_play(cdr.ResultTD);
					}
                }
			}
    		else if (!Config.Cdda) CDR_play(cdr.SetSector);
    		cdr.Play = 1;
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
    		break;

    	case CdlForward:
        	if (cdr.CurTrack < 0xaa) cdr.CurTrack++;
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlBackward:
        	if (cdr.CurTrack > 1) cdr.CurTrack--;
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlReadN:
			cdr.Irq = 0;
			StopReading();
			cdr.Ctrl|= 0x80;
        	cdr.Stat = NoIntr; 
			StartReading(1);
        	break;

    	case CdlStandby:
			StopCdda();
			StopReading();
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlStop:
			StopCdda();
			StopReading();
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlPause:
			StopCdda();
			StopReading();
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr;
    		AddIrqQueue(cdr.Cmd, 0x40000);
        	break;

		case CdlReset:
    	case CdlInit:
			StopCdda();
			StopReading();
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlMute:
        	cdr.Muted = 0;
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlDemute:
        	cdr.Muted = 1;
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlSetfilter:
        	cdr.File = cdr.Param[0];
        	cdr.Channel = cdr.Param[1];
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlSetmode:
#ifdef CDR_LOG
			CDR_LOG("Setmode %x\n", cdr.Param[0]);
#endif 
        	cdr.Mode = cdr.Param[0];
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlGetmode:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlGetlocL:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlGetlocP:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
			AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlGetTN:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlGetTD:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlSeekL:
//			((u32 *)cdr.SetSectorSeek)[0] = ((u32 *)cdr.SetSector)[0];
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlSeekP:
//        	((u32 *)cdr.SetSectorSeek)[0] = ((u32 *)cdr.SetSector)[0];
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlTest:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlID:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	case CdlReadS:
			cdr.Irq = 0;
			StopReading();
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
			StartReading(2);
        	break;

    	case CdlReadToc:
			cdr.Ctrl|= 0x80;
    		cdr.Stat = NoIntr; 
    		AddIrqQueue(cdr.Cmd, 0x800);
        	break;

    	default:
#ifdef CDR_LOG
			CDR_LOG("Unknown Cmd: %x\n", cdr.Cmd);
#endif
			return;
    }
	if (cdr.Stat != NoIntr) {
		psxHu32ref(0x1070)|= SWAP32((u32)0x4);
		psxRegs->interrupt|= 0x80000000;
	}
}

unsigned char cdrRead2(void) {
	unsigned char ret;
#ifdef CDR_LOG
	CDR_LOG("CD2 Read START\n");
#endif

	if (cdr.Readed == 0) {
		ret = 0;
	} else {
		ret = *cdr.pTransfer++;
	}

#ifdef CDR_LOG
	CDR_LOG("CD2 Read: %x\n", ret);
#endif
	return ret;
}

void cdrWrite2(unsigned char rt) {
#ifdef CDR_LOG
	CDR_LOG("CD2 write: %x\n", rt);
#endif
    if (cdr.Ctrl & 0x1) {
		switch (rt) {
			case 0x07:
	    		cdr.ParamP = 0;
				cdr.ParamC = 0;
				cdr.ResultReady = 1; //0;
				cdr.Ctrl&= ~3; //cdr.Ctrl = 0;
				break;

			default:
				cdr.Reg2 = rt;
				break;
		}
    } else if (!(cdr.Ctrl & 0x1) && cdr.ParamP < 8) {
		cdr.Param[cdr.ParamP++] = rt;
		cdr.ParamC++;
	}
}

unsigned char cdrRead3(void) {
#ifdef CDR_LOG
	CDR_LOG("CD3 Read Read START\n");
#endif

	if (cdr.Stat) {
		if (cdr.Ctrl & 0x1) psxHu8(0x1803) = cdr.Stat | 0xE0;
		else psxHu8(0x1803) = 0xff;
	} else psxHu8(0x1803) = 0;
#ifdef CDR_LOG
	CDR_LOG("CD3 Read: %x\n", psxHu8(0x1803));
#endif
	return psxHu8(0x1803);
}

void cdrWrite3(unsigned char rt) {
#ifdef CDR_LOG
	CDR_LOG("CD3 write: %x\n", rt);
#endif
    if (rt == 0x07 && cdr.Ctrl & 0x1) {
		cdr.Stat = 0;

		if (cdr.Irq == 0xff) { cdr.Irq = 0; return; }
        if (cdr.Irq) CDR_INT(cdr.eCycle);
        if (cdr.Reading && !cdr.ResultReady) 
            CDREAD_INT((cdr.Mode & 0x80) ? (cdReadTime / 2) : cdReadTime);

		return;
	}
	if (rt == 0x80 && !(cdr.Ctrl & 0x1) && cdr.Readed == 0) {
		cdr.Readed = 1;
		cdr.pTransfer = cdr.Transfer;

		switch (cdr.Mode&0x30) {
			case 0x10:
			case 0x00: cdr.pTransfer+=12; break;
			default: break;
		}
	}
}

void psxDma3(u32 madr, u32 bcr, u32 chcr) {
	u32 cdsize;
	u8 *ptr;

#ifdef CDR_LOG
	CDR_LOG("*** DMA 3 *** %lx addr = %lx size = %lx\n", chcr, madr, bcr);
#endif

	switch (chcr) {
		case 0x11000000:
		case 0x11400100:
			if (cdr.Readed == 0) {
#ifdef CDR_LOG
				CDR_LOG("*** DMA 3 *** NOT READY\n");
#endif
				break;
			}

			cdsize = (bcr & 0xffff) * 4;

			ptr = (u8*)PSXM(madr);
			if (ptr == NULL) {
#ifdef CPU_LOG
				CDR_LOG("*** DMA 3 *** NULL Pointer!!!\n");
#endif
				break;
			}
			memcpy(ptr, cdr.pTransfer, cdsize);
			psxCpu->Clear(madr, cdsize/4);
			cdr.pTransfer+= cdsize;

			break;
		default:
#ifdef CDR_LOG
			CDR_LOG("Unknown cddma %lx\n", chcr);
#endif
			break;
	}

	HW_DMA3_CHCR &= ~0x01000000;
	DMA_INTERRUPT(3);
}

void cdrReset() {
	memset(&cdr, 0, sizeof(cdr));
	cdr.CurTrack=1;
	cdr.File=1; cdr.Channel=1;
}

s32 cdrFreeze(gzFile f, s32 Mode) {
	int tmp;

	gzfreeze(&cdr, sizeof(cdr));

	if (Mode == 1) tmp = cdr.pTransfer - cdr.Transfer;
	gzfreezel(&tmp);
	if (Mode == 0) cdr.pTransfer = cdr.Transfer + tmp;

	return 0;
}

