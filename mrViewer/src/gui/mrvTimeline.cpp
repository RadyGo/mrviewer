/*
   mrViewer - the professional movie and flipbook playback
   Copyright (C) 2007-2014  Gonzalo Garramuno

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file   mrvTimeline.cpp
 * @author gga
 * @date   Fri Oct 13 13:36:04 2006
 *
 * @brief  An Fl_Widget to draw a timeline.
 *
 *
 */

#include "core/mrvI8N.h"
#include <cassert>
#include <cmath>  // for fabs()

#include <core/mrvRectangle.h>
#include <FL/fl_draw.H>

#include "core/mrvColor.h"
#include "core/mrvThread.h"

#include "gui/mrvImageBrowser.h"
#include "gui/mrvTimecode.h"
#include "gui/mrvPreferences.h"
#include "gui/mrvTimeline.h"
#include "gui/mrvImageView.h"
#include "mrViewer.h"
#include "mrvPreferencesUI.h"
#include "mrvReelUI.h"
#include "gui/mrvMedia.h"
#include "gui/mrvMediaList.h"

namespace
{
// Maximum number of frames to show cacheline.  Setting it too high can
// impact GUI playback when the image/movies are that long.
unsigned kMAX_FRAMES = 5000;
double kMinFrame = std::numeric_limits<double>::min();
double kMaxFrame = std::numeric_limits<double>::max();
}


namespace mrv
{

mrv::Timecode::Display Timeline::_display = Timecode::kFrames;

Timeline::Timeline( int x, int y, int w, int h, char* l ) :
mrv::Slider( x, y, w, h, l ),
_draw_cache( true ),
_edl( false ),
_tc( 0 ),
_fps( 24 ),
_display_min( 1 ),
_display_max( 50 ),
uiMain( NULL )
{
    type( TICK_ABOVE );
    slider_type( kNORMAL );
    Fl_Slider::minimum( 1 );
    Fl_Slider::maximum( 50 );
}

Timeline::~Timeline()
{
    _draw_cache = false;
    _edl = false;
    uiMain = NULL;
}

mrv::ImageBrowser* Timeline::browser() const
{
    assert( uiMain != NULL );
    assert( uiMain->uiReelWindow != NULL );
    if ( uiMain == NULL ) return NULL;
    if ( uiMain->uiReelWindow == NULL ) return NULL;
    return uiMain->uiReelWindow->uiBrowser;
}

void Timeline::display_minimum( const double& x )
{
    if ( x >= minimum() ) _display_min = x;

    if ( _edl )
    {
        CMedia* img = image_at( x );
        if ( img ) img->first_frame( global_to_local( x ) );

        ImageBrowser* b = browser();
        if ( b ) b->adjust_timeline();
    }

    if ( uiMain && uiMain->uiView )
    {
        char buf[1024];
        sprintf( buf, N_("TimelineMinDisplay %g"), x );
        uiMain->uiView->send_network( buf );
    }
}

void Timeline::display_maximum( const double& x )
{
    if ( x <= maximum() ) _display_max = x;

    if ( _edl )
    {
        CMedia* img = image_at( x );
        if ( img ) img->last_frame( global_to_local( x ) );

        ImageBrowser* b = browser();
        if ( b ) b->adjust_timeline();
    }

    if ( uiMain && uiMain->uiView )
    {
        char buf[1024];
        sprintf( buf, N_("TimelineMaxDisplay %g"), x );
        uiMain->uiView->send_network( buf );
    }
}

void Timeline::minimum( double x )
{
    Fl_Slider::minimum( x );
    _display_min = x;

    if ( uiMain && uiMain->uiView )
    {
        char buf[1024];
        sprintf( buf, N_("TimelineMin %g"), x );
        uiMain->uiView->send_network( buf );
    }
}

void Timeline::maximum( double x )
{
    Fl_Slider::maximum( x );
    _display_max = x;

    if ( uiMain && uiMain->uiView )
    {
        char buf[1024];
        sprintf( buf, N_("TimelineMax %g"), x );
        uiMain->uiView->send_network( buf );
    }
}

void Timeline::edl( bool x )
{
    _edl = x;

    if ( _edl && uiMain && browser() )
    {
        mrv::Timecode* uiFrame = uiMain->uiFrame;

        // Calculate frame range for timeline
        minimum( 1 );
        if ( uiMain->uiStartFrame )
            uiMain->uiStartFrame->frame( 1 );
        if ( uiMain->uiFrame && uiMain->uiFrame->value() < 1 )
            uiFrame->frame(1);

        uint64_t total = 0;

        const mrv::Reel& reel = browser()->current_reel();
        if ( !reel ) return;

        mrv::MediaList::const_iterator i = reel->images.begin();
        mrv::MediaList::const_iterator e = reel->images.end();

        for ( ; i != e; ++i )
        {
            CMedia* img = (*i)->image();
            if ( (*i)->position() == MRV_NOPTS_VALUE )
                (*i)->position( total );
            total += img->duration();
        }

        maximum( double(total) );
        if ( uiMain->uiEndFrame ) uiMain->uiEndFrame->frame( total );
        if ( uiFrame && uiFrame->value() > int64_t(total) )
            uiFrame->frame(total);
    }

    redraw();
}
void Timeline::draw_ticks(const mrv::Recti& r, int min_spacing)
{
    int x1, y1, x2, y2, dx, dy, w;
    x1 = x2 = r.x()+(slider_size()-1)/2;
    dx = 1;
    y1 = r.y();
    y2 = r.b()-1;
    dy = 0;
    w = r.w();



    fl_push_clip( r.x(), r.y(), r.w(), r.h() );

    if (w <= 0) return;

    double A,B;
    if ( ! uiMain->uiPrefs->uiPrefsTimelineSelectionDisplay->value() &&
         ( display_minimum() > minimum() || display_maximum() < maximum() ) )
    {
        A = display_minimum();
        B = display_maximum();
        if (A > B) {
            A = B;
            B = display_minimum();
        }
    }
    else
    {
        A = minimum();
        B = maximum();
        if (A > B) {
            A = B;
            B = minimum();
        }
    }
    //if (!finite(A) || !finite(B)) return;

    if (min_spacing < 1) min_spacing = 10; // fix for fill sliders
    // Figure out approximate size of min_spacing at zero:

    double mul = 1; // how far apart tick marks are
    double div = 1;
    int smallmod = 5; // how many tick marks apart "larger" ones are
    int nummod = 15; // how many tick marks apart numbers are

    if ( _display != Timecode::kFrames )
    {
        nummod = int(_fps);
    }

    int powincr = 10000;

    double derivative = (B-A)*min_spacing/w;
    if (derivative < step()) derivative = step();
    while (mul*5 <= derivative) mul *= 10;
    while (mul > derivative*2*div) div *= 10;
    if (derivative*div > mul*2) {
        mul *= 5;
        smallmod = 2;
    }
    else if (derivative*div > mul) {
        mul *= 2;
        nummod /= 2;
    }
    if ( nummod <= 1 ) nummod = 1;

    Fl_Color textcolor = fl_contrast( this->labelcolor(), color() );
    if ( _edl ) textcolor = FL_BLACK;
    Fl_Color linecolor = FL_BLACK;
    if ( Preferences::schemes.name == "Black" && !edl() )
    {
        linecolor = fl_rgb_color( 70, 70, 70 );
    }

    fl_color(linecolor);
    char buffer[128];
    for (int n = 0; ; n++) {
        // every ten they get further apart for log slider:
        if (n > powincr) {
            mul *= 10;
            n = (n-1)/10+1;
        }
        double v = mul*n/div;
        if (v > fabs(A) && v > fabs(B)) break;
        int sm = n%smallmod ? 3 : 0;
        if (v >= A && v <= B) {
            int t = slider_position(v, w);
            fl_line(x1+dx*t+dy*sm, y1+dy*t+dx*sm, x2+dx*t, y2+dy*t);
            if (n-1 != 0 && (n-1)%nummod == 0) {
                mrv::Timecode::format( buffer, _display, boost::int64_t(v),
                                       _tc, _fps );
                char* p = buffer;
                fl_font(labelfont(), labelsize());
                fl_color(textcolor);
                int wt = 0, ht = 0;
                fl_measure( p, wt, ht );
                fl_draw(p, float(x1+dx*t-wt/2),
                        float(y1+dy*t+fl_height()-fl_descent()));
                fl_color(linecolor);
            }
        }
        if (v && -v >= A && -v <= B) {
            int t = slider_position(-v, w);
            fl_line(x1+dx*t+dy*sm, y1+dy*t+dx*sm, x2+dx*t, y2+dy*t);
            if (n%nummod == 0) {
                mrv::Timecode::format( buffer, _display, boost::int64_t(-v), _tc,
                                       _fps );
                char* p = buffer;
                fl_font(labelfont(), labelsize());
                fl_color(textcolor);
                // int wt = 0, ht = 0;
                // measure( p, wt, ht );
                fl_draw(p, float(x1+dx*t),
                         float(y1+dy*t+fl_height()-fl_descent()));
                fl_color(linecolor);
            }
        }
    }

    fl_pop_clip();
}

/*!
  Subclasses can use this to redraw the moving part.
  Draw everything that is inside the box, such as the tick marks,
  the moving slider, and the "slot". The slot only drawn if \a slot
  is true. You should already have drawn the background of the slider.
*/
bool Timeline::draw(const mrv::Recti& sr, int flags, bool slot)
{
    // for back compatability, use type flag to set slider size:
    if (type()&16/*FILL*/) slider_size(0);

    mrv::Recti r = sr;

    // draw the tick marks and inset the slider drawing area to clear them:
    if (tick_size() && (type()&TICK_BOTH)) {
        mrv::Recti tr = r;
        // r.move_b(-tick_size());
        // switch (type()&TICK_BOTH) {
        // case TICK_BOTH:
        //     r.y(r.y()+tick_size()/2);
        //     break;
        // case TICK_ABOVE:
        //     r.y(r.h()-tick_size());
        //     tr.set_b(r.center_y());
        //     break;
        // case TICK_BELOW:
        //     tr.set_y(r.center_y()+(slot?3:0));
        //     break;
        // }
        fl_color(fl_inactive(fl_contrast(labelcolor(),color())));
        draw_ticks(tr, (slider_size()+1)/2);
    }

    // if (slot) {
    //     const int slot_size_ = 6;
    //     mrv::Recti sl;
    //     int dx = (slider_size()-slot_size_)/2;
    //     if (dx < 0) dx = 0;
    //     sl.x(dx+r.x());
    //     sl.w(r.w()-2*dx);
    //     sl.y(r.y()+(r.h()-slot_size_+1)/2);
    //     sl.h(slot_size_);
    //     Fl::set_box_color(FL_BLACK);
    //     Fl_Boxtype b = box();
    //     box( FL_THIN_DOWN_BOX );
    //     draw_box();
    //     box( b );
    // }

    // if user directly set selected_color we use it:
    if ( selection_color() ) {
        Fl::set_box_color( selection_color() );
        fl_color(fl_contrast(labelcolor(), selection_color()));
    }

    // figure out where the slider should be:
    // mrv::Recti s(r);
    // int sglyph = FL_ALIGN_INSIDE; // draw a box
    // s.x(r.x()+slider_position(value(),r.w()));
    // s.w(slider_size());
    // if (!s.w()) {
    //     s.w(s.x()-r.x());    // fill slider
    //     s.x(r.x());
    // }

    return true;
}

void Timeline::draw_cacheline( CMedia* img, int64_t pos, int64_t size,
                               int64_t mn, int64_t mx, int64_t frame,
                               const mrv::Recti& r )
{

    int64_t j = frame;


//    if ( !img->has_video() && pos < j ) j = pos;

    int64_t max = frame + size;
    if ( mx < max ) max = mx;

    // If too many frames, playback suffers, so we exit here
    if ( max - j > kMAX_FRAMES ) return;

    int rx = r.x() + (slider_size()-1)/2;
    int ry = r.y() + r.h()/2;
    int ww = r.w();
    int hh = r.h() - 8;

    fl_push_clip( rx, ry, ww, hh );

    CMedia::Cache c = CMedia::kLeftCache;
    fl_color( FL_DARK_GREEN );
    fl_line_style( FL_SOLID, 1 );

    if ( ( img->stereo_output() != CMedia::kNoStereo &&
            img->stereo_output() != CMedia::kStereoLeft ) ||
            img->stereo_input() > CMedia::kSeparateLayersInput )
    {
        c = CMedia::kStereoCache;
        fl_color( FL_GREEN );
    }


    int dx;

#define NO_FRAME_VALUE std::numeric_limits<int>::min()



    int64_t t2;
    while ( j <= max )
    {
        dx = NO_FRAME_VALUE;
        int64_t t = j - pos + 1;
        for ( ; j < max; ++j, ++t )
        {
            if ( img->is_cache_filled( t ) >= c )
            {
                t2 = t;
                dx = rx + slider_position( double(j), ww );
                break;
            }
        }

        if ( dx == NO_FRAME_VALUE )
            break;

        t = j - pos + 1;
        for ( ; j <= max; ++j, ++t )
        {
            if ( img->is_cache_filled( t ) < c )
            {
                int dx2 = rx + slider_position( double(j), ww );
                int wh = dx2-dx;
                fl_rectf( dx, ry, wh, hh );
                dx = NO_FRAME_VALUE;
                break;
            }
        }
    }

    int64_t t = j - pos;  // not +1
    if ( dx != NO_FRAME_VALUE && img->is_cache_filled( t ) >= c )
    {
        int dx2 = rx + slider_position( double(j), ww );
        int wh = dx2-dx;
        fl_rectf( dx, ry, wh, hh );
    }

    fl_pop_clip();

}


void Timeline::draw_selection( const mrv::Recti& r )
{
    int rx = r.x() + (slider_size()-1)/2;
    int  dx = slider_position( _display_min, r.w() );
    int end = slider_position( _display_max, r.w() );

    fl_color( FL_CYAN );
    fl_rectf( rx+dx, r.y(), end-dx, r.h()-8 );
}

int Timeline::handle( int e )
{
    return Fl_Slider::handle( e );
    // if ( r != 0 ) return r;
    // return uiMain->uiView->handle( e );
}

/**
 * Main widget drawing routine
 *
 */
void Timeline::draw()
{
    // Flags flags = this->flags();
    // Flags f2 = flags & ~FOCUSED;
    // if (pushed()) f2 |= PUSHED;
    // flags &= ~HIGHLIGHT;

    int f2 = 0;

    // drawstyle(style(),flags);


    int X = x() + Fl::box_dx(box());
    int Y = y() + Fl::box_dy(box());
    int W = w() - Fl::box_dw(box());
    int H = h() - Fl::box_dh(box());

    mrv::Recti r( X, Y, W, H );


    draw_box();

    // Box* box = this->box();
    // if (!box->fills_rectangle()) draw_background();


    // Get number of frames
    double mn = minimum();
    double mx = maximum();

    if ( uiMain->uiPrefs->uiPrefsTimelineSelectionDisplay->value() )
    {
        mn = display_minimum();
        mx = display_maximum();
    }

    double v  = value();

    if ( !browser() ) return;


    // Draw each rectangle for each segment
    if ( _edl )
    {

        const mrv::Reel& reel = browser()->current_reel();
        if ( !reel ) return;

        mrv::MediaList::const_iterator i = reel->images.begin();
        mrv::MediaList::const_iterator e = reel->images.end();

        _fps = 24.0;

        int ww = r.w();

        // If minimum less than 0, start boxes later
        uint64_t size = 0;
        uint64_t frame = 1;
        int rx = r.x() + (slider_size()-1)/2;

        for ( ; i != e; frame += size, ++i )
        {
            int64_t pos = (*i)->position();
            CMedia* img = (*i)->image();
            size = img->duration();

            // skip this block if outside visible timeline span
            if ( frame + size < mn || frame > mx ) continue;

            int  dx = slider_position( double(frame),      ww );
            int end = slider_position( double(frame+size), ww );

            mrv::Recti lr( rx+dx, r.y(), end-dx, r.h() );

            // Draw a block
            if ( v >= frame && v < frame + size )
            {
                _fps = img->fps();
                fl_color( fl_darker( FL_YELLOW ) );
            }
            else
            {
                fl_color( fl_lighter( labelcolor() ) );
            }

            fl_rectf( lr.x(), lr.y(), lr.w(), lr.h() );
        }


        if ( ( ! uiMain->uiPrefs->uiPrefsTimelineSelectionDisplay->value() ) &&
             ( _display_min > minimum() || _display_max < maximum() ) )
        {
            draw_selection(r);
        }

        frame = 1;
        unsigned idx = 0;
        for ( i = reel->images.begin(); i != e; frame += size, ++i )
        {
            CMedia* img = (*i)->image();

            CMedia::Mutex& m = img->video_mutex();
            SCOPED_LOCK( m );

            size = img->duration();
            int64_t pos = (*i)->position() - img->first_frame();


            // skip this block if outside visible timeline span
            if ( frame + size < mn || frame > mx ) continue;

            if ( _draw_cache )
            {
                draw_cacheline( img, pos, size, int64_t(mn),
                                int64_t(mx),
                                frame, r );
            }

            int dx = rx + slider_position( double(frame), ww );

            fl_color( FL_BLUE );
            fl_line_style( FL_SOLID, 3 );
            fl_line( dx, r.y(), dx, r.b()-1 ); // -1 to compensate line style
            fl_line_style( FL_SOLID );
        }
    }
    else
    {
        if ( _draw_cache )
        {
            mrv::media m = browser()->current_image();
            if ( m )
            {
                CMedia* img = m->image();
                CMedia::Mutex& mtx = img->video_mutex();
                SCOPED_LOCK( mtx );
                boost::int64_t first = img->first_frame();
                int64_t pos = 1;
                if ( _edl )
                    pos = m->position() - img->first_frame();
                draw_cacheline( img, pos,
                                img->duration() + img->start_number(),
                                int64_t(mn), int64_t(mx),
                                first, r );
            }
        };

        if ( ( ! uiMain->uiPrefs->uiPrefsTimelineSelectionDisplay->value() ) &&
             ( _display_min > minimum() || _display_max < maximum() ) )
        {
            draw_selection(r);
        }

    }

    //Fl_Slider::draw();

    draw( r, f2, r.y()==0 );

    X = x() - Fl::box_dx(box()) + slider_position( value(), w() -
                                                   Fl::box_dw(box()) );

    Y = y() + Fl::box_dy(box());
    W = 15  - Fl::box_dw(box());
    H = h() - Fl::box_dh(box());
    fl_push_clip( X, Y, W, H );
    Fl_Color c = fl_rgb_color( 180, 180, 128 );
    draw_box( FL_PLASTIC_UP_BOX, X, Y, W, H, c );
    clear_damage();
    fl_pop_clip();
}

/**
 * Given an image, return its offset from frame 1 when in edl mode
 *
 * @param img image to search in edl list
 *
 * @return offset in timeline
 */
uint64_t Timeline::offset( const CMedia* img ) const
{
    if ( !img ) return 0;

    mrv::Reel reel = browser()->current_reel();
    if (!reel) return 0;

    return reel->offset( img );
}

/**
 * Given a frame, return its image index in browser when in edl mode
 *
 * @param f frame to search in edl list
 *
 * @return index of image in image browser list
 */
size_t Timeline::index( const int64_t f ) const
{
    const mrv::Reel& reel = browser()->current_reel();
    if (!reel) return 0;

    mrv::MediaList::const_iterator i = reel->images.begin();
    mrv::MediaList::const_iterator e = reel->images.end();

    double mn = minimum();
    double mx = maximum();
    if ( mn > mx )
    {
        double t = mx;
        mx = mn;
        mn = t;
    }

    if ( f < boost::int64_t(mn) ) return 0;
    if ( f > boost::int64_t(mx) ) return unsigned(e - i);

    int64_t  t = 1;
    size_t r = 0;
    for ( ; i != e; ++i, ++r )
    {
        CMedia* img = (*i)->image();
        uint64_t size = img->duration();
        t += size;
        if ( t > f ) break;
    }
    if ( r >= reel->images.size() ) r = reel->images.size() - 1;
    return r;
}

/**
 * Given a frame, return the image in browser for that frame when in edl mode
 *
 * @param f frame to search in edl list
 *
 * @return image at that point in the timeline or NULL
 */
mrv::media Timeline::media_at( const int64_t f ) const
{
    mrv::Reel reel = browser()->current_reel();
    if (!reel) return mrv::media();

    return reel->media_at( f );
}

CMedia* Timeline::image_at( const int64_t f ) const
{
    mrv::media m = media_at( f );
    if ( !m ) return NULL;
    return m->image();
}
/**
 * Given an image, return its offset from frame 1 when in edl mode
 *
 * @param img image to search in edl list
 *
 * @return offset in timeline
 */
int64_t Timeline::global_to_local( const int64_t frame ) const
{
    mrv::Reel reel = browser()->current_reel();
    if (!reel) return 0;

    return reel->global_to_local( frame );
}

void change_timeline_display( ViewerUI* uiMain )
{
    int i = uiMain->uiTimecodeSwitch->value();
    const char* label = uiMain->uiTimecodeSwitch->child(i)->label();

    char buf[3];
    buf[0] = label[0];
    buf[1] = ':';
    buf[2] = 0;

    uiMain->uiTimecodeSwitch->copy_label( buf );
    uiMain->uiTimecodeSwitch->redraw();

    mrv::Timecode::Display d = (mrv::Timecode::Display) i;
    uiMain->uiFrame->display( d );
    uiMain->uiStartFrame->display( d );
    uiMain->uiEndFrame->display( d );
    uiMain->uiTimeline->display( d );
}

int Timeline::slider_position( double value, int w )
{
    double A = minimum();
    double B = maximum();

    if ( uiMain->uiPrefs->uiPrefsTimelineSelectionDisplay->value() )
    {
        A = display_minimum();
        B = display_maximum();
    }

    if (B == A) return 0;
    bool flip = B < A;
    if (flip) {A = B; B = minimum();}
    if (!horizontal()) flip = !flip;
    // if both are negative, make the range positive:
    if (B <= 0) {flip = !flip; double t = A; A = -B; B = -t; value = -value;}
    double fraction;
    if (!(slider_type() & kLOG)) {
        // linear slider
    fraction = (value-A)/(B-A);
  } else if (A > 0) {
    // logatithmic slider
    if (value <= A) fraction = 0;
    else fraction = (::log(value)-::log(A))/(::log(B)-::log(A));
  } else if (A == 0) {
    // squared slider
    if (value <= 0) fraction = 0;
    else fraction = sqrt(value/B);
  } else {
    // squared signed slider
    if (value < 0) fraction = (1-sqrt(value/A))*.5;
    else fraction = (1+sqrt(value/B))*.5;
  }
  if (flip) fraction = 1-fraction;
  w -= slider_size(); if (w <= 0) return 0;
  if (fraction >= 1) return w;
  else if (fraction <= 0) return 0;
  else return int(fraction*w+.5);
}

} // namespace mrv
