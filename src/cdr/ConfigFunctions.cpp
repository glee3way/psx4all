/************************************************************************

Copyright mooby 2002

CDRMooby2 ConfigFunctions.cpp
http://mooby.psxfanatics.com

  This file is protected by the GNU GPL which should be included with
  the source code distribution.

************************************************************************/



#include "defines.h"

#include "externs.h"

#include <FL/Fl.H>
#include <FL/fl_ask.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Check_Button.H>

#include <iostream>
#include "Utils.hpp"
#include "Preferences.hpp"
#include "ConfigFunctions.hpp"

extern Preferences prefs;

/** configure plugin external functions **/

long CALLBACK CDRconfigure(void)
{
   ConfigWindow();
   prefs.write();
   return 0;
}


int    CD_Configure(UINT32 *par)
{
	ConfigWindow();
   prefs.write();
   return 0;
}

void CALLBACK CDVDconfigure()
{
   CDRconfigure();
}
