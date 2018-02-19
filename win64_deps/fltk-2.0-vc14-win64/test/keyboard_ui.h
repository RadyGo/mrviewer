// generated by Fast Light User Interface Designer (fluid) version 2.1000

#ifndef keyboard_ui_h
#define keyboard_ui_h
#include <fltk/Window.h>
#include <fltk/Output.h>
extern fltk::Output* key_output;
extern fltk::Output* text_output;
#include <fltk/Button.h>
extern void key_cb(fltk::Button*, void*);
#include <fltk/Widget.h>
extern void shift_cb(fltk::Button*, void*);
fltk::Window* make_window();
#endif
