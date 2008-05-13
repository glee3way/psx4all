/***************************************************************************
                           StdAfx.h  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2002/05/15 - Pete
// - generic cleanup for the Peops release
//
//*************************************************************************//

//////////////////////////////////////////////////////////
// WINDOWS
//////////////////////////////////////////////////////////

//#ifdef WIN32
//  #include <string.h>
/*
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include "mmsystem.h"
#include <process.h>
#include <stdlib.h>

// enable that for auxprintf();
//#define SMALLDEBUG
//#include <dbgout.h>
//void auxprintf (LPCTSTR pFormat, ...);

#ifdef INLINE
#undef INLINE
#endif
#define INLINE inline
*/
//////////////////////////////////////////////////////////
// LINUX
//////////////////////////////////////////////////////////
//#else


#include "../common.h"

#ifndef __WIN32__
#if !defined(GIZMONDO) && !defined(POCKETPC)
  #include <unistd.h>
  #include <sys/time.h>  
#endif
#endif

#if !defined(GIZMONDO) && !defined(POCKETPC)
#include <stdio.h>
#include <stdlib.h>
#endif

#if defined(GP2X) || defined(ZAURUS) || defined(V43)
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#endif

#ifndef NOTHREADLIB
#include <pthread.h>
#endif
#define RRand(range) (random()%range)  
#include <string.h> 
#include <math.h>  


#undef CALLBACK
#define CALLBACK
#define DWORD unsigned long
#define LOWORD(l)           ((unsigned short)(l)) 
#define HIWORD(l)           ((unsigned short)(((unsigned long)(l) >> 16) & 0xFFFF)) 

/*
#ifdef INLINE
#undef INLINE
#endif
#define INLINE inline
*/
//#endif

#include "psemuxa.h"
