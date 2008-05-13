#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#include "gui.h"
#include "spu.h"

_langs *langs;
unsigned int langsMax;
char cfgfile[256];

s32 InitComponents(void)
{
	s32 ret;
#if defined(PSP) && defined(PSP_GPU)
  GPU_SelectPluggin(2);
#elif defined(IPHONE)
  GPU_SelectPluggin(0); // 3
#else
  GPU_SelectPluggin(0);
#endif

	ret = CDR_init();
	if (ret < 0) { SysPrintf("CDRinit error : %d", ret); return -1; }
	ret = GPU_init();
	if (ret < 0) { SysPrintf("GPUinit error: %d", ret); return -1; }
	ret = SPU_init();
	if (ret < 0) { SysPrintf("SPUinit error: %d", ret); return -1; }
	ret = PAD1_init(1);
	if (ret < 0) { SysPrintf("PAD1init error: %d", ret); return -1; }
	ret = PAD2_init(2);
	if (ret < 0) { SysPrintf("PAD2init error: %d", ret); return -1; }

	ret = CDR_open();
	if (ret < 0) { SysPrintf("Error Opening CDR Component"); return -1; }
	ret = SPU_open();
	if (ret < 0) { SysPrintf("Error Opening SPU Component"); return -1; }
	// EDIT SPU_registerCallback(SPUirq);
  //ret = GPU_open(0);
	//if (ret < 0) { SysPrintf("Error Opening GPU Component"); return -1; }
	ret = PAD1_open();
	if (ret < 0) { SysPrintf("Error Opening PAD1 Component"); return -1; }
	ret = PAD2_open();
	if (ret < 0) { SysPrintf("Error Opening PAD2 Component"); return -1; }

	return 0;
}

void CloseComponents() {
	s32 ret;

	ret = CDR_close();
	if (ret < 0) { SysMessage("Error Closing CDR Plugin"); }
	ret = SPU_close();
	if (ret < 0) { SysMessage("Error Closing SPU Plugin"); }
	ret = PAD1_close();
	if (ret < 0) { SysMessage("Error Closing PAD1 Plugin"); }
	ret = PAD2_close();
	if (ret < 0) { SysMessage("Error Closing PAD2 Plugin"); }
	//ret = GPU_close();
	//if (ret < 0) { SysMessage("Error Closing GPU Plugin"); }

	CDR_shutdown();
	GPU_done();
//GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();
}

static u8 buf[256];
u8 stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
u8 mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
u8 analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static s32 bufcount, bufc;

PadDataS padd1, padd2;

#define PSE_PAD_TYPE_MOUSE 1
#define PSE_PAD_TYPE_NEGCON 2
#define PSE_PAD_TYPE_ANALOGPAD 7
#define PSE_PAD_TYPE_ANALOGJOY 5
#define PSE_PAD_TYPE_STANDARD 4

u8 _PADstartPoll(PadDataS *pad) {
	bufc = 0;

	switch (pad->controllerType) {
		case PSE_PAD_TYPE_MOUSE:
			mousepar[3] = pad->buttonStatus & 0xff;
			mousepar[4] = pad->buttonStatus >> 8;
			mousepar[5] = pad->moveX;
			mousepar[6] = pad->moveY;

			memcpy(buf, mousepar, 7);
			bufcount = 6;
			break;
		case PSE_PAD_TYPE_NEGCON: // npc101/npc104(slph00001/slph00069)
			analogpar[1] = 0x23;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGPAD: // scph1150
			analogpar[1] = 0x73;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGJOY: // scph1110
			analogpar[1] = 0x53;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_STANDARD:
		default:
			stdpar[3] = pad->buttonStatus & 0xff;
			stdpar[4] = pad->buttonStatus >> 8;

			memcpy(buf, stdpar, 5);
			bufcount = 4;
	}

	return buf[bufc++];
}

u8 _PADpoll(u8 value) {
	if (bufc > bufcount) return 0xFF;
	return buf[bufc++];
}

u8 PAD1_startPoll(s32 pad) {
	PadDataS padd;

	PAD1_readPort1(&padd);

	return _PADstartPoll(&padd);
}

u8 PAD1_poll(u8 value) {
	return _PADpoll(value);
}

u8 PAD2_startPoll(s32 pad) {
	PadDataS padd;

	PAD2_readPort2(&padd);

	return _PADstartPoll(&padd);
}

u8 PAD2_poll(u8 value) {
	return 0xFF; //_PADpoll(value);
}
