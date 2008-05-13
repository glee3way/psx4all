#include <stdlib.h> //malloc/free
#include <string.h>

#include "common.h"

#include "decode_xa.h"

int iSoundMuted = 1;

static int spu_sbaddr;
static short spureg[(0x1e00-0x1c00)/2];
static short *spumem;

long SPU_init(void) {
    spumem = (short *)malloc(512*1024);
    if (spumem == NULL) return -1;

    return 0;
}

long SPU_shutdown(void) {
    if (spumem != NULL) {
	free(spumem);
	spumem = NULL;
    }

    return 0;
}

long SPU_open(void) {
    return 0;
}

long SPU_close(void) {
    return 0;
}

// New Interface

void SPU_writeRegister(unsigned long reg, unsigned short val) {
    spureg[(reg-0x1f801c00)/2] = val;
    switch(reg) {
	case 0x1f801da6: // spu sbaddr
    	    spu_sbaddr = val * 8;
    	    break;
	case 0x1f801da8: // spu data
	    spumem[spu_sbaddr/2] = (short)val;
	    spu_sbaddr+=2;
	    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
    	    break;
    }
}

unsigned short SPU_readRegister(unsigned long reg) {
    switch (reg){
	case 0x1f801da6: // spu sbaddr
    	    return spu_sbaddr / 8;
	case 0x1f801da8: // spu data
	    {
	    int ret = spumem[spu_sbaddr/2];
	    spu_sbaddr+=2;
	    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
	    return ret;
	    }
	default:
	    return spureg[(reg-0x1f801c00)/2];
    }
    return 0;
}

void SPU_readDMAMem(unsigned short * ptr, int size) {
    for(int i = 0; i < size; i++)
	{
		ptr[i] = spumem[spu_sbaddr/2];
		spu_sbaddr+=2;
		if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
	}
}

void SPU_writeDMAMem(unsigned short *ptr, int size) {
    for(int i = 0; i < size; i++)
	{
		spumem[spu_sbaddr/2] = (short)ptr[i];
		spu_sbaddr+=2;
		if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
	}
}

void SPU_playADPCMchannel(xa_decode_t *xap) {
}
// Old Interface

unsigned short SPU_getOne(unsigned long val) {
    if (val > 0x7ffff) return 0;
    return spumem[val/2];
}

void SPU_putOne(unsigned long val, unsigned short data) {
    if (val > 0x7ffff) return;
    spumem[val/2] = data;
}

void SPU_setAddr(unsigned char ch, unsigned short waddr) {
}

void SPU_setPitch(unsigned char ch, unsigned short pitch) {
}

void SPU_setVolumeL(unsigned char ch, short vol) {
}

void SPU_setVolumeR(unsigned char ch, short vol) {
}

void SPU_startChannels1(unsigned short channels) {
}

void SPU_startChannels2(unsigned short channels) {
}

void SPU_stopChannels1(unsigned short channels) {
}

void SPU_stopChannels2(unsigned short channels) {
}


long SPU_test(void) {
    return 0;
}

long SPU_configure(void) {
    return 0;
}

void SPU_about(void) {
}

long SPU_freeze(unsigned long ulFreezeMode,SPUFreeze_t * pF)
{
	if( ulFreezeMode == 1 )
	{
		memcpy(pF->cSPURam, spumem, 512*1024);
		memcpy(pF->cSPUPort, spureg, 0x200);
		pF->Addr = spu_sbaddr;
	}
	else
	{
		memcpy(spumem, pF->cSPURam, 512*1024);
		memcpy(spureg, pF->cSPUPort, 0x200);
		spu_sbaddr = pF->Addr;
	}
	return 1;
}

void SPU_async(u32 length)
{
}

extern "C" void sound_callback(void *userdata, u8 *stream, int len)
{
}
