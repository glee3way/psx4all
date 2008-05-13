#ifndef CONFIG_FUNCTIONS_HPP
#define CONFIG_FUNCTIONS_HPP

#include "ConfigCallbacks.hpp"

#include "Utils.hpp"
#include "Preferences.hpp"

extern Preferences prefs;

// based on source generated from FLUID.
// this is the window that pops up when you use the Configure option

class ConfigWindow
{
public:
   ConfigWindow() 
   {
     { Fl_Window* o = new Fl_Window(252, 385, "CDRMooby2 Config");
       w = o;
       { Fl_Check_Button* o = new Fl_Check_Button(20,20,220,40, "Repeat all CDDA tracks");
         o->value(prefs.prefsMap[repeatString] == repeatAllString);
         o->callback((Fl_Callback*)repeatAllCDDA, this);
         repeatAllButton = o;
       }
       { Fl_Check_Button* o = new Fl_Check_Button(20,60,220,40, "Repeat one CDDA track");
         o->value(prefs.prefsMap[repeatString] == repeatOneString);
         o->callback((Fl_Callback*)repeatOneCDDA, this);
         repeatOneButton = o;
       }
       { Fl_Check_Button* o = new Fl_Check_Button(20,100,220,40, "Play one CDDA track and stop");
         o->value(prefs.prefsMap[repeatString] == playOneString);
         o->callback((Fl_Callback*)playOneCDDA, this);
         playOneButton = o;
       }
       { Fl_Value_Slider* o = new Fl_Value_Slider(20, 140, 210, 25, "CDDA Volume");
         o->type(1);
         o->value(atof(prefs.prefsMap[volumeString].c_str()));
         o->callback((Fl_Callback*)CDDAVolume);
       }
       { Fl_Button* o = new Fl_Button(20, 230, 95, 25, "Compress");
         o->callback((Fl_Callback*)bzCompress);
       }
       { Fl_Button* o = new Fl_Button(130, 230, 95, 25, "Decompress");
         o->callback((Fl_Callback*)bzDecompress);
       }
       { Fl_Button* o = new Fl_Button(20, 305, 95, 25, "Compress");
         o->callback((Fl_Callback*)zCompress);
       }
       { Fl_Button* o = new Fl_Button(130, 305, 95, 25, "Decompress");
         o->callback((Fl_Callback*)zDecompress);
       }
       new Fl_Box(5, 200, 250, 25, "bz.image compression");
       new Fl_Box(5, 280, 250, 25, ".Z.table compression");
       { Fl_Return_Button* o = new Fl_Return_Button(165, 350, 80, 25, "OK");
         o->callback((Fl_Callback*)configOK);
       }
       o->set_modal();
       o->end();
     }
     w->show();
     Fl::run();
   }

   ~ConfigWindow()
   {
      
   }

   Fl_Window* w;
   Fl_Check_Button* repeatAllButton;
   Fl_Check_Button* repeatOneButton;
   Fl_Check_Button* playOneButton;
};


#endif
