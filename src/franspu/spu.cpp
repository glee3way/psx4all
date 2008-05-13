#include "spu.h"
#include <minimal.h>


extern void SoundFeedStreamData(unsigned char* pSound,long lBytes);
extern void InitADSR(void);
extern void SetupSound(void);
extern void RemoveSound(void);

// psx buffer / addresses
unsigned short  regArea[10000];                        
unsigned short  spuMem[256*1024];
unsigned char * spuMemC;
unsigned char * pSpuBuffer;
unsigned char * pMixIrq=0;

// user settings          
#ifdef NOSOUND
int             iUseXA=0;
#else
int             iUseXA=1;
#endif
int		iSoundMuted=0;
                            
// infos struct for each channel
SPUCHAN         s_chan[MAXCHAN+1];                     // channel + 1 infos (1 is security for fmod handling)
REVERBInfo      rvb;

unsigned long   dwNoiseVal=1;                          // global noise generator
unsigned short  spuCtrl=0;                             // some vars to store psx reg infos
unsigned short  spuStat=0;
unsigned short  spuIrq=0;             
unsigned long   spuAddr=0xffffffff;                    // address into spu mem
int             bEndThread=0;                          // thread handlers
int             bSpuInit=0;
int             bSPUIsOpen=0;

int SSumR[NSSIZE];
int SSumL[NSSIZE];
int iFMod[NSSIZE];
short * pS;

// dirty inline func includes
#include "spu_adsr.h"
#include "spu_xa.h"

// START SOUND... called by main thread to setup a new sound on a channel
INLINE void StartSound(SPUCHAN * pChannel)
{
 	pChannel->ADSRX.lVolume=1;                            // Start ADSR
 	pChannel->ADSRX.State=0;
 	pChannel->ADSRX.EnvelopeVol=0;
 	pChannel->pCurr=pChannel->pStart;                     // set sample start
 	pChannel->s_1=0;                                      // init mixing vars
 	pChannel->s_2=0;
 	pChannel->iSBPos=28;
 	pChannel->bNew=0;                                     // init channel flags
 	pChannel->bStop=0;                                   
 	pChannel->bOn=1;
 	pChannel->SB[29]=0;                                   // init our interpolation helpers
 	pChannel->SB[30]=0;
 	pChannel->spos=0x10000L;
 	pChannel->SB[31]=0;    				// -> no/simple interpolation starts with one 44100 decoding
}

INLINE void VoiceChangeFrequency(SPUCHAN * pChannel)
{
 	pChannel->iUsedFreq=pChannel->iActFreq;               // -> take it and calc steps
 	pChannel->sinc=pChannel->iRawPitch<<4;
 	if(!pChannel->sinc) pChannel->sinc=1;
}

INLINE void FModChangeFrequency(SPUCHAN * pChannel,int ns)
{
 	int NP=pChannel->iRawPitch;
 	NP=((32768L+iFMod[ns])*NP)/32768L;
 	if(NP>0x3fff) NP=0x3fff;
 	if(NP<0x1)    NP=0x1;
 	NP=(44100L*NP)/(4096L);                               // calc frequency
 	pChannel->iActFreq=NP;
 	pChannel->iUsedFreq=NP;
 	pChannel->sinc=(((NP/10)<<16)/4410);
 	if(!pChannel->sinc) pChannel->sinc=1;
 	iFMod[ns]=0;
}                    

// noise handler... just produces some noise data
INLINE int iGetNoiseVal(SPUCHAN * pChannel)
{
 	int fa;
 	if((dwNoiseVal<<=1)&0x80000000L)
  	{
   		dwNoiseVal^=0x0040001L;
   		fa=((dwNoiseVal>>2)&0x7fff);
   		fa=-fa;
  	}
 	else
 		fa=(dwNoiseVal>>2)&0x7fff;
 	// mmm... depending on the noise freq we allow bigger/smaller changes to the previous val
 	fa=pChannel->iOldNoise+((fa-pChannel->iOldNoise)/((0x001f-((spuCtrl&0x3f00)>>9))+1));
 	if(fa>32767L)  fa=32767L;
 	if(fa<-32767L) fa=-32767L;              
 	pChannel->iOldNoise=fa;
 	pChannel->SB[29] = fa;                               // -> store noise val in "current sample" slot
 	return fa;
}                                 

INLINE void StoreInterpolationVal(SPUCHAN * pChannel,int fa)
{
 	if(pChannel->bFMod==2)                                	// fmod freq channel
  		pChannel->SB[29]=fa;
 	else
  	{
   		if((spuCtrl&0x4000)==0)
   			fa=0;                       		// muted?
   		else                                            // else adjust
    		{
     			if(fa>32767L)  fa=32767L;
     			if(fa<-32767L) fa=-32767L;              
    		}
   		pChannel->SB[29]=fa;                           	// no interpolation
  	}
}

// here is the main job handler... direct func call (calculates 1 msec of sound)
INLINE void SPU_async_1ms(SPUCHAN * pChannel,int *SSumL, int *SSumR, int *iFMod)
{
	int ch;			// Channel loop
	int ns; 		// Samples loop
	unsigned int i; 	// Internal loop
 	int s_1,s_2,fa;
 	unsigned char * start;
 	int predict_nr,shift_factor,flags,s;
	const int f[5][2] = {{0,0},{60,0},{115,-52},{98,-55},{122,-60}};

 	memset(SSumL,0,NSSIZE*sizeof(int));
 	memset(SSumR,0,NSSIZE*sizeof(int));

   	// main channel loop
     	for(ch=0;ch<MAXCHAN;ch++,pChannel++)              	// loop em all... we will collect 1 ms of sound of each playing channel
      	{
       		if(pChannel->bNew) StartSound(pChannel);        // start new sound
       		if(!pChannel->bOn) continue;                    // channel not playing? next
       		if(pChannel->iActFreq!=pChannel->iUsedFreq)     // new psx frequency?
        		VoiceChangeFrequency(pChannel);

       		ns=0;
       		while(ns<NSSIZE)                                // loop until 1 ms of data is reached
        	{
         		if(pChannel->bFMod==1 && iFMod[ns])     // fmod freq channel
          			FModChangeFrequency(pChannel,ns);
         		while(pChannel->spos>=0x10000L)
          		{
           			if(pChannel->iSBPos==28)        // 28 reached?
            			{
             				start=pChannel->pCurr;  // set up the current pos
             				if (start == (unsigned char*)-1) // special "stop" sign
              				{
               					pChannel->bOn=0; // -> turn everything off
               					pChannel->ADSRX.lVolume=0;
               					pChannel->ADSRX.EnvelopeVol=0;
               					goto ENDX;       // -> and done for this channel
              				}
             				pChannel->iSBPos=0;
             				s_1=pChannel->s_1;
             				s_2=pChannel->s_2;
             				predict_nr=(int)*start;start++;
             				shift_factor=predict_nr&0xf;
             				predict_nr >>= 4;
             				flags=(int)*start;start++;

             				for (i=0;i<28;start++)      
              				{
               					s=((((int)*start)&0xf)<<12);
               					if(s&0x8000) s|=0xffff0000;
               					fa=(s >> shift_factor);
               					fa=fa + ((s_1 * f[predict_nr][0])>>6) + ((s_2 * f[predict_nr][1])>>6);
               					s_2=s_1;s_1=fa;
               					s=((((int)*start) & 0xf0) << 8);
               					pChannel->SB[i++]=fa;
               					if(s&0x8000) s|=0xffff0000;
               					fa=(s>>shift_factor);              
               					fa=fa + ((s_1 * f[predict_nr][0])>>6) + ((s_2 * f[predict_nr][1])>>6);
               					s_2=s_1;s_1=fa;
               					pChannel->SB[i++]=fa;
              				}     

             				// flag handler
             				if((flags&4) && (!pChannel->bIgnoreLoop))
              					pChannel->pLoop=start-16;                // loop address
             				if(flags&1)                               	// 1: stop/loop
              				{
               					// We play this block out first...
               					if(flags!=3 || pChannel->pLoop==NULL)   // PETE: if we don't check exactly for 3, loop hang ups will happen (DQ4, for example)
                 					start = (unsigned char*)-1;	// and checking if pLoop is set avoids crashes, yeah
               					else
                 					start = pChannel->pLoop;
              				}

             				pChannel->pCurr=start;                    // store values for next cycle
             				pChannel->s_1=s_1;
             				pChannel->s_2=s_2;      
           			}
           			
           			fa=pChannel->SB[pChannel->iSBPos++];        // get sample data
           			StoreInterpolationVal(pChannel,fa);         // store val for later interpolation
           			pChannel->spos -= 0x10000L;
          		}

         		if(pChannel->bNoise)
              			fa=iGetNoiseVal(pChannel);               // get noise val
         		else 
         			fa=pChannel->SB[29];       		// get interpolation val

         		pChannel->sval=(MixADSR(pChannel)*fa)>>10;   // mix adsr

         		if(pChannel->bFMod==2)                        // fmod freq channel
          			iFMod[ns]=pChannel->sval;             // -> store 1T sample data, use that to do fmod on next channel
         		else                                          // no fmod freq channel
          		{                                          
            			SSumL[ns]+=(pChannel->sval*pChannel->iLeftVolume)>>14;
             			SSumR[ns]+=(pChannel->sval*pChannel->iRightVolume)>>14;
          		}

         		// ok, go on until 1 ms data of this channel is collected
         		ns++;                                          
         		pChannel->spos += pChannel->sinc;             
        	}        
ENDX:   ;                                                      
      	}
   
  	// here we have another 1 ms of sound data

  	// Mix XA
  	if(XAPlay!=XAFeed || XARepeat) MixXA();

  	// Mono Mix
	for(i=0;i<NSSIZE;i++)
      		*pS++=((*SSumL++)+(*SSumR++))>>1;
}

void SPU_async(unsigned long cycle)
{
 	if( iSoundMuted > 0 ) return;
	int i;
	int t=(cycle?20:16); /* cycle 0=NTSC 16 ms, 1=PAL 20 ms */
 	for (i=0;i<t;i++)
 		SPU_async_1ms(s_chan,SSumL,SSumR,iFMod); // Calculates 1 ms of sound
        SoundFeedStreamData((unsigned char*)pSpuBuffer,((unsigned char *)pS)-((unsigned char *)pSpuBuffer));
    	pS=(short *)pSpuBuffer;
}


// XA AUDIO
void SPU_playADPCMchannel(xa_decode_t *xap)
{
 	if ((iUseXA)&&(xap)&&(xap->freq))
 		FeedXA(xap); // call main XA feeder
}

// SPUINIT: this func will be called first by the main emu
long SPU_init(void)
{
 	spuMemC=(unsigned char *)spuMem;                      // just small setup
 	memset((void *)s_chan,0,MAXCHAN*sizeof(SPUCHAN));
 	memset((void *)&rvb,0,sizeof(REVERBInfo));
 	InitADSR();
 	return 0;
}

// SPUOPEN: called by main emu after init
s32 SPU_open(void)
{
 	int i;
 	if(bSPUIsOpen) return 0;                              // security for some stupid main emus
#ifdef NOSOUND
 	iUseXA=0;                                             // just small setup
#else
 	iUseXA=1;
#endif
 	spuIrq=0;                       
 	spuAddr=0xffffffff;
 	bEndThread=0;
 	spuMemC=(unsigned char *)spuMem;      
 	pMixIrq=0;
 	memset((void *)s_chan,0,(MAXCHAN+1)*sizeof(SPUCHAN));
 
 	SetupSound();                                         // setup sound (before init!)

	//Setup streams
 	pSpuBuffer=(unsigned char *)malloc(32768);            // alloc mixing buffer
 	XAStart = (unsigned long *)malloc(44100*4);           // alloc xa buffer
 	XAPlay  = XAStart;
 	XAFeed  = XAStart;
 	XAEnd   = XAStart + 44100;
 	for(i=0;i<MAXCHAN;i++)                                // loop sound channels
  	{
   		s_chan[i].ADSRX.SustainLevel = 0xf<<27;       // -> init sustain
   		s_chan[i].pLoop=spuMemC;
   		s_chan[i].pStart=spuMemC;
   		s_chan[i].pCurr=spuMemC;
  	}

 	// Setup timers
 	memset(SSumR,0,NSSIZE*sizeof(int));                   // init some mixing buffers
 	memset(SSumL,0,NSSIZE*sizeof(int));
 	memset(iFMod,0,NSSIZE*sizeof(int));
 	pS=(short *)pSpuBuffer;                               // setup soundbuffer pointer

 	bSPUIsOpen=1;
 	return PSE_SPU_ERR_SUCCESS;        
}

// SPUCLOSE: called before shutdown
long SPU_close(void)
{
 	if(!bSPUIsOpen) return 0;                             // some security
 	bSPUIsOpen=0;                                         // no more open
 	
 	RemoveSound();                                        // no more sound handling

	// Remove streams
 	free(pSpuBuffer);                                     // free mixing buffer
 	pSpuBuffer=NULL;
 	free(XAStart);                                        // free XA buffer
 	XAStart=0;

	return 0;
}

// SPUSHUTDOWN: called by main emu on final exit
long SPU_shutdown(void)
{
 	return 0;
}

// SPUFREEZE: Used for savestates. Dummy.
long SPU_freeze(unsigned long ulFreezeMode,SPUFreeze_t * pF)
{
 	return 1;
}
