// generated by Fast Light User Interface Designer (fluid) version 2.1000

#ifndef mrvFontsWindowUI_h
#define mrvFontsWindowUI_h
#include <fltk/DoubleBufferWindow.h>
extern fltk::DoubleBufferWindow* uiMain;
#include <fltk/Group.h>
#include <fltk/Input.h>
#include <fltk/Choice.h>
#include <fltk/ValueSlider.h>

namespace fltk {
class Font;
}

namespace mrv  {

extern fltk::Font* font_current;
extern unsigned    font_size;
extern std::string font_text;

fltk::DoubleBufferWindow* make_window();
}
#endif
