
/*
    mrViewer - the professional movie and flipbook playback
    Copyright (C) 2007-2014  Gonzalo Garramuño

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
 * @file   mrvPlayback.cpp
 * @author gga
 * @date   Fri Jul  6 17:43:07 2007
 *
 * @brief  This file implements a callback that is used to play videos or
 *         image sequences.  This callback is usually run as part of a new
 *         playback.
 *
 *
 */

#include <cstdio>

#include <iostream>

extern "C" {
#include <libavutil/time.h>
}

#ifdef _WIN32
# include <float.h>
# define isnan _isnan
#endif

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_array.hpp>

#include "core/CMedia.h"
#include "core/aviImage.h"
#include "core/mrvMath.h"
#include "core/mrvTimer.h"
#include "core/mrvThread.h"
#include "core/mrvBarrier.h"

#include "gui/mrvIO.h"
#include "gui/mrvReel.h"
#include "gui/mrvTimeline.h"
#include "gui/mrvImageView.h"
#include "gui/mrvImageBrowser.h"
#include "mrViewer.h"

#include "mrvPlayback.h"


namespace
{
  const char* kModule = "play";
}

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

// #undef TRACE
// #define TRACE(x)

//#undef DBG
//#define DBG(x)  std::cerr << x << std::endl

#undef LOG
#define LOG(x) //std::cerr << x << std::endl;

#define DEBUG_THREADS

typedef boost::recursive_mutex Mutex;



void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}



namespace mrv {


enum EndStatus {
kEndIgnore,
kEndStop,
kEndNextImage,
kEndChangeDirection,
kEndLoop,
};


double get_clock(Clock *c)
{
    // if (*c->queue_serial != c->serial)
    // {
    //   return NAN;
    // } else {
    double time = av_gettime_relative() / 1000000.0;
    return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
    // }
}

void set_clock_at(Clock *c, double pts, int serial, double time)
{
    c->pts = pts;
    c->last_updated = time;
    c->pts_drift = c->pts - time;
    c->serial = serial;
}

static void set_clock(Clock *c, double pts, int serial)
{
    double time = av_gettime_relative() / 1000000.0;
    set_clock_at(c, pts, serial, time);
}



static void set_clock_speed(Clock *c, double speed)
{
    set_clock(c, get_clock(c), c->serial);
    c->speed = speed;
}

static void init_clock(Clock *c, int *queue_serial)
{
    c->speed = 1.0;
    c->paused = 0;
    c->queue_serial = queue_serial;
    set_clock(c, 0, -1);
}


void sync_clock_to_slave(Clock *c, Clock *slave)
{
    double clock = get_clock(c);
    double slave_clock = get_clock(slave);

#if __cplusplus >= 201103L
    using std::isnan;
#endif
    if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
        set_clock(c, slave_clock, -1);
}

void update_video_pts(CMedia* is, double pts, int64_t pos, int serial) {
    /* update current video pts */
    set_clock(&is->vidclk, pts, serial);
    sync_clock_to_slave(&is->extclk, &is->vidclk);
}

inline int get_master_sync_type(CMedia* img) {
    if (img->av_sync_type == CMedia::AV_SYNC_VIDEO_MASTER) {
        if (img->has_picture())
            return CMedia::AV_SYNC_VIDEO_MASTER;
        else
            return CMedia::AV_SYNC_AUDIO_MASTER;
    } else if (img->av_sync_type ==  CMedia::AV_SYNC_AUDIO_MASTER) {
        if (img->has_audio())
            return  CMedia::AV_SYNC_AUDIO_MASTER;
        else
            return  CMedia::AV_SYNC_EXTERNAL_CLOCK;
    } else {
        return  CMedia::AV_SYNC_EXTERNAL_CLOCK;
    }
}

inline double get_master_clock(CMedia* img)
{
    double val;

    switch (get_master_sync_type(img)) {
        case CMedia::AV_SYNC_VIDEO_MASTER:
            val = get_clock(&img->vidclk);
            break;
        case CMedia::AV_SYNC_AUDIO_MASTER:
            val = get_clock(&img->audclk);
            break;
        default:
            val = get_clock(&img->extclk);
            break;
    }
    return val;
}


inline unsigned int barrier_thread_count( const CMedia* img )
{
    unsigned r = 1;               // 1 for decode thread
    if    ( img->valid_video() )    r += 1;
    if    ( img->valid_audio() )    r += 1;
    if    ( img->valid_subtitle() ) r += 1;
    return r;
}


CMedia::DecodeStatus check_loop( const int64_t frame,
                                 CMedia* img,
                                 const mrv::Reel reel,
                                 const mrv::Timeline* timeline,
                                 int64_t& first,
                                 int64_t& last )
{
    last = int64_t(timeline->maximum()) + img->first_frame() - 1;
    first = int64_t(timeline->minimum()) + img->first_frame() - 1;


    AVRational rp;
    rp.num = img->play_fps() * 1000;
    rp.den = 1000;

    AVRational rt;
    rt.num = timeline->fps() * 1000;
    rt.den = 1000;

    first = av_rescale_q( first, rp, rt );
    last  = av_rescale_q( last, rp, rt );

    
    // std::cerr << "check loop reel " << reel->name << std::endl;
    if ( reel->edl )
    {
        CMedia::Mutex& m = img->video_mutex();
        SCOPED_LOCK( m );


        boost::int64_t s = reel->location(img);
        boost::int64_t e = s + img->duration() - 1;

        if ( e < last )  last = e;
        if ( s > first ) first = s;

        last = reel->global_to_local( last );
        first = reel->global_to_local( first );

    }
    else
    {
        if ( img->has_video() || img->has_audio() )
        {
            CMedia::Mutex& m = img->video_mutex();
            SCOPED_LOCK( m );

            int64_t s = img->first_frame();
            int64_t e = img->last_frame();
            
            s = av_rescale_q( s, rp, rt );
            e = av_rescale_q( e, rp, rt );

            if ( last > e )
                last = e;
            else if ( s > first )
                first = s;
        }
    }

    if ( frame > last )
    {
        return CMedia::kDecodeLoopEnd;
    }
    else if ( frame < first )
    {
        return CMedia::kDecodeLoopStart;
    }

    
    return CMedia::kDecodeOK;
}

CMedia::DecodeStatus check_decode_loop( const int64_t frame,
                                        CMedia* img,
                                        const mrv::Reel reel,
                                        const mrv::Timeline* timeline )
{
    int64_t first, last;
    CMedia::DecodeStatus status = check_loop( frame, img, reel, timeline,
                                              first, last );

    if ( status == CMedia::kDecodeLoopEnd )
    {
        img->loop_at_end( last+1 );
    }
    else if ( status == CMedia::kDecodeLoopStart )
    {
        img->loop_at_start( first-1 );
    }
    return status;
}



EndStatus handle_loop( boost::int64_t& frame,
                       int&     step,
                       CMedia* img,
                       bool    fg,
                       mrv::ViewerUI* uiMain,
                       const mrv::Reel  reel,
                       const mrv::Timeline* timeline,
                       const mrv::CMedia::DecodeStatus end )
{

    if ( !img || !timeline || !reel || !uiMain ) return kEndIgnore;

    // std::cerr << "handle loop reel " << reel->name << std::endl;

    CMedia::Mutex& m = img->video_mutex();
    SCOPED_LOCK( m );

    CMedia::Mutex& ma = img->audio_mutex();
    SCOPED_LOCK( ma );

    mrv::ImageView* view = uiMain->uiView;

    EndStatus status = kEndIgnore;
    mrv::media c;
    CMedia* next = NULL;

    int64_t first, last;
    check_loop( frame, img, reel, timeline, first, last );

    CMedia::Looping loop = view->looping();


    switch( end )
    {
        case CMedia::kDecodeLoopEnd:
            {

                if ( reel->edl )
                {
                    boost::int64_t f = frame;

                    f -= img->first_frame();
                    f += reel->location(img);

                    if ( f <= timeline->maximum() )
                    {
                        next = reel->image_at( f );
                    }


                    if ( next )
                    {
                        f = reel->global_to_local( f );
                    }
                    else
                    {
                        if ( loop == CMedia::kLoop )
                        {
                            f = boost::int64_t(timeline->minimum());
                            next = reel->image_at( f );
                            f = reel->global_to_local( f );
                        }
                        else
                        {
                            next = img;
                        }
                    }

                    if ( next != img && next != NULL)
                    {
                        //if ( video )
                        {
                            CMedia::Mutex& m2 = next->video_mutex();
                            SCOPED_LOCK( m2 );

                            if ( next->stopped() )
                            {
                                next->seek( f );
                                next->do_seek();
                                next->play( CMedia::kForwards, uiMain, fg );
                            }

                            img->playback( CMedia::kStopped );
                            if ( img->has_video() ) img->clear_cache();
                        }

                        status = kEndNextImage;
                        return status;
                    }
                    if ( img->stopped() ) return kEndNextImage;
                }

                if ( loop == CMedia::kLoop )
                {
                    frame = first;
                    //if ( img->right_eye() ) img->right_eye()->seek( frame );
                    status = kEndLoop;
                    init_clock(&img->vidclk, NULL);
                    init_clock(&img->audclk, NULL);
                    init_clock(&img->extclk, NULL);
                    set_clock(&img->extclk, get_clock(&img->extclk), false);
                }
                else if ( loop == CMedia::kPingPong )
                {
                    frame = last;
                    step  = -1;
                    img->playback( CMedia::kBackwards );
                    if (fg)
                        view->playback( CMedia::kBackwards );
                    status = kEndChangeDirection;
                    init_clock(&img->vidclk, NULL);
                    init_clock(&img->audclk, NULL);
                    init_clock(&img->extclk, NULL);
                    set_clock(&img->extclk, get_clock(&img->extclk), false);
                }
                else
                {
                    if (fg)
                        view->playback( CMedia::kStopped );
                    img->playback( CMedia::kStopped );
                }
                break;
            }
        case CMedia::kDecodeLoopStart:
            {
                if ( reel->edl )
                {
                    boost::int64_t f = frame;

                    f -= img->first_frame();
                    f += reel->location(img);

                    if ( f >= timeline->minimum() )
                    {
                        next = reel->image_at( f );
                    }


                    f = reel->global_to_local( f );

                    if ( !next )
                    {
                        if ( loop == CMedia::kLoop )
                        {
                            f = boost::int64_t( timeline->maximum() );
                            next = reel->image_at( f );
                            f = reel->global_to_local( f );
                        }
                        else
                        {
                            next = img;
                        }
                    }


                    if ( next != img && next != NULL )
                    {
                        //if ( video )
                        {
                            CMedia::Mutex& m2 = next->video_mutex();
                            SCOPED_LOCK( m2 );

                            if ( next->stopped() )
                            {
                                next->seek( f );
                                next->do_seek();
                                next->play( CMedia::kBackwards, uiMain, fg );
                            }

                            img->playback( CMedia::kStopped );
                            img->flush_all();
                            if ( img->has_video() ) img->clear_cache();
                        }

                        status = kEndNextImage;
                        return status;
                    }
                    if ( img->stopped() ) return kEndNextImage;
                }

                if ( loop == CMedia::kLoop )
                {
                    frame = last;
                    //if ( img->right_eye() ) img->right_eye()->seek( frame );
                    status = kEndLoop;
                    init_clock(&img->vidclk, NULL);
                    init_clock(&img->audclk, NULL);
                    init_clock(&img->extclk, NULL);
                    set_clock(&img->extclk, get_clock(&img->extclk), false);
                }
                else if ( loop == CMedia::kPingPong )
                {
                    frame = first;
                    step = 1;
                    img->playback( CMedia::kForwards );
                    if (fg)
                        view->playback( CMedia::kForwards );
                    status = kEndChangeDirection;
                    init_clock(&img->vidclk, NULL);
                    init_clock(&img->audclk, NULL);
                    init_clock(&img->extclk, NULL);
                    set_clock(&img->extclk, get_clock(&img->extclk), false);
                }
                else
                {
                    img->playback( CMedia::kStopped );
                    if (fg)
                        view->playback( CMedia::kStopped );
                }
                break;
            }
        default:
            {
                // error
            }
    }

    if ( status == kEndStop || status == kEndNextImage )
    {
        img->playback( CMedia::kStopped );
        if ( img->has_video() ) img->clear_cache();
    }

    return status;
}




//
// Main loop used to play audio (of any image)
//
void audio_thread( PlaybackData* data )
{
    assert( data != NULL );

    mrv::ViewerUI*     uiMain   = data->uiMain;
    assert( uiMain != NULL );
    CMedia* img = data->image;
    assert( img != NULL );

    bool fg = data->fg;

    // delete the data (we don't need it anymore)
    delete data;


    int64_t frame = img->frame() + img->audio_offset();


    int64_t failed_frame = std::numeric_limits< int64_t >::min();


    mrv::ImageView*      view = uiMain->uiView;
    mrv::Timeline*      timeline = uiMain->uiTimeline;
    mrv::ImageBrowser*   browser = uiMain->uiReelWindow->uiBrowser;


    int idx = fg ? view->fg_reel() : view->bg_reel();

    mrv::Reel   reel = browser->reel_at( idx );
    if (!reel) return;

#ifdef DEBUG_THREADS
    DBG("ENTER " << (fg ? "FG" : "BG") << " AUDIO THREAD " << img->name() << " stopped? " << img->stopped() << " frame " << frame);
#endif
    mrv::Timer timer;


    //img->av_sync_type = CMedia::AV_SYNC_EXTERNAL_CLOCK;
    img->av_sync_type = CMedia::AV_SYNC_AUDIO_MASTER;
    init_clock(&img->vidclk, NULL);
    init_clock(&img->audclk, NULL);
    init_clock(&img->extclk, NULL);
    set_clock(&img->extclk, get_clock(&img->extclk), false);


    while ( !img->stopped() && view->playback() != CMedia::kStopped )
    {

        int step = (int) img->playback();
        if ( step == 0 ) break;


        // DBG( "wait audio " << frame );
        img->wait_audio();


        boost::int64_t f = frame;
        // DBG( "decode audio " << frame );



        CMedia::DecodeStatus status = img->decode_audio( f );
        //DBG( img->name() << " decoded audio " << f << " status " << status );


        // if ( !img->right_eye() )
        //     img->debug_audio_packets( frame, "play", true );
        
        if ( status != CMedia::kDecodeError )
        {
            assert( img != NULL );
            assert( reel != NULL );
            assert( timeline != NULL );
            int64_t first, last;
            status = check_loop( frame, img, reel, timeline, first, last );
        }



        switch( status )
        {
            case CMedia::kDecodeError:
                LOG_ERROR( img->name()
                           << _(" - decode Error audio frame ") << frame );
                frame += step;
                continue;
            case CMedia::kDecodeMissingFrame:
                LOG_WARNING( img->name()
                             << _(" - decode missing audio frame ") << frame );
                timer.setDesiredFrameRate( img->play_fps() );
                timer.waitUntilNextFrameIsDue();
                frame += step;
                continue;
            case CMedia::kDecodeNoStream:
                timer.setDesiredFrameRate( img->play_fps() );
                timer.waitUntilNextFrameIsDue();
                if ( fg && !img->has_picture() && reel->edl &&
                     img->is_left_eye() )
                {
                    int64_t f = frame + reel->location(img) - img->first_frame();
                    f -= img->audio_offset();
                    view->frame( f );
                }
                frame += step;
                continue;
            case  CMedia::kDecodeLoopEnd:
            case  CMedia::kDecodeLoopStart:
                {

                    // LOG_INFO( img->name() << " BARRIER IN AUDIO WAIT "
                    //           << frame );

                    img->remove_to_end( CMedia::kAudioStream );

                    CMedia::Barrier* barrier = img->loop_barrier();
                    // Wait until all threads loop and decode is restarted
                    bool ok = barrier->wait();

                    // LOG_INFO( img->name() << " BARRIER PASSED IN AUDIO "
                    //           << frame );

                    if ( img->stopped() ) continue;


                    frame -= img->audio_offset();

                    EndStatus end = handle_loop( frame, step, img, fg, uiMain,
                                                 reel, timeline, status );

                    frame += img->audio_offset();



                    DBG( img->name() << " AUDIO LOOP END/START HAS FRAME " << frame );
                    continue;
                }
            case CMedia::kDecodeOK:
                break;
            default:
                break;
        }



        if ( ! img->has_audio() && img->has_picture() )
        {
            // if audio was turned off, follow video.
            // audio can be turned off due to user changing channels
            // or due to a problem with audio engine.
            frame = img->frame();
            continue;
        }



        if ( fg && !img->has_picture() && reel->edl && img->is_left_eye() )
        {
            int64_t offset = img->audio_offset();
            int64_t f = frame + reel->location(img) - img->first_frame();
            f -= offset;
            view->frame( f );
        }


        if ( !img->stopped() )
        {
            img->find_audio(frame);
        }

        frame += step;
    }



    
#ifdef DEBUG_THREADS
    DBG( "EXIT " << (fg ? "FG" : "BG") << " AUDIO THREAD " << img->name() << " stopped? "  << img->stopped() << " frame " << img->audio_frame() );
    assert( img->stopped() );
#endif

} // audio_thread



  //
  // Main loop used to decode subtitles
  //
void subtitle_thread( PlaybackData* data )
{
    assert( data != NULL );

    mrv::ViewerUI*     uiMain   = data->uiMain;

    CMedia* img = data->image;
    assert( img != NULL );



    bool fg = data->fg;

    // delete the data (we don't need it anymore)
    delete data;


    mrv::ImageView*      view = uiMain->uiView;
    mrv::Timeline*      timeline = uiMain->uiTimeline;
    mrv::ImageBrowser*   browser = uiMain->uiReelWindow->uiBrowser;

    int idx = fg ? view->fg_reel() : view->bg_reel();

    mrv::Reel   reel = browser->reel_at( idx );
    if (!reel) return;

    mrv::Timer timer;

#ifdef DEBUG_THREADS
    cerr << "ENTER SUBTITLE THREAD " << img->name() << endl;
#endif


    while ( !img->stopped() && view->playback() != CMedia::kStopped )
    {
        int step = (int) img->playback();
        if ( step == 0 ) break;

        int64_t frame = img->frame() + step;
        CMedia::DecodeStatus status = img->decode_subtitle( frame );

        if ( status != CMedia::kDecodeError )
        {
            int64_t first, last;
            status = check_loop( frame, img, reel, timeline, first, last );
        }

        
        switch( status )
        {
            case CMedia::kDecodeError:
            case CMedia::kDecodeMissingFrame:
            case CMedia::kDecodeMissingSamples:
            case CMedia::kDecodeDone:
            case CMedia::kDecodeNoStream:
            case CMedia::kDecodeOK:
                img->find_subtitle( frame );
                break;
            case CMedia::kDecodeLoopEnd:
            case CMedia::kDecodeLoopStart:

                img->remove_to_end( CMedia::kSubtitleStream );
                
                CMedia::Barrier* barrier = img->loop_barrier();
                // Wait until all threads loop and decode is restarted
                barrier->wait();

                if ( img->stopped() ) continue;

                EndStatus end = handle_loop( frame, step, img, fg, uiMain,
                                             reel, timeline, status );
                continue;
        }

        double fps = img->play_fps();
        timer.setDesiredFrameRate( fps );
        timer.waitUntilNextFrameIsDue();

    }


#ifdef DEBUG_THREADS
    cerr << endl << "EXIT  SUBTITLE THREAD " << img->name()
         << " stopped? " << img->stopped() << endl;
    assert( img->stopped() );
#endif

}  // subtitle_thread



void check_video_speed( double& delay, const int step,
                       CMedia* img, CMedia* bimg )
{
    double video_clock, master_clock;

    if ( step < 0 )
    {
        video_clock = img->video_clock();
        master_clock = img->audio_clock();
    }
    else
    {
        video_clock = get_clock(&img->vidclk);
        master_clock = get_master_clock(bimg);
    }

#if 0
    std::cerr  << " VC: " << video_clock
               << " MC: " << master_clock
               << " AC: " << get_clock(&img->audclk)
               << " EC: " << get_clock(&img->extclk)
               << std::endl;
#endif


    double diff = step * ( video_clock - master_clock );

    double absdiff = std::abs(diff);

    if ( absdiff > 1000.0 ) diff = 0.0;

#if __cplusplus >= 201103L
    using std::isnan;
#endif
    if (! isnan(diff) )
    {

        img->avdiff( diff );

        // Skip or repeat the frame. Take delay into account
        //    FFPlay still doesn't "know if this is the best guess."
        if(absdiff < AV_NOSYNC_THRESHOLD) {
            double sdiff = step * diff;
            double sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN,
                                          FFMIN(AV_SYNC_THRESHOLD_MAX,
                                                delay));

            if (sdiff <= -sync_threshold)
            {
                delay = FFMAX(0, delay + sdiff);
            }
            else if (sdiff >= sync_threshold &&
                     delay > AV_SYNC_FRAMEDUP_THRESHOLD)
            {
                delay += sdiff;      // make fps slower
            }
            else if (sdiff >= sync_threshold) {
                delay *= 2;      // make fps repeat frame
            }
        }
    }
}

//
// Main loop used to play video (of any image)
//
void video_thread( PlaybackData* data )
{
    assert( data != NULL );

    mrv::ViewerUI*     uiMain   = data->uiMain;
    assert( uiMain != NULL );
    CMedia* img = data->image;
    assert( img != NULL );



    bool fg = data->fg;

    mrv::ImageView*      view = uiMain->uiView;
    mrv::Timeline*      timeline = uiMain->uiTimeline;
    mrv::ImageBrowser*   browser = uiMain->uiReelWindow->uiBrowser;

    // delete the data (we don't need it anymore)
    delete data;

    int idx = fg ? view->fg_reel() : view->bg_reel();

    mrv::Reel   reel = browser->reel_at( idx );
    if (!reel) return;

    if (!fg)
    {
        mrv::Reel fgreel = browser->reel_at( view->fg_reel() );
        int64_t d = reel->duration();
        if ( fgreel->duration() > d && d > 1 &&
             view->looping() != CMedia::kNoLoop )
        {
            LOG_WARNING( _( "Background reel duration is too short.  "
                            "Looping may not work correctly." ) );
        }
        else if ( fgreel == reel )
        {
            mrv::media fg = view->foreground();
            mrv::media bg = view->background();
            if ( fg && bg )
            {
                CMedia* img  = fg->image();
                CMedia* bimg = bg->image();
                int64_t d = bimg->duration();
                if ( img->duration() > d && d > 1 &&
                     view->looping() != CMedia::kNoLoop )
                {
                    LOG_WARNING( _( "Background image duration is too short.  "
                                    "Looping may not work correctly." ) );
                }
                if ( fabs( img->play_fps() - bimg->play_fps() ) > 0.001 )
                {
                    char buf[256];
                    sprintf( buf, _( "Background image play fps ( %lg ) does "
                                     "not match foreground's ( %lg ).  Looping "
                                     "will not work correctly." ),
                             bimg->play_fps(), img->play_fps() );
                    LOG_WARNING( buf );
                }
            }
        }
    }

    int64_t failed_frame = std::numeric_limits< int64_t >::min();
    int64_t frame;
    {
        // We lock the video mutex to make sure frame was properly updated
        boost::recursive_mutex::scoped_lock lk( img->video_mutex() );

        frame = img->frame();
    }

#ifdef DEBUG_THREADS
    DBG( "ENTER " << (fg ? "FG" : "BG") << " VIDEO THREAD " << img->name() << " stopped? " << img->stopped() << " frame " << frame << " timeline frame "
         << timeline->value() );
#endif


    mrv::Timer timer;
    int delay_counter = 0;
    double fps = img->play_fps();
    timer.setDesiredFrameRate( fps );



    while ( !img->stopped() && view->playback() != CMedia::kStopped )
    {
        img->wait_image();

        // if ( !fg )
        //     img->debug_video_packets( frame, "play", true );

        int step = (int) img->playback();
        if ( step == 0 ) break;


        //DBG( img->name() << " decode image " << frame );
        CMedia::DecodeStatus status = img->decode_video( frame );
        // DBG( img->name() << " decoded image " << frame << " status "
        //      << CMedia::decode_error(status) );

        if ( status != CMedia::kDecodeError )
        {
            int64_t first, last;
            status = check_loop( frame, img, reel, timeline, first, last );
        }

        switch( status )
        {
            // case CMedia::DecodeDone:
            //    continue;
        case CMedia::kDecodeError:
            LOG_ERROR( img->name() << _(" - Decode of image frame ") << frame
                       << _(" returned ") << CMedia::decode_error( status ) );
            break;
        case CMedia::kDecodeLoopEnd:
        case CMedia::kDecodeLoopStart:
        {
            img->remove_to_end( CMedia::kVideoStream );
            
            DBG( img->name() << " VIDEO LOOP END " );
            

            CMedia::Barrier* barrier = img->loop_barrier();
            DBG( img->name() << " LOOP BARRIER " << barrier );
            // LOG_INFO( img->name() << " BARRIER VIDEO WAIT      gen: "
            //           << barrier->generation()
            //           << " count: " << barrier->count()
            //           << " threshold: " << barrier->threshold()
            //           << " used: " << barrier->used() );
            
            // Wait until all threads loop and decode is restarted
            bool ok = barrier->wait();
            
            barrier = img->fg_bg_barrier();
            if ( barrier )
            {
                barrier->wait();
            }

            barrier = img->stereo_barrier();
            if ( barrier )
            {
                barrier->wait();
            }


            if ( img->stopped() ) continue;

            // LOG_INFO( img->name() << " BARRIER PASSED IN VIDEO stopped? "
            //           << img->stopped() << " frame: " << frame );

            DBG( img->name() << " VIDEO LOOP frame: " << frame );


            EndStatus end = handle_loop( frame, step, img, fg, uiMain,
                                         reel, timeline, status );



            DBG( img->name() << " VIDEO LOOP END frame: " << frame
                 << " step " << step );

            continue;
        }
        case CMedia::kDecodeMissingFrame:
            break;
        case CMedia::kDecodeBufferFull:
        default:
            break;
        }

      fps = img->play_fps();

      double delay = 1.0 / fps;

      double diff = 0.0;
      double bgdiff = 0.0;

      // // Calculate video-audio difference
      if ( img->has_audio() && status == CMedia::kDecodeOK )
      {
          check_video_speed( delay, step, img, img );
      }



      timer.setDesiredSecondsPerFrame( delay );
      //timer.setDesiredFrameRate( fps );
      timer.waitUntilNextFrameIsDue();

      img->real_fps( timer.actualFrameRate() );

      img->find_image( frame );

      if ( reel->edl && fg && img->is_left_eye() )
      {
          int64_t f = frame + reel->location(img) - img->first_frame();
          view->frame( f );
      }

      frame += step;
   }



#ifdef DEBUG_THREADS
    DBG( "EXIT " << (fg ? "FG" : "BG") << " VIDEO THREAD "
         << img->name() << " stopped? " << img->stopped()
         << " at " << frame << "  img->frame: " << img->frame() );
   assert( img->stopped() );
#endif

}  // video_thread




void decode_thread( PlaybackData* data )
{
   assert( data != NULL );

   mrv::ViewerUI*     uiMain   = data->uiMain;
   assert( uiMain != NULL );


   CMedia* img = data->image;
   assert( img != NULL );



   bool fg = data->fg;

   mrv::ImageView*      view = uiMain->uiView;
   mrv::Timeline*      timeline = uiMain->uiTimeline;
   mrv::ImageBrowser*   browser = uiMain->uiReelWindow->uiBrowser;
   assert( timeline != NULL );

   // delete the data (we don't need it anymore)
   delete data;

   int idx = fg ? view->fg_reel() : view->bg_reel();

   mrv::Reel   reel = browser->reel_at( idx );
   if (!reel) return;


   int step = (int) img->playback();


   int64_t frame = img->dts();

#ifdef DEBUG_THREADS
   DBG( "ENTER " << (fg ? "FG" : "BG") << " DECODE THREAD " << img->name() << " stopped? " << img->stopped() << " frame " << frame
        << " step " << step );
#endif



   while ( !img->stopped() && view->playback() != CMedia::kStopped )
   {

      if ( img->seek_request() )
      {
         img->do_seek();
         frame = img->dts();
      }


      step = (int) img->playback();
      frame += step;

      CMedia::DecodeStatus status = check_decode_loop( frame, img, reel,
                                                       timeline );


      if ( status != CMedia::kDecodeOK )
      {

          CMedia::Barrier* barrier = img->loop_barrier();
          
          // LOG_INFO( img->name() << " BARRIER DECODE WAIT      gen: "
          //           << barrier->generation()
          //           << " count: " << barrier->count()
          //           << " threshold: " << barrier->threshold()
          //           << " used: " << barrier->used() );

          // Wait until all threads loop and decode is restarted
          barrier->wait();

          // LOG_INFO( img->name() << " BARRIER DECODE LOCK PASS gen: "
          //           << barrier->generation()
          //           << " count: " << barrier->count()
          //           << " threshold: " << barrier->threshold()
          //           << " used: " << barrier->used() );

          // img->clear_packets();

         // Do the looping, taking into account ui state
         //  and return new frame and step.
         // This handle loop has to come after the barrier as decode thread
         // goes faster than video or audio threads

         if ( img->stopped() ) continue;

         EndStatus end = handle_loop( frame, step, img, fg,
                                      uiMain, reel, timeline, status );
// #undef DBG
// #define DBG(x) std::cerr << x << std::endl;
         
         DBG( img->name() << " handle_loop " << end << " frame " << frame
              << " step " << step );
      }


      // If we could not get a frame (buffers full, usually),
      // wait a little.
      while ( !img->frame( frame ) )
      {
          if ( img->stopped() ||
               view->playback() == CMedia::kStopped ) break;
          sleep_ms( 10 );
      }


      // After read, when playing backwards or playing audio files,
      // decode position may be several frames advanced as we buffer
      // multiple frames, so get back the dts frame from image.

      if ( img->has_video() || img->has_audio() )
      {
          frame = img->dts();
      }

   }

#ifdef DEBUG_THREADS
   DBG( "EXIT " << (fg ? "FG" : "BG") << " DECODE THREAD " << img->name() << " stopped? " << img->stopped() << " frame " << img->frame() << "  dts: " << img->dts() );
   assert( img->stopped() );
#endif

}



} // namespace mrv
