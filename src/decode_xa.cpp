//============================================
//=== Audio XA decoding
//=== Kazzuya
//============================================
//=== Modified by linuzappz / yokota
//============================================

#include <stdio.h>

#include "common.h"
#include "decode_xa.h"
#ifndef DREAMCAST
#define FIXED
#endif

#ifdef _MSC_VER_
#pragma warning(disable:4244)
#endif

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

typedef struct {
	U8  filenum;
	U8  channum;
	U8  submode;
	U8  coding;
	U8  filenum2;
	U8  channum2;
	U8  submode2;
	U8  coding2;
} xa_subheader_t;

#define CLAMP(_X_,_MI_,_MA_) if(_X_<_MI_)_X_=_MI_; else if(_X_>_MA_)_X_=_MA_;
#define SH	4
#define SHC	10
#define BLKSIZ  28       /* block size (32 - 4 nibbles) */
#define AUDIO_CODING_GET_STEREO(_X_)    ( (_X_) & 3)
#define AUDIO_CODING_GET_FREQ(_X_)      (((_X_) >> 2) & 3)
#define AUDIO_CODING_GET_BPS(_X_)       (((_X_) >> 4) & 3)

#ifndef FIXED
	static double K0[4] = {0.0,0.9375,1.796875,1.53125};
	static double K1[4] = {0.0,0.0,-0.8125,-0.859375};
	#define IK0(fid) ((int)((-K0[fid]) * (1<<SHC)))
	#define IK1(fid) ((int)((-K1[fid]) * (1<<SHC)))
#else
	static int K0[4] = {0.0*(1<<SHC),0.9375*(1<<SHC),1.796875*(1<<SHC),1.53125*(1<<SHC)};
 	static int K1[4] = {0.0*(1<<SHC),0.0*(1<<SHC),-0.8125*(1<<SHC),-0.859375*(1<<SHC)};
	#define IK0(fid) (-K0[fid])
	#define IK1(fid) (-K1[fid])
#endif

static INLINE void ADPCM_DecodeBlock16( ADPCM_Decode_t *decp, const U8 filter_range, const U16 *blockp, short *destp, const int inc )
{
	int i;
	int range, filterid;
	long fy0, fy1;

	filterid = (filter_range >>  4) & 0x0f;
	range    = (filter_range >>  0) & 0x0f;

	fy0 = decp->y0;
	fy1 = decp->y1;

	for (i = BLKSIZ/4; i; --i) {
		long y;
		long x0, x1, x2, x3;

		y = *blockp++;
		x3 = (((int)((short)( y        & 0xf000) >> range))<<SH);
		x2 = (((int)((short)((y <<  4) & 0xf000) >> range))<<SH);
		x1 = (((int)((short)((y <<  8) & 0xf000) >> range))<<SH);
		x0 = (((int)((short)((y << 12) & 0xf000) >> range))<<SH);

		x0 -= (IK0(filterid) * fy0 + (IK1(filterid) * fy1)) >> SHC; fy1 = fy0; fy0 = x0;
		x1 -= (IK0(filterid) * fy0 + (IK1(filterid) * fy1)) >> SHC; fy1 = fy0; fy0 = x1;
		x2 -= (IK0(filterid) * fy0 + (IK1(filterid) * fy1)) >> SHC; fy1 = fy0; fy0 = x2;
		x3 -= (IK0(filterid) * fy0 + (IK1(filterid) * fy1)) >> SHC; fy1 = fy0; fy0 = x3;

		/*CLAMP( x0, -32768<<SH, 32767<<SH );*/ *destp = x0 >> SH; destp += inc;
		/*CLAMP( x1, -32768<<SH, 32767<<SH );*/ *destp = x1 >> SH; destp += inc;
		/*CLAMP( x2, -32768<<SH, 32767<<SH );*/ *destp = x2 >> SH; destp += inc;
		/*CLAMP( x3, -32768<<SH, 32767<<SH );*/ *destp = x3 >> SH; destp += inc;
	}
	decp->y0 = fy0;
	decp->y1 = fy1;
}

static U16 data[4096];
static int headtable[4] = {0,2,8,10};

static INLINE void xa_decode_data( xa_decode_t *xdp, unsigned char *srcp, short *destp, int nbits )
{
	const U8    *sound_groupsp;
	const U8    *sound_datap, *sound_datap2;
	int         i, j, k;
	U16 *datap;

	if (xdp->stereo) // stereo
	{
		if ((nbits == 8) && (xdp->freq == 37800)) // level A
		{
			for (j=0; j < 18; j++)
			{
				sound_groupsp = srcp + j * 128;		// sound groups header
				sound_datap = sound_groupsp + 16;	// sound data just after the header

				for (i=0; i < nbits; i++)
				{
     					datap = data;
    					sound_datap2 = sound_datap + i;
					for (k=0; k < 14; k++, sound_datap2 += 8)
        	   				*(datap++) = (U16)sound_datap2[0] | (U16)(sound_datap2[4] << 8);
					ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+0], data, destp+0, 2 );

        				datap = data;
        				sound_datap2 = sound_datap + i;
        				for (k=0; k < 14; k++, sound_datap2 += 8)
           					*(datap++) = (U16)sound_datap2[0] | (U16)(sound_datap2[4] << 8);
					ADPCM_DecodeBlock16( &xdp->right,  sound_groupsp[headtable[i]+1], data, destp+1, 2 );

	        			destp += 28*2;
				}
    			}
		}
		else  // level B/C
		{	
			for (j=0; j < 18; j++)
			{
				sound_groupsp = srcp + j * 128;		// sound groups header
				sound_datap = sound_groupsp + 16;	// sound data just after the header

				for (i=0; i < nbits; i++) 
	    			{
	    				datap = data;
	    				sound_datap2 = sound_datap + i;
        				for (k=0; k < 7; k++, sound_datap2 += 16) 
        					*(datap++) = (U16)(sound_datap2[ 0] & 0x0f) | ((U16)(sound_datap2[ 4] & 0x0f) <<  4) | ((U16)(sound_datap2[ 8] & 0x0f) <<  8) | ((U16)(sound_datap2[12] & 0x0f) << 12);
					ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+0], data, destp+0, 2 );

	        			datap = data;
	        			sound_datap2 = sound_datap + i;
        				for (k=0; k < 7; k++, sound_datap2 += 16)
           					*(datap++) = (U16)(sound_datap2[ 0] >> 4) | ((U16)(sound_datap2[ 4] >> 4) <<  4) | ((U16)(sound_datap2[ 8] >> 4) <<  8) | ((U16)(sound_datap2[12] >> 4) << 12);
					ADPCM_DecodeBlock16( &xdp->right,  sound_groupsp[headtable[i]+1], data, destp+1, 2 );

	        			destp += 28*2;
				}
	    		}
		}
	} 
	else // mono
	{
		if ((nbits == 8) && (xdp->freq == 37800)) // level A
		{
			for (j=0; j < 18; j++)
			{
    				sound_groupsp = srcp + j * 128;		// sound groups header
    				sound_datap = sound_groupsp + 16;	// sound data just after the header

    				for (i=0; i < nbits; i++)
    				{
        				datap = data;
        				sound_datap2 = sound_datap + i;
        				for (k=0; k < 14; k++, sound_datap2 += 8)
           					*(datap++) = (U16)sound_datap2[0] | (U16)(sound_datap2[4] << 8);
	        			ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+0], data, destp, 1 );
	        			destp += 28;

	        			datap = data;
	        			sound_datap2 = sound_datap + i;
        				for (k=0; k < 14; k++, sound_datap2 += 8)
           					*(datap++) = (U16)sound_datap2[0] | (U16)(sound_datap2[4] << 8);
	       				ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+1], data, destp, 1 );
					destp += 28;
				}
	    	}
		} else // level B/C
		{
			for (j=0; j < 18; j++)
	    		{
	    			sound_groupsp = srcp + j * 128;		// sound groups header
	    			sound_datap = sound_groupsp + 16;	// sound data just after the header

	    			for (i=0; i < nbits; i++) 
	        		{
	        			datap = data;
	        			sound_datap2 = sound_datap + i;
        			
        				for (k=0; k < 7; k++, sound_datap2 += 16)
           					*(datap++) = (U16)(sound_datap2[ 0] & 0x0f) | ((U16)(sound_datap2[ 4] & 0x0f) <<  4) | ((U16)(sound_datap2[ 8] & 0x0f) <<  8) | ((U16)(sound_datap2[12] & 0x0f) << 12);
	        			ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+0], data, destp, 1 );
	        			destp += 28;

		        		datap = data;
		        		sound_datap2 = sound_datap + i;
	        			for (k=0; k < 7; k++, sound_datap2 += 16) 
	            				*(datap++) = (U16)(sound_datap2[ 0] >> 4) | ((U16)(sound_datap2[ 4] >> 4) <<  4) | ((U16)(sound_datap2[ 8] >> 4) <<  8) | ((U16)(sound_datap2[12] >> 4) << 12);
		       			ADPCM_DecodeBlock16( &xdp->left,  sound_groupsp[headtable[i]+1], data, destp, 1 );
					destp += 28;
				}
    			}
		}
	}
}

//================================================================
//=== THIS IS WHAT YOU HAVE TO CALL
//=== xdp              - structure were all important data are returned
//=== sectorp          - data in input
//=== pcmp             - data in output
//=== is_first_sector  - 1 if it's the 1st sector of the stream
//===                  - 0 for any other successive sector
//=== return -1 if error
//================================================================
long xa_decode_sector( xa_decode_t *xdp, unsigned char *sectorp, int is_first_sector ) 
{
    	if ( is_first_sector )
    	{
		switch ( AUDIO_CODING_GET_BPS(((xa_subheader_t *)sectorp)->coding) ) {
			case 0: xdp->nbits = 4; break;
			case 1: xdp->nbits = 8; break;
			default: xdp->nbits = 0; break;
		}

		if ( AUDIO_CODING_GET_STEREO(((xa_subheader_t *)sectorp)->coding)==1 ) {
			xdp->stereo = 1;
			xdp->nsamples = 18 * 28 * 4;
		} else {
			xdp->stereo = 0;
			xdp->nsamples = 18 * 28 * 8;
		}

		switch ( AUDIO_CODING_GET_FREQ(((xa_subheader_t *)sectorp)->coding) ) {
			case 0: xdp->freq = 37800;   break;
			case 1: xdp->freq = 18900;   break;
			default: xdp->freq = 0; return -1;
		}

		xdp->left.y0=0;
		xdp->left.y1=0;
		xdp->right.y0=0;
		xdp->right.y1=0;
    	}
	
	xa_decode_data( xdp, sectorp + sizeof(xa_subheader_t), xdp->pcm, (xdp->nbits == 4 ? 4 : 2) );

	return 0;
}
