#ifndef CONFIG_CALLBACKS_HPP
#define CONFIG_CALLBACKS_HPP

void bzCompress(Fl_Button*, void*);
void bzDecompress(Fl_Button*, void*);
void zCompress(Fl_Button*, void*);
void zDecompress(Fl_Button*, void*);
void CDDAVolume(Fl_Value_Slider*, void*);
void repeatAllCDDA(Fl_Check_Button*, void* val = NULL);
void repeatOneCDDA(Fl_Check_Button*, void* val = NULL);
void playOneCDDA(Fl_Check_Button*, void* val = NULL);
void configOK(Fl_Return_Button*, void*);

#endif
