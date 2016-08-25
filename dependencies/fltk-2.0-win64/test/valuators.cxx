// generated by Fast Light User Interface Designer (fluid) version 2.1000

#include "valuators.h"

static void callback(fltk::Widget* o, void*) {
  printf("%g     \r", ((fltk::Valuator*)o)->value());
  fflush(stdout);
}

#include <fltk/run.h>

int main (int argc, char **argv) {

  fltk::Window* w;
   {fltk::Window* o = new fltk::Window(565, 490, "Valuator classes, showing values for the type()");
    w = o;
    o->labelsize(10);
    o->shortcut(0xff1b);
    o->begin();
     {fltk::Widget* o = new fltk::Widget(10, 10, 280, 235, "Fl_Slider");
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->color((fltk::Color)49);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_CENTER);
    }
     {fltk::Slider* o = new fltk::Slider(25, 49, 20, 157, "VERTICAL");
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_TOP);
    }
     {fltk::Slider* o = new fltk::Slider(60, 70, 20, 158, "VERTICAL|TICK_LEFT");
      o->type(fltk::Slider::TICK_ABOVE);
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Slider* o = new fltk::Slider(100, 49, 20, 157, "VERTICAL|TICK_RIGHT");
      o->type(fltk::Slider::TICK_BELOW);
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_TOP);
    }
     {fltk::Slider* o = new fltk::Slider(140, 54, 130, 16, "HORIZONTAL");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Slider* o = new fltk::Slider(140, 81, 130, 22, "HORIZONTAL|TICK_ABOVE");
      o->type(fltk::Slider::TICK_ABOVE);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Slider* o = new fltk::Slider(140, 119, 130, 22, "HORIZONTAL|TICK_ABOVE,box");
      o->type(fltk::Slider::TICK_ABOVE);
      o->box(fltk::DOWN_BOX);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Slider* o = new fltk::Slider(140, 157, 130, 22, "HORIZONTAL|TICK_BELOW");
      o->type(fltk::Slider::TICK_BELOW);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Slider* o = new fltk::Slider(140, 201, 130, 22, "HORIZONTAL|TICK_BOTH");
      o->type(fltk::Slider::TICK_BOTH);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Widget* o = new fltk::Widget(295, 10, 260, 126, "Fl_Value_Input");
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->color((fltk::Color)49);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_CENTER);
    }
     {fltk::ValueInput* o = new fltk::ValueInput(360, 35, 180, 22, "outside label");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueInput* o = new fltk::ValueInput(310, 63, 100, 22, "inside");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_LEFT|fltk::ALIGN_CENTER);
    }
     {fltk::ValueInput* o = new fltk::ValueInput(410, 63, 65, 22, "x");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->maximum(100);
      o->step(0.1);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_LEFT|fltk::ALIGN_CENTER);
    }
     {fltk::ValueInput* o = new fltk::ValueInput(475, 63, 65, 22, "y");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->maximum(100);
      o->step(0.1);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_LEFT|fltk::ALIGN_CENTER);
    }
     {fltk::ValueInput* o = new fltk::ValueInput(360, 93, 180, 32, "larger");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Widget* o = new fltk::Widget(10, 250, 280, 229, "Fl_Value_Slider");
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->color((fltk::Color)49);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_CENTER);
    }
     {fltk::Widget* o = new fltk::Widget(295, 141, 145, 131, "   Fl_Scrollbar");
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->color((fltk::Color)49);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_LEFT|fltk::ALIGN_CENTER);
    }
     {fltk::Scrollbar* o = new fltk::Scrollbar(300, 240, 105, 20, "HORIZONTAL");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->maximum(100);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Scrollbar* o = new fltk::Scrollbar(405, 145, 20, 115, "VERTICAL");
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->minimum(100);
      o->maximum(0);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(25, 277, 30, 158, "VERTICAL");
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->textsize(10);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_TOP);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(65, 310, 30, 152, "VERTICAL|TICK_LEFT");
      o->type(fltk::ValueSlider::TICK_ABOVE);
      o->set_vertical();
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->textsize(10);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(105, 283, 35, 158, "VERTICAL|TICK_LEFT,box");
      o->type(fltk::ValueSlider::TICK_ABOVE);
      o->set_vertical();
      o->box(fltk::DOWN_BOX);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->textsize(10);
      o->callback((fltk::Callback*)callback);
      o->align(fltk::ALIGN_TOP);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(145, 294, 130, 21, "HORIZONTAL");
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(145, 337, 130, 22, "HORIZONTAL|TICK_BELOW");
      o->type(fltk::ValueSlider::TICK_BELOW);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(145, 381, 130, 21, "HORIZONTAL|TICK_BELOW,box");
      o->type(fltk::ValueSlider::TICK_BELOW);
      o->box(fltk::DOWN_BOX);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ValueSlider* o = new fltk::ValueSlider(145, 424, 130, 33, "HORIZONTAL|TICK_BOTH");
      o->type(fltk::ValueSlider::TICK_BOTH);
      o->color((fltk::Color)10);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Widget* o = new fltk::Widget(295, 277, 145, 136, "Fl_Roller");
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->color((fltk::Color)49);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_CENTER);
    }
     {fltk::ThumbWheel* o = new fltk::ThumbWheel(305, 340, 90, 20, "HORIZONTAL");
      o->color((fltk::Color)0xe6e7e600);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::ThumbWheel* o = new fltk::ThumbWheel(405, 299, 20, 103, "VERTICAL");
      o->set_vertical();
      o->color((fltk::Color)0xe6e7e600);
      o->selection_color((fltk::Color)1);
      o->labelsize(8);
      o->callback((fltk::Callback*)callback);
    }
     {fltk::Group* o = new fltk::Group(445, 141, 110, 272, "Fl_Dial");
      o->set_vertical();
      o->box(fltk::ENGRAVED_BOX);
      o->labelfont(fltk::HELVETICA_BOLD);
      o->align(fltk::ALIGN_TOP|fltk::ALIGN_CENTER);
      o->begin();
       {fltk::Dial* o = new fltk::Dial(23, 24, 63, 65, "NORMAL");
        o->set_vertical();
        o->color((fltk::Color)10);
        o->selection_color((fltk::Color)1);
        o->labelsize(8);
        o->value(0.5);
        o->callback((fltk::Callback*)callback);
        o->angles(0,315);
      }
       {fltk::Dial* o = new fltk::Dial(23, 104, 63, 65, "LINE");
        o->type(fltk::Dial::LINE);
        o->set_vertical();
        o->color((fltk::Color)10);
        o->selection_color((fltk::Color)1);
        o->labelsize(8);
        o->value(0.5);
        o->callback((fltk::Callback*)callback);
      }
       {fltk::Dial* o = new fltk::Dial(23, 184, 63, 65, "FILL");
        o->type(fltk::Dial::FILL);
        o->set_vertical();
        o->color((fltk::Color)10);
        o->selection_color((fltk::Color)1);
        o->labelsize(8);
        o->value(0.75);
        o->callback((fltk::Callback*)callback);
        o->angles(0,360);
      }
      o->end();
    }
     {fltk::Widget* o = new fltk::Widget(295, 419, 260, 60, "All widgets have color(green) and selection_color(red) to show the areas thes\
e colors affect.");
      o->box(fltk::ENGRAVED_BOX);
      o->labelsize(10);
      o->align(fltk::ALIGN_WRAP);
    }
    o->end();
    o->resizable(o);
  }
  w->show(argc, argv);
  return  fltk::run();
}
