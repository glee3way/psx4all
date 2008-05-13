
#include "stdio.h"
#include "minimal.h"
#include "cpuctrl.h"

#define RAMTWEAKSVALUES "--active 1 --trc 6 --tras 4 --twr 1 --tmrd 1 --trfc 1 --trp 2 --trcd 2"
#define RAMTWEAKSTRUCTURE "--active %d --trc %d --tras %d --twr %d --tmrd %d --trfc %d --trp %d --trcd %d"
void set_ram_tweaks(void)
{
	int args[8]={0,-1,-1,-1,-1,-1,-1,-1};
	sscanf(RAMTWEAKSVALUES,RAMTWEAKSTRUCTURE,&args[0],&args[1],&args[2],&args[3],&args[4],&args[5],&args[6],&args[7]);
	if (args[0]) {
		printf("RAM Tweaks Activated\r\n");
		if((args[1]!=-1) && (args[1]-1 < 16)) set_tRC(args[1]-1);
		if((args[2]!=-1) && (args[2]-1 < 16)) set_tRAS(args[2]-1);
		if((args[3]!=-1) && (args[3]-1 < 16)) set_tWR(args[3]-1);
		if((args[4]!=-1) && (args[4]-1 < 16)) set_tMRD(args[4]-1);
		if((args[5]!=-1) && (args[5]-1 < 16)) set_tRFC(args[5]-1);
		if((args[6]!=-1) && (args[6] < 16)) set_tRP(args[6]-1);
		if((args[7]!=-1) && (args[7]-1 < 16)) set_tRCD(args[7]-1);
	} else {
		printf("RAM Tweaks Disabled\r\n");
	}
}
