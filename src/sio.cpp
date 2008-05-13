#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "common.h"

static unsigned char buf[256];
unsigned char cardh[4] = { 0x00, 0x00, 0x5a, 0x5d };

// Transfer Ready and the Buffer is Empty
unsigned short StatReg = TX_RDY | TX_EMPTY;
unsigned short ModeReg;
unsigned short CtrlReg;
unsigned short BaudReg;

static unsigned long bufcount;
static unsigned long parp;
unsigned long mcdst,rdwr;
static unsigned char adrH,adrL;
static unsigned long padst;
PadDataS pad;
char Mcd1Data[MCD_SIZE], Mcd2Data[MCD_SIZE];

#define SIO_INT(cycles) { \
	if (!Config.Sio) { \
		psxRegs->interrupt|= 0x80; \
		psxRegs->intCycle[7+1] = (u32)((float)cycles * PsxCycleMult); \
		psxRegs->intCycle[7] = psxRegs->cycle; \
	} \
}

unsigned char sioRead8() {
	unsigned char ret = 0;

	if (StatReg & RX_RDY) {
		ret = buf[parp];
		if (parp == bufcount) {
			StatReg &= ~RX_RDY;		// Receive is not Ready now
			if (mcdst == 5) {
				mcdst = 0;
				if (rdwr == 2) {
					switch (CtrlReg&0x2002) {
						case 0x0002:
							memcpy(Mcd1Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd1, Mcd1Data, (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(Mcd2Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd2, Mcd2Data, (adrL | (adrH << 8)) * 128, 128);
							break;
					}
				}
			}
			if (padst == 2) padst = 0;
			if (mcdst == 1) {
				mcdst = 2;
				StatReg|= RX_RDY;
			}
		}
	}
	return ret;
}

void sioWrite8(unsigned char value) {
	switch (padst) {
		case 1: SIO_INT(BIAS_SIO);
			if ((value&0x40) == 0x40) {
				padst = 2; parp = 1;
				switch (CtrlReg&0x2002) {
					case 0x0002:
						buf[parp] = PAD1_poll(value);
						break;
					case 0x2002:
						buf[parp] = PAD2_poll(value);
						break;
				}
				if (!(buf[parp] & 0x0f)) {
					bufcount = 2 + 32;
				} else {
					bufcount = 2 + (buf[parp] & 0x0f) * 2;
				}
				if (buf[parp] == 0x41) {
					switch (value) {
						case 0x43:
							buf[1] = 0x43;
							break;
						case 0x45:
							buf[1] = 0xf3;
							break;
					}
				}
			}
			else padst = 0;
			return;
		case 2:
			parp++;
			switch (CtrlReg&0x2002) {
				case 0x0002: buf[parp] = PAD1_poll(value); break;
				case 0x2002: buf[parp] = PAD2_poll(value); break;
			}

			if (parp == bufcount) { padst = 0; return; }
			SIO_INT(BIAS_SIO);
			return;
	}

	switch (mcdst) {
		case 1:
			SIO_INT(BIAS_SIO);
			if (rdwr) { parp++; return; }
			parp = 1;
			switch (value) {
				case 0x52: rdwr = 1; break;
				case 0x57: rdwr = 2; break;
				default: mcdst = 0;
			}
			return;
		case 2: // address H
			SIO_INT(BIAS_SIO);
			adrH = value;
			*buf = 0;
			parp = 0;
			bufcount = 1;
			mcdst = 3;
			return;
		case 3: // address L
			SIO_INT(BIAS_SIO);
			adrL = value;
			*buf = adrH;
			parp = 0;
			bufcount = 1;
			mcdst = 4;
			return;
		case 4:
			SIO_INT(BIAS_SIO);
			parp = 0;
			switch (rdwr) {
				case 1: // read
					buf[0] = 0x5c;
					buf[1] = 0x5d;
					buf[2] = adrH;
					buf[3] = adrL;
					switch (CtrlReg&0x2002) {
						case 0x0002:
							memcpy(&buf[4], Mcd1Data + (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(&buf[4], Mcd2Data + (adrL | (adrH << 8)) * 128, 128);
							break;
					}
					{
					char x_or = 0;
					int i;
					for (i=2;i<128+4;i++)
						x_or^=buf[i];
					buf[132] = x_or;
					}
					buf[133] = 0x47;
					bufcount = 133;
					break;
				case 2: // write
					buf[0] = adrL;
					buf[1] = value;
					buf[129] = 0x5c;
					buf[130] = 0x5d;
					buf[131] = 0x47;
					bufcount = 131;
					break;
			}
			mcdst = 5;
			return;
		case 5:	
			parp++;
			if (rdwr == 2) {
				if (parp < 128) buf[parp+1] = value;
			}
			SIO_INT(BIAS_SIO);
			return;
	}

	switch (value) {
		case 0x01: // start pad
			StatReg |= RX_RDY;		// Transfer is Ready

			switch (CtrlReg&0x2002) {
				case 0x0002: buf[0] = PAD1_startPoll(1); break;
				case 0x2002: buf[0] = PAD2_startPoll(2); break;
			}

			bufcount = 2;
			parp = 0;
			padst = 1;
			SIO_INT(BIAS_SIO);
			return;
		case 0x81: // start memcard
			StatReg |= RX_RDY;
			memcpy(buf, cardh, 4);
			parp = 0;
			bufcount = 3;
			mcdst = 1;
			rdwr = 0;
			SIO_INT(BIAS_SIO);
			return;
	}
}

void sioWriteCtrl16(unsigned short value) {
	CtrlReg = value & ~RESET_ERR;
	if (value & RESET_ERR) StatReg &= ~IRQ;
	if ((CtrlReg & SIO_RESET) || (!CtrlReg)) {
		padst = 0; mcdst = 0; parp = 0;
		StatReg = TX_RDY | TX_EMPTY;
		psxRegs->interrupt&=~0x80;
	}
}

void sioInterrupt() {
	StatReg|= IRQ;
	psxHu32ref(0x1070)|= SWAPu32(0x80);
	psxRegs->interrupt|= 0x80000000;
}

static void CreateMcd(char *mcd) {

}

static void LoadMcd(int mcd, char *str) {
	FILE *f;
	char *data = NULL;

	if (mcd == 1) data = Mcd1Data;
	if (mcd == 2) data = Mcd2Data;

	if (*str == 0) sprintf(str, "/mnt/sd/mcd00%d.mcr", mcd);
	f = fopen(str, "rb");
	if (f == NULL) {
		SysPrintf("Failed loading MemCard %s\n", str);
	}
	else {
		u32 filesize;
		fseek(f, 0, SEEK_END);
		filesize = ftell(f);
		fseek(f, 0, SEEK_SET);
			if (filesize == MCD_SIZE + 64) 
				fseek(f, 64, SEEK_SET);
			else if(filesize == MCD_SIZE + 3904)
				fseek(f, 3904, SEEK_SET);
		fread(data, 1, MCD_SIZE, f);
		fclose(f);
	}
}

void LoadMcds(char *mcd1, char *mcd2) {
	LoadMcd(1, mcd1);
	LoadMcd(2, mcd2);
}

static void ConvertMcd(char *mcd, char *data) {

}

void SaveMcd(char *mcd, char *data, unsigned long adr, int size) {
	FILE *f;
	f = fopen(mcd, "r+b");
	if (f != NULL) {
		u32 filesize;
		fseek(f, 0, SEEK_END);
		filesize = ftell(f);
		fseek(f, 0, SEEK_SET);
		if (filesize == MCD_SIZE + 64)
			fseek(f, adr + 64, SEEK_SET);
		else if (filesize == MCD_SIZE + 3904)
			fseek(f, adr + 3904, SEEK_SET);
		else
			fseek(f, adr, SEEK_SET);

		fwrite(data + adr, 1, size, f);
#if defined(ARM_ARCH) && !defined(GIZMONDO)
		sync();
		sync();
#endif
		fclose(f);
		return;
	}
}

int sioFreeze(gzFile f, int Mode) {
	char Unused[4096];

	gzfreezel(buf);
	gzfreezel(&StatReg);
	gzfreezel(&ModeReg);
	gzfreezel(&CtrlReg);
	gzfreezel(&BaudReg);
	gzfreezel(&bufcount);
	gzfreezel(&parp);
	gzfreezel(&mcdst);
	gzfreezel(&rdwr);
	gzfreezel(&adrH);
	gzfreezel(&adrL);
	gzfreezel(&padst);
	gzfreezel(Unused);

	return 0;
}
