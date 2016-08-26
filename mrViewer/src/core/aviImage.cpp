/*
    mrViewer - the professional movie and flipbook playback
    Copyright (C) 2007-2016  Gonzalo Garramuño

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
 * @file   aviImage.cpp
 * @author gga
 * @date   Tue Sep 26 17:54:48 2006
 * 
 * @brief  Read and play an avi/mov/wmv file with audio.
 *         We rely on using the ffmpeg library.
 * 
 */

#include <cstdio>

#define __STDC_LIMIT_MACROS
#include <inttypes.h>

#include <iostream>
#include <algorithm>
#include <limits>
#include <cstring>
using namespace std;


#if !defined(WIN32) && !defined(WIN64)
#  include <arpa/inet.h>
#else
#  include <winsock2.h>    // for htonl
#endif



extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/avstring.h>
#include <libswresample/swresample.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}


#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "core/mrvPlayback.h"
#include "core/aviImage.h"
#include "core/mrvFrameFunctors.h"
#include "core/mrvThread.h"
#include "core/mrvCPU.h"
#include "core/mrvColorSpaces.h"
#include "gui/mrvImageView.h"
#include "gui/mrvIO.h"
#include "mrViewer.h"


namespace 
{
  const char* kModule = "avi";
}


#define IMG_ERROR(x) LOG_ERROR( name() << " - " << x )
#define IMG_INFO(x) LOG_INFO( name() << " - " << x )
#define IMG_WARNING(x) LOG_WARNING( name() << " - " << x )
#define LOG(x) std::cerr << x << std::endl;

//#define DEBUG_STREAM_INDICES
//#define DEBUG_STREAM_KEYFRAMES
//#define DEBUG_DECODE
//#define DEBUG_DECODE_POP_AUDIO
//#define DEBUG_DECODE_AUDIO
//#define DEBUG_SEEK
//#define DEBUG_SEEK_VIDEO_PACKETS
//#define DEBUG_SEEK_AUDIO_PACKETS
//#define DEBUG_SEEK_SUBTITLE_PACKETS
//#define DEBUG_HSEEK_VIDEO_PACKETS
//#define DEBUG_VIDEO_PACKETS
//#define DEBUG_VIDEO_STORES
//#define DEBUG_AUDIO_PACKETS
//#define DEBUG_PACKETS
//#define DEBUG_PACKETS_DETAIL
// #define DEBUG_AUDIO_STORES
// #define DEBUG_STORES_DETAIL
//#define DEBUG_SUBTITLE_STORES
//#define DEBUG_SUBTITLE_RECT



//  in ffmpeg, sizes are in bytes...
#define kMAX_QUEUE_SIZE (15 * 1024 * 1024)
#define kMAX_PACKET_SIZE 50
#define kMAX_AUDIOQ_SIZE (20 * 16 * 1024)
#define kMAX_SUBTITLEQ_SIZE (5 * 30 * 1024)
#define kMIN_FRAMES 25

namespace {
  const unsigned int  kMaxCacheImages = 70;
}

namespace mrv {


fs::path relativePath( const fs::path &path, const fs::path &relative_to )
{
    // create absolute paths
    std::string ps = fs::absolute(path).generic_string();
    std::string rs = fs::absolute(relative_to).generic_string();

#ifdef _WIN32
    std::transform( ps.begin(), ps.end(), ps.begin(), toupper );
    std::transform( rs.begin(), rs.end(), rs.begin(), toupper );
#endif

    fs::path p = ps;
    fs::path r = rs;

    // if root paths are different, return absolute path
    if( p.root_path() != r.root_path() )
    {
        LOG_ERROR( "Path " << p.root_path() << " different than "
                   << r.root_path() );
        return p;
    }

    // initialize relative path
    fs::path result;

    // find out where the two paths diverge
    fs::path::const_iterator itr_path = p.begin();
    fs::path::const_iterator itr_relative_to = r.begin();
    while( *itr_path == *itr_relative_to && itr_path != p.end() && itr_relative_to != r.end() ) {
        ++itr_path;
        ++itr_relative_to;
    }

    // add "../" for each remaining token in relative_to
    while( itr_relative_to != r.end() ) {
        result /= "..";
        ++itr_relative_to;
    }

    // add remaining path
    while( itr_path != p.end() ) {
        result /= *itr_path;
        ++itr_path;
    }

    return result;
}



const char* const kColorRange[] = {
_("Unspecified"),
"MPEG", ///< the normal 219*2^(n-8) "MPEG" YUV ranges
"JPEG", ///< the normal     2^n-1   "JPEG" YUV ranges
};

const char* const kColorSpaces[] = {
    "RGB",
    "BT709",
    _("Unspecified"),
    _("Reserved"),
    "FCC",
    "BT470BG", ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    "SMPTE170M", ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
    "SMPTE240M",
    "YCOCG", ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    "BT2020_NCL", ///< ITU-R BT2020 non-constant luminance system
    "BT2020_CL", ///< ITU-R BT2020 constant luminance system
};

const size_t aviImage::colorspace_index() const
{
    if ( !_av_frame ) return 2; // Unspecified
    aviImage* img = const_cast< aviImage* >( this );
    if ( _colorspace_index < 0 || 
         _colorspace_index >= sizeof( kColorSpaces )/sizeof(char*) )
        img->_colorspace_index = av_frame_get_colorspace(_av_frame);
    return _colorspace_index;
}

const char* const aviImage::colorspace() const
{
    aviImage* img = const_cast< aviImage* >( this );
    return _( kColorSpaces[img->colorspace_index()] );
}

const char* const aviImage::color_range() const
{
    if ( !_av_frame ) return kColorRange[0];
    return kColorRange[av_frame_get_color_range(_av_frame)];
}


aviImage::aviImage() :
  CMedia(),
  _video_index(-1),
  _av_dst_pix_fmt( AV_PIX_FMT_RGB24 ),
  _pix_fmt( VideoFrame::kRGB ),
  _ptype( VideoFrame::kHalf ),
  _av_frame( NULL ),
  _filt_frame( NULL ),
  _video_codec( NULL ),
  _subtitle_ctx( NULL ),
  buffersink_ctx( NULL ),
  buffersrc_ctx( NULL ),
  filter_graph( NULL ),
  _convert_ctx( NULL ),
  _max_images( kMaxCacheImages ),
  _subtitle_codec( NULL )
{
  _gamma = 1.0f;
  _compression = "";

  memset(&_sub, 0, sizeof(AVSubtitle));

}


aviImage::~aviImage()
{

  if ( !stopped() )
    stop();

  image_damage(kNoDamage);

  _video_packets.clear();
  _subtitle_packets.clear();

  flush_video();
  flush_subtitle();

  if ( _convert_ctx )
      sws_freeContext( _convert_ctx );

  if ( filter_graph )
      avfilter_graph_free(&filter_graph);

  if ( _av_frame )
      av_frame_unref( _av_frame );
  if ( _filt_frame )
      av_frame_unref( _filt_frame );

  close_video_codec();
  close_subtitle_codec();

  if ( _av_frame )
      av_frame_free( &_av_frame );

  if ( _filt_frame )
      av_frame_free( &_filt_frame );

  avsubtitle_free( &_sub );

}


bool aviImage::test_filename( const char* buf )
{ 
   AVFormatContext* ctx = NULL;
   int error = avformat_open_input( &ctx, buf, NULL, NULL );
   if ( ctx )
      avformat_close_input( &ctx );

   if ( error < 0 ) return false;

   return true;
}


/*! Test a block of data read from the start of the file to see if it
  looks like the start of an .avi file. This returns true if the 
  data contains RIFF as magic number and a chunk of 'AVI '
  following.
  I tried opening the file as the file test but it was too sensitive and
  was opening mray's map files as sound files.
*/
bool aviImage::test(const boost::uint8_t *data, unsigned len)
{



  if ( len < 12 ) return false;


  unsigned int magic = ntohl( *((unsigned int*)data) );


  if ( magic == 0x000001ba || magic == 0x00000001 )
    {
      // MPEG movie
      return true;
    }
  else if ( magic == 0x1a45dfa3 )
  {
     // Matroska
     return true;
  }
  else if ( magic == 0x3026B275 )
    {
      // WMV movie
      magic = ntohl( *(unsigned int*)(data+4) );
      if ( magic != 0x8E66CF11 ) return false;

      magic = ntohl( *(unsigned int*)(data+8) );
      if ( magic != 0xA6D900AA ) return false;

      magic = ntohl( *(unsigned int*)(data+12) );
      if ( magic != 0x0062CE6C ) return false;
      return true;
    }
  else if ( strncmp( (char*)data, "FLV", 3 ) == 0 )
    {
      // FLV
      return true;
    }
  else if ( ( strncmp( (char*)data, "GIF89a", 6 ) == 0 ) ||
            ( strncmp( (char*)data, "GIF87a", 6 ) == 0 ) )
    {
      // GIF89
      return true;
    }
  else if ( strncmp( (char*)data, ".RMF", 4 ) == 0 )
    {
      // Real Movie
      return true;
    }
  else if ( strncmp( (char*)data, "OggS", 4 ) == 0 ) 
    {
      return true;
    }
  else if ( strncmp( (char*)data, "RIFF", 4 ) == 0 ) 
    {
      // AVI or WAV
      const char* tag = (char*)data + 8;
      if ( strncmp( tag, "AVI ", 4 ) != 0 &&
	   strncmp( tag, "WAVE", 4 ) != 0 &&
	   strncmp( tag, "CDXA", 4 ) != 0 )
	return false;

      return true;
    }
  else if ( strncmp( (char*)data, "ID3", 3 ) == 0 ||
	    (magic & 0xFFE00000) == 0xFFE00000 ||
            (magic == 0x00000000) )
    {
      // MP3
        if ( (magic != 0x00000000) &&
             ((magic & 0xF000) == 0xF000 ||
              (magic & 0xF000) == 0 ) ) return false;
      return true;
    }
  else if ( magic == 0x00000144 )
  {
     // RED ONE camera images
     if ( strncmp( (char*)data+4, "RED1", 4 ) != 0 )
	return false;
     return true;
  } 
  else if ( magic == 0x060E2B34 )
  {
      unsigned int tag = ntohl( *((unsigned int*)data+1) );
      if ( tag != 0x02050101 ) return false;

      tag = ntohl( *((unsigned int*)data+2) );
      if ( tag != 0x0D010201 ) return false;

      return true;
  }
  else if ( strncmp( (char*)data, "YUV4MPEG2", 9 ) == 0 )
  {
      return true;
  }
  else
    {
      // Check for Quicktime
      if ( strncmp( (char*)data+4, "ftyp", 4 ) != 0 && 
	   strncmp( (char*)data+4, "moov", 4 ) != 0 &&
	   strncmp( (char*)data+4, "free", 4 ) != 0 && 
	   strncmp( (char*)data+4, "mdat", 4 ) != 0 &&
	   strncmp( (char*)data+4, "wide", 4 ) != 0 )
	return false;


      return true;
    }

   uint8_t* d = new uint8_t[ len + AVPROBE_PADDING_SIZE ];
   memset( d+len, 0, AVPROBE_PADDING_SIZE );
   memcpy( d, data, len );

   AVProbeData pd = { NULL, d, len };
   int score_max = 0;
   AVInputFormat* ctx = av_probe_input_format3(&pd, 1, &score_max);

   delete [] d;

   // if ( ctx && score_max >= AVPROBE_SCORE_MAX / 4 + 1 )
   if ( ctx && score_max > 10 )
      return true;

  return false;


}

// Returns the current subtitle stream
AVStream* aviImage::get_subtitle_stream() const
{
  return _subtitle_index >= 0 ? _context->streams[ subtitle_stream_index() ] : NULL;
}

// Returns the current video stream
AVStream* aviImage::get_video_stream() const
{
  return _video_index >= 0 ? _context->streams[ video_stream_index() ] : NULL;
}

int aviImage::init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = get_video_stream()->time_base;
    AVRational fr = av_guess_frame_rate(_context, get_video_stream(), NULL);
    enum AVPixelFormat pix_fmts[] = { _video_ctx->pix_fmt, AV_PIX_FMT_NONE };

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        LOG_ERROR( _("No memory to allocate filter graph") );
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             _video_ctx->width, _video_ctx->height, _video_ctx->pix_fmt,
             time_base.num, time_base.den,
             _video_ctx->sample_aspect_ratio.num,
             _video_ctx->sample_aspect_ratio.den);
    if (fr.num && fr.den)
        av_strlcatf(args, sizeof(args), ":frame_rate=%d/%d", fr.num, fr.den);


    LOG_INFO( "args " << args );

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOG_ERROR( _( "Cannot create buffer source" ) );
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOG_ERROR( _("Cannot create buffer sink" ) );
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOG_ERROR( _("Cannot set output pixel format" ) );
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
    {
        LOG_ERROR( _("Error parsing filter description") );
        goto end;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
    {
        LOG_ERROR( _("Error configuring filter graph") );
        goto end;
    }


end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}


void aviImage::subtitle_file( const char* f )
{
    flush_subtitle();

    close_subtitle_codec();

    avfilter_graph_free( &filter_graph );
    filter_graph = NULL;

    if ( _filt_frame )
    {
        av_frame_unref( _filt_frame );
        av_frame_free( &_filt_frame );
    }

    _subtitle_info.clear();
    _subtitle_index = -1;

    if ( f == NULL )
        _subtitle_file.clear();
    else
    {
        std::ostringstream msg;
  

        subtitle_info_t s;
        populate_stream_info( s, msg, _context, _video_ctx, 0 );
        s.has_codec  = false;
        s.bitrate    = 256;
        _subtitle_info.push_back( s );

        _subtitle_file = f;
    
        fs::path sp = _subtitle_file;
        _subtitle_file = sp.filename().string();
        sp = sp.parent_path();

        fs::path p = relativePath( sp, fs::current_path() );

        _filter_description = "subtitles=";
        _subtitle_file = p.generic_string() + '/' + _subtitle_file;

        LOG_INFO( "Current Path " << fs::current_path() );
        LOG_INFO( "Subtitle file " << _subtitle_file );
        _filter_description += _subtitle_file;


        int ret;
        if ( ret = init_filters( _filter_description.c_str() ) < 0 )
        {
            LOG_ERROR( "Could not init filters: ret " << ret
                       << " " << get_error_text(ret) );
            _subtitle_index = -1;
            avfilter_graph_free( &filter_graph );
            filter_graph = NULL;
            return;
        }
        else
        {
            _subtitle_index = 0;
        }

        _filt_frame = av_frame_alloc();
        if ( ! _filt_frame )
        {
            LOG_ERROR( _("Could not allocate filter frame") );
        }
    }

}

bool aviImage::has_video() const
{
  return ( _video_index >= 0 && _video_info[ _video_index ].has_codec );
}


bool aviImage::valid_video() const
{
  // If there's at least one valid video stream, return true
  size_t num_streams = number_of_video_streams();

  bool valid = false;
  for ( size_t i = 0; i < num_streams; ++i )
    {
      if ( _video_info[i].has_codec ) { valid = true; break; }
    }

  return valid;
}

// Opens the video codec associated to the current stream
void aviImage::open_video_codec()
{
  AVStream *stream = get_video_stream();
  if ( stream == NULL ) return;

  AVCodecContext* ictx = stream->codec;

  _video_codec = avcodec_find_decoder( ictx->codec_id );

  _video_ctx = avcodec_alloc_context3(_video_codec);
  int r = avcodec_copy_context(_video_ctx, ictx);
  if ( r < 0 )
  {
      throw _("avcodec_copy_context failed for video"); 
  }


  static int workaround_bugs = 1;
  static enum AVDiscard skip_frame= AVDISCARD_DEFAULT;
  static enum AVDiscard skip_idct= AVDISCARD_DEFAULT;
  static enum AVDiscard skip_loop_filter= AVDISCARD_DEFAULT;
  static int idct = FF_IDCT_AUTO;
  static int error_concealment = 3;

  _video_ctx->codec_id        = _video_codec->id;
  _video_ctx->workaround_bugs = workaround_bugs;
  // _video_ctx->skip_frame= skip_frame;
  // _video_ctx->skip_idct = skip_idct;
  // _video_ctx->skip_loop_filter= skip_loop_filter;
  // _video_ctx->idct_algo = idct;


  if(_video_codec->capabilities & CODEC_CAP_DR1)
     _video_ctx->flags |= CODEC_FLAG_EMU_EDGE;

  double aspect_ratio;
  if ( _video_ctx->sample_aspect_ratio.num == 0 )
    aspect_ratio = 0;
  else
    aspect_ratio = av_q2d( _video_ctx->sample_aspect_ratio ) *
    _video_ctx->width / _video_ctx->height;




  if ( width() > 0 && height() > 0 )
  {
     double image_ratio = (double) width() / (double)height();
     if ( aspect_ratio <= 0.0 ) aspect_ratio = image_ratio;

     if ( image_ratio == aspect_ratio ) _pixel_ratio = 1.0;
     else _pixel_ratio = aspect_ratio / image_ratio;
  }

  AVDictionary* info = NULL;
  av_dict_set(&info, "threads", "2", 0);  // not "auto" nor "4"

  // recounted frames needed for subtitles
  av_dict_set(&info, "refcounted_frames", "1", 0);

  if ( _video_codec == NULL ||
       avcodec_open2( _video_ctx, _video_codec, &info ) < 0 )
    _video_index = -1;


}

void aviImage::close_video_codec()
{
    if ( _video_ctx && _video_index >= 0 )
    {
        avcodec_close( _video_ctx );
        avcodec_free_context( &_video_ctx );
    }
}


// Flush video buffers
void aviImage::flush_video()
{
    SCOPED_LOCK( _mutex );
    if ( _video_ctx && _video_index >= 0 )
    {
	avcodec_flush_buffers( _video_ctx );
    }
}


void aviImage::clear_cache()
{
    {
        SCOPED_LOCK( _mutex );
        _images.clear();
    }

    clear_stores();
}

/// VCR play (and cache frames if needed) sequence
void aviImage::play( const Playback dir, mrv::ViewerUI* const uiMain,
		     const bool fg )
{
   CMedia::play( dir, uiMain, fg );
}

bool aviImage::is_cache_filled( int64_t frame )
{
    return in_video_store( frame );
}

// Seek to the requested frame
bool aviImage::seek_to_position( const boost::int64_t frame )
{


#ifdef DEBUG_SEEK
    LOG_INFO( "BEFORE SEEK:  D: " << _dts << " E: " << _expected );
#endif


    // double frac = ( (double) (frame - _frameStart) / 
    // 		   (double) (_frameEnd - _frameStart) );
    // boost::int64_t offset = boost::int64_t( _context->duration * frac );
    // if ( _context->start_time != AV_NOPTS_VALUE )
    //    offset += _context->start_time;

    // static const AVRational base = { 1, AV_TIME_BASE };
    // boost::int64_t min_ts = std::numeric_limits< boost::int64_t >::max();

    if ( _context == NULL ) return false;
    

    bool skip = false;
    bool got_audio = !has_audio();
    bool got_video = !has_video();
    bool got_subtitle = !has_subtitle();

    int flag = AVSEEK_FLAG_BACKWARD;

    if ( playback() == kStopped &&
         (got_video || in_video_store( frame )) &&
         (got_audio || in_audio_store( frame + _audio_offset )) &&
         (got_subtitle || in_subtitle_store( frame )) )
    {
        skip = true;
    }

    // With frame and reverse playback, we often do not get the current
    // frame.  So we search for frame - 1.
    boost::int64_t start = frame;

    if ( !skip ) --start;
    if ( playback() == kBackwards ) --start;


    boost::int64_t offset = boost::int64_t( double(start) * AV_TIME_BASE
                                            / fps() );

    if ( offset < 0 ) offset = 0;

    int ret = av_seek_frame( _context, -1, offset, flag );
    if (ret < 0)
    {
        IMG_ERROR( _("Could not seek to frame ") << frame
                   << N_(": ") << get_error_text(ret) );
        return false;
    }

    if ( _acontext )
    {
        offset = boost::int64_t( double(start + _audio_offset) * AV_TIME_BASE
                                 / fps() );
        if ( offset < 0 ) offset = 0;

        int ret = av_seek_frame( _acontext, -1, offset, flag );

        if (ret < 0)
        {
            IMG_ERROR( _("Could not seek to frame ") << frame 
                   << N_(": ") << get_error_text(ret) );
            return false;
        }
    }


    // Skip the seek packets when playback is stopped (scrubbing)
    if ( skip )
    {
        boost::int64_t f = frame;
        if ( f > _frame_end ) f = _frame_end;
        boost::int64_t dts = queue_packets( f, false, got_video,
                                            got_audio, got_subtitle );
        _dts = _adts = dts;
        _expected = _expected_audio = _dts;
        _seek_req = false;
        return true;
    }

    
    boost::int64_t vpts = 0, apts = 0, spts = 0;

    mrv::PacketQueue::Mutex& vpm = _video_packets.mutex();
    SCOPED_LOCK( vpm );

    mrv::PacketQueue::Mutex& apm = _audio_packets.mutex();
    SCOPED_LOCK( apm );

    mrv::PacketQueue::Mutex& spm = _subtitle_packets.mutex();
    SCOPED_LOCK( spm );

    if ( !got_video ) {
        vpts = frame2pts( get_video_stream(), start );
    }

    if ( !got_audio ) {
        if ( _acontext )
        {
            apts = frame2pts( get_audio_stream(), start + 1 + _audio_offset );
            DBG( "++++++++ SET APTS " << apts << " START " << start
                 << " offset " << _audio_offset );
        }
        else
        {
            apts = frame2pts( get_audio_stream(), start + 1);
        }
    }

    if ( !got_subtitle ) {
        spts = frame2pts( get_subtitle_stream(), start );
    }

    if ( !_seek_req && playback() == kBackwards )
    {
        if ( !got_video )    _video_packets.preroll(vpts);
        if ( !got_audio )    _audio_packets.preroll(apts);
        if ( !got_subtitle ) _subtitle_packets.preroll(spts);
    }
    else
    {
        DBG( "SEEK REQ: " << _seek_req << " GOT AUDIO: " << got_audio
             << " APTS: " << apts );
        if ( !got_video )    _video_packets.seek_begin(vpts);
        if ( !got_audio && apts >= 0 )    _audio_packets.seek_begin(apts);
        if ( !got_subtitle ) _subtitle_packets.seek_begin(spts);
    }

#ifdef DEBUG_SEEK_VIDEO_PACKETS
    debug_video_packets(start, "BEFORE SEEK", true);
#endif

#ifdef DEBUG_SEEK_AUDIO_PACKETS
    debug_audio_packets(start, "BEFORE SEEK", true);
#endif


    boost::int64_t dts = queue_packets( frame, true, got_video,
                                        got_audio, got_subtitle );


    _dts = _adts = dts;
    assert( _dts >= first_frame() && _dts <= last_frame() );

    _expected = _expected_audio = dts + 1;
    _seek_req = false;


#ifdef DEBUG_SEEK
    LOG_INFO( "AFTER SEEK:  D: " << _dts << " E: " << _expected );
#endif

#ifdef DEBUG_VIDEO_STORES
    debug_video_stores(frame, "AFTER SEEK");
#endif

#ifdef DEBUG_AUDIO_STORES
    debug_audio_stores(frame, "AFTER SEEK");
#endif

#ifdef DEBUG_SEEK_VIDEO_PACKETS
    debug_video_packets(frame, "AFTER SEEK", true);
#endif

#ifdef DEBUG_SEEK_AUDIO_PACKETS
    debug_audio_packets(frame, "AFTER SEEK", true);
#endif


    return true;
}


mrv::image_type_ptr aviImage::allocate_image( const boost::int64_t& frame,
					      const boost::int64_t& pts
)
{
    return mrv::image_type_ptr( new image_type( frame,
                                                width(), 
                                                height(), 
                                                (unsigned short) _num_channels,
                                                _pix_fmt,
                                                _ptype,
                                                _av_frame->repeat_pict,
                                                pts ) );
}


void aviImage::store_image( const boost::int64_t frame, 
			    const boost::int64_t pts )
{

  SCOPED_LOCK( _mutex );

  AVStream* stream = get_video_stream();
  assert( stream != NULL );

  mrv::image_type_ptr image;
  try {

      image = allocate_image( frame, boost::int64_t( double(pts) * 
                                                     av_q2d( _video_ctx->time_base )
                              )
      );
  } catch ( const std::exception& e )
  {
      LOG_ERROR( "Problem allocating image " << e.what() );
  }


  if ( ! image )
  {
      IMG_ERROR( "No memory for video frame" );
      IMG_ERROR( "Audios #" << _audio.size() );
      IMG_ERROR( "Videos #" << _images.size() );
      return;
  }

  AVFrame output;
  boost::uint8_t* ptr = (boost::uint8_t*)image->data().get();

  unsigned int w = width();
  unsigned int h = height();

  // Fill the fields of AVPicture output based on _av_dst_pix_fmt
  // avpicture_fill( &output, ptr, _av_dst_pix_fmt, w, h );
  av_image_fill_arrays( output.data, output.linesize, ptr, _av_dst_pix_fmt,
                        w, h, 1);

  AVPixelFormat fmt = _video_ctx->pix_fmt;

  // We handle all cases directly except YUV410 and PAL8
  _convert_ctx = sws_getCachedContext(_convert_ctx,
                                      _video_ctx->width, 
                                      _video_ctx->height,
                                      fmt, w, h,
                                      _av_dst_pix_fmt, 0, 
                                      NULL, NULL, NULL);

  if ( _convert_ctx == NULL )
  {
      IMG_ERROR( _("Could not get image conversion context.") );
      return;
  }

  sws_scale(_convert_ctx, _av_frame->data, _av_frame->linesize,
            0, _video_ctx->height, output.data, output.linesize);

  if ( _av_frame->interlaced_frame )
    _interlaced = ( _av_frame->top_field_first ? 
		    kTopFieldFirst : kBottomFieldFirst );



  if ( _images.empty() || _images.back()->frame() < frame )
  {
     _images.push_back( image );
  }
  else
  {
     video_cache_t::iterator at = std::lower_bound( _images.begin(), 
						    _images.end(),
						    frame, 
						    LessThanFunctor() );

     // Avoid storing duplicate frames, replace old frame with this one
     if ( at != _images.end() )
     {
	if ( (*at)->frame() == frame )
	{
	   at = _images.erase(at);
	}
     }

     _images.insert( at, image );
  }
 
}

CMedia::DecodeStatus
aviImage::decode_video_packet( boost::int64_t& ptsframe, 
			       const boost::int64_t frame, 
			       const AVPacket& p
			       )
{
   AVPacket pkt = p;

  AVStream* stream = get_video_stream();
  assert( stream != NULL );

  int got_pict = 0;

  bool eof_found = false;
  bool eof = false;
  if ( pkt.data == NULL ) {
      eof = true;
      pkt.size = 0;
  }

  while( pkt.size > 0 || pkt.data == NULL )
  {
     int err = avcodec_decode_video2( _video_ctx, _av_frame, &got_pict, 
				      &pkt );


     if ( got_pict ) {
         ptsframe = av_frame_get_best_effort_timestamp( _av_frame );

         if ( ptsframe == AV_NOPTS_VALUE )
         {
             if ( _av_frame->pkt_pts != AV_NOPTS_VALUE )
                 ptsframe = _av_frame->pkt_pts;
             else if ( _av_frame->pkt_dts != AV_NOPTS_VALUE )
                 ptsframe = _av_frame->pkt_dts;
         }

         _av_frame->pts = ptsframe;

         // Turn PTS into a frame
         if ( ptsframe == AV_NOPTS_VALUE )
         {
             ptsframe = get_frame( stream, pkt );
             if ( ptsframe == AV_NOPTS_VALUE ) ptsframe = frame;
         }
         else
         {
             ptsframe = pts2frame( stream, ptsframe );
         }


         if ( filter_graph && _subtitle_index >= 0 )
         {
             /* push the decoded frame into the filtergraph */
             if (av_buffersrc_add_frame_flags(buffersrc_ctx, _av_frame,
                                              AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                 LOG_ERROR( _("Error while feeding the filtergraph") );
                 close_subtitle_codec();
                 break;
             }

             int ret = av_buffersink_get_frame(buffersink_ctx, _filt_frame);
             if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                 break;
             if (ret < 0)
             {
                 LOG_ERROR( "av_buffersink_get frame failed" );
                 close_subtitle_codec();
                 return kDecodeError;
             }

             av_frame_unref( _av_frame );
             _av_frame = av_frame_clone( _filt_frame );
             if (!_av_frame )
             {
                 LOG_ERROR( _("Could not clone subtitle frame") );
                 close_subtitle_codec();
                 return kDecodeError;
             }
         }


        if ( eof )
        {
            eof_found = true;
            store_image( ptsframe, pkt.dts );
            av_frame_unref(_av_frame);
            av_frame_unref(_filt_frame);
            continue;
        }

	return kDecodeOK;
     }


     if ( err < 0 ) {
         IMG_ERROR( "avcodec_decode_video2: " << get_error_text(err) );
         return kDecodeError;
     }

     if ( err == 0 ) return kDecodeLoopEnd;


     pkt.size -= err;
     pkt.data += err;
  }

  return kDecodeMissingFrame;
}



// Decode the image
CMedia::DecodeStatus
aviImage::decode_image( const boost::int64_t frame, AVPacket& pkt )
{
  boost::int64_t ptsframe = frame;
  DecodeStatus status = decode_video_packet( ptsframe, frame, pkt );

  if ( status == kDecodeOK )
  {
      store_image( ptsframe, pkt.dts );
      av_frame_unref(_av_frame);
      av_frame_unref(_filt_frame);
  }
  else if ( status == kDecodeError )
  {
       char ftype = av_get_picture_type_char( _av_frame->pict_type );
       if ( ptsframe >= first_frame() && ptsframe <= last_frame() )
           IMG_WARNING( _("Could not decode video frame ") << ptsframe 
                        << _(" type ") << ftype << " pts: " 
                       << (pkt.pts == AV_NOPTS_VALUE ?
                           -1 : pkt.pts ) << " dts: " << pkt.dts
		      << " data: " << (void*)pkt.data);
  }

  return status;
}

void aviImage::clear_packets()
{

#ifdef DEBUG_AUDIO_PACKETS
   cerr << "+++++++++++++ CLEAR VIDEO/AUDIO PACKETS " << _frame 
	<< " expected: " << _expected << endl;
#endif

   if ( _right_eye ) _right_eye->clear_packets();

  _video_packets.clear();
  _audio_packets.clear();
  _subtitle_packets.clear();

  _audio_buf_used = 0;
}


//
// Limit the video store to approx. max frames images on each side.
// We have to check both where frame is as well as where _dts is.
//
void aviImage::limit_video_store(const boost::int64_t frame)
{
    SCOPED_LOCK( _mutex );

  boost::int64_t first, last;

  switch( playback() )
  {
      case kBackwards:
          first = frame - max_video_frames();
          last  = frame;
          if ( _dts < first ) first = _dts;
          break;
      case kForwards:
          first = frame - max_video_frames();
          last  = frame + max_video_frames();
          if ( _dts > last )   last  = _dts;
          if ( _dts < first )  first = _dts;
          break;
      default:
          first = frame - max_video_frames();
          last  = frame + max_video_frames();
          if ( _dts > last )   last = _dts;
          if ( _dts < first ) first = _dts;
          break;
  }

  if ( _images.empty() ) return;



  video_cache_t::iterator end = _images.end();
  _images.erase( std::remove_if( _images.begin(), end,
        			 NotInRangeFunctor( first, last ) ), end );


}

//
// Limit the subtitle store to approx. max frames images on each side.
// We have to check both where frame is as well as where _dts is.
//
void aviImage::limit_subtitle_store(const boost::int64_t frame)
{

  boost::int64_t first, last;

  switch( playback() )
    {
    case kBackwards:
       first = frame - (boost::int64_t)fps() * 2;
       last  = frame;
       if ( _dts < first ) first = _dts;
      break;
    case kForwards:
      first = frame;
      last  = frame + (boost::int64_t)fps() * 2;
      if ( _dts > last )   last = _dts;
      break;
    default:
       first = frame - (boost::int64_t)fps() * 2;
       last  = frame + (boost::int64_t)fps() * 2;
      break;
    }

  subtitle_cache_t::iterator end = _subtitles.end();
  _subtitles.erase( std::remove_if( _subtitles.begin(), end,
				    NotInRangeFunctor( first, last ) ), end );



}

// Opens the subtitle codec associated to the current stream
void aviImage::open_subtitle_codec()
{

  AVStream* stream = get_subtitle_stream();
  if (!stream) return;

  AVCodecContext* ictx = stream->codec;
  _subtitle_codec = avcodec_find_decoder( ictx->codec_id );

  _subtitle_ctx = avcodec_alloc_context3(_subtitle_codec);
  int r = avcodec_copy_context(_subtitle_ctx, ictx);
  if ( r < 0 )
  {
      LOG_ERROR( _("avcodec_copy_context failed for subtitle") );
      return;
  }

  static int workaround_bugs = 1;
  static enum AVDiscard skip_frame= AVDISCARD_DEFAULT;
  static enum AVDiscard skip_idct= AVDISCARD_DEFAULT;
  static enum AVDiscard skip_loop_filter= AVDISCARD_DEFAULT;
  static int error_concealment = 3;

  _subtitle_ctx->idct_algo         = FF_IDCT_AUTO;
  _subtitle_ctx->workaround_bugs = workaround_bugs;
  _subtitle_ctx->skip_frame= skip_frame;
  _subtitle_ctx->skip_idct= skip_idct;
  _subtitle_ctx->skip_loop_filter= skip_loop_filter;
  _subtitle_ctx->error_concealment= error_concealment;

  AVDictionary* info = NULL;
  if ( _subtitle_codec == NULL || 
       avcodec_open2( _subtitle_ctx, _subtitle_codec, &info ) < 0 )
    _subtitle_index = -1;
}

void aviImage::close_subtitle_codec()
{
    if ( _subtitle_ctx && _subtitle_index >= 0 )
    {
        avcodec_close( _subtitle_ctx );
        avcodec_free_context( &_subtitle_ctx );
    }
}

bool aviImage::find_subtitle( const boost::int64_t frame )
{

  SCOPED_LOCK( _subtitle_mutex );

  subtitle_cache_t::iterator end = _subtitles.end();
  subtitle_cache_t::iterator i = _subtitles.begin();

  _subtitle.reset();
  for ( ; i != end; ++i )
  {
     if ( frame >= (*i)->frame() && frame <= (*i)->frame() + (*i)->repeat() )
     {
	_subtitle = *i;
     }
  }


  image_damage( image_damage() | kDamageSubtitle );

  limit_subtitle_store( frame );

  return false;
}

bool aviImage::find_image( const boost::int64_t frame )
{

    if ( _right_eye && (playback() == kStopped || playback() == kSaving) )
        _right_eye->find_image( frame );

#ifdef DEBUG_VIDEO_PACKETS
  debug_video_packets(frame, "find_image");
#endif

#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "find_image");
#endif

  _frame = frame;

  if ( !has_video() ) 
  {
      _video_pts   = frame  / _fps;
      _video_clock = double(av_gettime_relative()) / 1000000.0;

      update_video_pts(this, _video_pts, 0, 0);
      return true;
  }

  {

    SCOPED_LOCK( _mutex );

    video_cache_t::iterator end = _images.end();
    video_cache_t::iterator i;

    if ( playback() == kBackwards )
    {
       i = std::upper_bound( _images.begin(), end, 
			     frame, LessThanFunctor() );
    }
    else
    {
       i = std::lower_bound( _images.begin(), end, 
			     frame, LessThanFunctor() );
    }

    if ( i != end && *i )
      {
	_hires = *i;

        boost::int64_t distance = frame - _hires->frame();


        if ( distance > _hires->repeat() )
        {
            boost::int64_t first = (*_images.begin())->frame();
            video_cache_t::iterator end = std::max_element( _images.begin(), 
                                                            _images.end() );
            boost::int64_t last  = (*end)->frame();
            boost::uint64_t diff = last - first + 1;
            IMG_ERROR( _("Video Sync master frame ") << frame 
                       << " != " << _hires->frame()
                       << _(" video frame, cache ") << first << "-" << last
                       << " (" << diff << _(") cache size: ") << _images.size()
                       << " dts: " << _dts );
            // 	debug_video_stores(frame);
            // 	debug_video_packets(frame);
        }
      }
    else
    {
	// Hmm... no close image was found.  If we have some images in
	// cache, we choose the last one in it.  This avoids problems if
	// the last frame is the one with problem.
	// If not, we fail.
	
	if ( ! _images.empty() )
	  {
	    _hires = _images.back();

	    if ( !filter_graph &&
                 _hires->frame() != frame && 
		 abs(frame - _hires->frame() ) < 10 )
            {
	       IMG_WARNING( _("find_image: frame ") << frame 
			    << _(" not found, choosing ") << _hires->frame() 
			    << _(" instead") );
               // debug_video_packets( frame, "find_image", false );
            }
	  }
	else
	  {
              IMG_ERROR( _("find_image: frame ") << frame << _(" not found") );
              return false;
	  }
      }



    // Limit (clean) the video store as we play it
    limit_video_store( frame );

    _video_pts   = frame  / _fps; //av_q2d( get_video_stream()->avg_frame_rate );
    _video_clock = double(av_gettime_relative()) / 1000000.0;

    update_video_pts(this, _video_pts, 0, 0);

  }  // release lock


  refresh();

  return true;
}


int aviImage::subtitle_stream_index() const
{
  assert( _subtitle_index >= 0 && 
	  _subtitle_index < int(_subtitle_info.size()) );
  return _subtitle_info[ _subtitle_index ].stream_index;
}

/** 
 * Change video stream
 * 
 * @param x video stream number or -1 for no stream.
 */
void aviImage::video_stream( int x )
{
  if ( x < -1 || unsigned(x) >= number_of_video_streams() ) {
     IMG_ERROR( _("Invalid video stream ") << x );
    return;
  }

  _video_index  = x;
  _num_channels = 0;
  if ( x < 0 ) return;

  static AVPixelFormat fmt[] = { AV_PIX_FMT_BGR24, AV_PIX_FMT_BGR32, AV_PIX_FMT_NONE };
  AVPixelFormat* fmts = fmt;


  if ( supports_yuv() )
    {
       static AVPixelFormat fmts2[] = { AV_PIX_FMT_RGB24, AV_PIX_FMT_RGB32,
				      AV_PIX_FMT_BGR24, AV_PIX_FMT_BGR32,
				      AV_PIX_FMT_YUV444P,
				      AV_PIX_FMT_YUV422P,
				      AV_PIX_FMT_YUV420P,
				      AV_PIX_FMT_NONE };
       fmts = fmts2;

//       mask |= ( (1 << AV_PIX_FMT_YUVA420P) | (1 << AV_PIX_FMT_YUV444P) | 
// 		(1 << AV_PIX_FMT_YUV422P) | (1 << AV_PIX_FMT_YUV420P) );
    }

  AVStream* stream = get_video_stream();
  AVCodecContext* ctx = stream->codec;

  int has_alpha = ( ( ctx->pix_fmt == AV_PIX_FMT_RGBA    ) |
		    ( ctx->pix_fmt == AV_PIX_FMT_ABGR    ) |
		    ( ctx->pix_fmt == AV_PIX_FMT_ARGB    ) |
		    ( ctx->pix_fmt == AV_PIX_FMT_RGB32   ) |
		    ( ctx->pix_fmt == AV_PIX_FMT_RGB32_1 ) |
		    ( ctx->pix_fmt == AV_PIX_FMT_PAL8    ) | 
		    ( ctx->pix_fmt == AV_PIX_FMT_BGR32   ) | 
		    ( ctx->pix_fmt == AV_PIX_FMT_BGR32_1 ) );

  _av_dst_pix_fmt = avcodec_find_best_pix_fmt_of_list( fmts, 
						       ctx->pix_fmt,
						       has_alpha, NULL );


  _num_channels = 0;
  _layers.clear();
		
  rgb_layers();
  lumma_layers();

  if ( _av_dst_pix_fmt == AV_PIX_FMT_RGBA ||
       _av_dst_pix_fmt == AV_PIX_FMT_BGRA ||
       _av_dst_pix_fmt == AV_PIX_FMT_YUVA420P) alpha_layers();

  if (ctx->lowres) {
      ctx->flags |= CODEC_FLAG_EMU_EDGE;
  }

  _ptype = VideoFrame::kByte;
  unsigned int w = ctx->width;

  _colorspace_index = ctx->colorspace;

  switch( _av_dst_pix_fmt )
  {
      case AV_PIX_FMT_RGBA64BE:
          _ptype = VideoFrame::kShort;
          _pix_fmt = VideoFrame::kRGBA; break;
      case AV_PIX_FMT_RGBA64LE:
          _ptype = VideoFrame::kShort;
          _pix_fmt = VideoFrame::kRGBA; break;
      case AV_PIX_FMT_BGRA64BE:
      case AV_PIX_FMT_BGRA64LE:
          _ptype = VideoFrame::kShort;
          _pix_fmt = VideoFrame::kBGRA; break;
      case AV_PIX_FMT_BGR24:
          _pix_fmt = VideoFrame::kBGR; break;
      case AV_PIX_FMT_BGRA:
          _pix_fmt = VideoFrame::kBGRA; break;
      case AV_PIX_FMT_RGB24:
          _pix_fmt = VideoFrame::kRGB; break;
      case AV_PIX_FMT_RGBA:
          _pix_fmt = VideoFrame::kRGBA; break;
      case AV_PIX_FMT_YUV444P:
          if ( ctx->colorspace == AVCOL_SPC_BT709 )
              _pix_fmt = VideoFrame::kITU_709_YCbCr444; 
          else
              _pix_fmt = VideoFrame::kITU_601_YCbCr444; 
          break;
      case AV_PIX_FMT_YUV422P:
          if ( ctx->colorspace == AVCOL_SPC_BT709 )
              _pix_fmt = VideoFrame::kITU_709_YCbCr422;
          else
              _pix_fmt = VideoFrame::kITU_601_YCbCr422;
          break;
      case AV_PIX_FMT_YUV420P:
          if ( ctx->colorspace == AVCOL_SPC_BT709 )
              _pix_fmt = VideoFrame::kITU_709_YCbCr420;
          else
              _pix_fmt = VideoFrame::kITU_601_YCbCr420;
          break;
      case AV_PIX_FMT_YUVA420P:
          if ( ctx->colorspace == AVCOL_SPC_BT709 )
              _pix_fmt = VideoFrame::kITU_709_YCbCr420A;
          else
              _pix_fmt = VideoFrame::kITU_601_YCbCr420A;
          break;
      default:
          IMG_ERROR( _("Unknown destination video frame format: ") 
                     << _av_dst_pix_fmt << " " 
                     << av_get_pix_fmt_name( _av_dst_pix_fmt ) );

          _pix_fmt = VideoFrame::kBGRA; break;
  }



}

bool aviImage::readFrame(int64_t & pts)
{
    AVPacket packet;

    int got_video = 0;

    while (! got_video)
    {
        int r = av_read_frame(_context, &packet);

        if (r < 0)
        {
            packet.data = 0;
            packet.size = 0;
        }

        if ( video_stream_index() == packet.stream_index)
        {
            if (avcodec_decode_video2( _video_ctx, _av_frame, &got_video,
                                       &packet) <= 0)
            {
                break;
            }
        }
    }

    pts = av_frame_get_best_effort_timestamp( _av_frame );

    if ( pts == AV_NOPTS_VALUE )
    {
        if ( _av_frame->pkt_pts != AV_NOPTS_VALUE )
            pts = _av_frame->pkt_pts;
        else if ( _av_frame->pkt_dts != AV_NOPTS_VALUE )
            pts = _av_frame->pkt_dts;
    }

    AVRational q = { 1, AV_TIME_BASE };

    pts = av_rescale_q( pts,
                        get_video_stream()->time_base,
                        q );

    return got_video;
}

int aviImage::video_stream_index() const
{
    assert( _video_index >= 0 && 
            _video_index < int(_video_info.size()) );
    return _video_info[ _video_index ].stream_index;
}


// Analyse streams and set input values
void aviImage::populate()
{

    std::ostringstream msg;
  
    if ( _context == NULL ) return;


  
    // Iterate through all the streams available
    for( unsigned i = 0; i < _context->nb_streams; ++i ) 
    {
        // Get the codec context
        const AVStream* stream = _context->streams[ i ];

        if ( stream == NULL ) continue;

        const AVCodecContext* ctx = stream->codec;
        if ( ctx == NULL ) continue;

      
        // Determine the type and obtain the first index of each type
        switch( ctx->codec_type ) 
	{
            // We ignore attachments for now.  
            case AVMEDIA_TYPE_ATTACHMENT:
                {
                    continue;
                }
                // We ignore data tracks for now.  Data tracks are, for example,
                // the timecode track in quicktimes.
            case AVMEDIA_TYPE_DATA:
                {
                    continue;
                }
            case AVMEDIA_TYPE_VIDEO:
                {
		 
                    video_info_t s;
                    populate_stream_info( s, msg, _context, ctx, i );
                    s.has_b_frames = ( ctx->has_b_frames != 0 );
                    s.fps          = calculate_fps( stream );
                    if ( av_get_pix_fmt_name( ctx->pix_fmt ) )
                        s.pixel_format = av_get_pix_fmt_name( ctx->pix_fmt );
                    _video_info.push_back( s );
                    if ( _video_index < 0 && s.has_codec )
                    {
                        video_stream( 0 );
                        int w = ctx->width;
                        int h = ctx->height;
                        image_size( w, h );
                    }
                    break;
                }
            case AVMEDIA_TYPE_AUDIO:
                {
		 
                    audio_info_t s;
                    populate_stream_info( s, msg, _context, ctx, i );
		 
                    s.channels   = ctx->channels;
                    s.frequency  = ctx->sample_rate;
                    s.bitrate    = calculate_bitrate( ctx );
		 
                    if ( stream->metadata )
                    {
                        AVDictionaryEntry* lang = av_dict_get(stream->metadata, 
                                                              "language", NULL, 0);
                        if ( lang && lang->value )
                            s.language = lang->value;
                    }
                    else
                    {
                        s.language = "und";
                    }

                    const char* fmt = av_get_sample_fmt_name( ctx->sample_fmt );
                    if ( fmt ) s.format = fmt;

                    _audio_info.push_back( s );
                    if ( _audio_index < 0 && s.has_codec )
                        _audio_index = 0;
                    break;
                }
            case AVMEDIA_TYPE_SUBTITLE:
                {
		 
                    subtitle_info_t s;
                    populate_stream_info( s, msg, _context, ctx, i );
                    s.bitrate    = calculate_bitrate( ctx );
                    AVDictionaryEntry* lang = av_dict_get(stream->metadata, 
                                                          "language", NULL, 0);
                    if ( lang && lang->value )
                        s.language = lang->value;
                    _subtitle_info.push_back( s );
                    if ( _subtitle_index < 0 )
                        _subtitle_index = 0;
                    break;
                }
            default:
                {
                    const char* stream = stream_type( ctx );
                    msg << _("\n\nNot a known stream type for stream #") 
                        << i << (", type ") << stream;
                    break;
                }
	}
    }


    if ( msg.str().size() > 0 )
    {
        LOG_ERROR( filename() << msg.str() );
    }

    if ( _video_index < 0 && _audio_index < 0 )
    {
        LOG_ERROR( filename() << _(" No audio or video stream in file") );
        return;
    }

    // Open these video and audio codecs
    if ( has_video() )    open_video_codec();
    if ( has_audio() )    open_audio_codec();
    if ( has_subtitle() ) open_subtitle_codec();


    // Configure video input properties
    AVStream* stream = NULL;

    if ( has_video() )
    {
        stream = get_video_stream();
    }
    else if ( has_audio() )
    {
        stream = get_audio_stream();
    }
    else
    {
        return;  // no stream detected
    }

    _orig_fps = _fps = _play_fps = calculate_fps( stream );

  

#ifdef DEBUG_STREAM_INDICES
    debug_stream_index( stream );
#elif defined(DEBUG_STREAM_KEYFRAMES)
    debug_stream_keyframes( stream );
#endif


    //
    // Calculate frame start and frame end if possible
    //

    _frameStart = 1;

    if ( _context->start_time != AV_NOPTS_VALUE )
    {
        _frameStart = boost::int64_t( ( _fps * ( double )_context->start_time / 
                                        ( double )AV_TIME_BASE ) ) + 1;
    }
    else
    {
        double start = std::numeric_limits< double >::max();
        if ( has_video() )
	{
            start = _video_info[ _video_index ].start;
	}
      
        if ( has_audio() )
	{
            double d = _audio_info[ _audio_index ].start;
            if ( d < start ) start = d;
	}

        _frameStart = (boost::int64_t)start;
    }

    _frame_start = _frame = _frameStart;

    //
    // BUG FIX for ffmpeg bugs with some codecs/containers.
    //
    // using context->start_time and context->duration should be enough,
    // but with that ffmpeg often reports a frame that cannot be decoded
    // with some codecs like divx.
    //
    int64_t duration;
    if ( _context->duration > 0 )
    {
        duration = int64_t( (_fps * ( double )(_context->duration) / 
                             ( double )AV_TIME_BASE ) + 0.5 );
    }
    else
    {
        double length = 0;

        if ( has_video() )
        {
            length = _video_info[ _video_index ].duration;
        }
      
        if ( has_audio() )
        {
            double d = _audio_info[ _audio_index ].duration;
            if ( d > length ) length = d;
        }

        if ( length > 0 )
        {
            duration = boost::int64_t( length * _fps + 1 );
        }
        else
        {
            if ( stream->nb_frames != 0 )
                duration = stream->nb_frames;
            else {
                // As a last resort, count the frames manually.
                int64_t pts = 0;

                duration = 0; // GIF89
                while ( readFrame(pts) )
                    ++duration;

                av_seek_frame( _context,
                               video_stream_index(),
                               0,
                               AVSEEK_FLAG_BACKWARD);
            }
        }
    }

    _frameEnd = _frameStart + duration - 1;
    _frame_end = _frameEnd;

    _frame_offset = 0;

  
    boost::int64_t dts = _frameStart;
 
    unsigned audio_bytes = 0;
    unsigned bytes_per_frame = audio_bytes_per_frame();

    if ( has_video() || has_audio() )
    {

        // Loop until we get first frame
        AVPacket pkt = {0};
        // Clear the packet
        av_init_packet( &pkt );
        pkt.size = 0;
        pkt.data = NULL;

        int force_exit = 0;
        bool eof = false;
        short counter = 0;
        bool got_audio = ! has_audio();
        bool got_video = ! has_video();
        while( !got_video || !got_audio )
	{
            // Hack to exit loop if got_video or got_audio fails
            ++force_exit;
            if ( force_exit == 200 ) break;

            int error = av_read_frame( _context, &pkt );
            if ( error < 0 )
            {
                int err = _context->pb ? _context->pb->error : 0;
                if ( err != 0 )
                {
                    IMG_ERROR("populate: Could not read frame 1 error: "
                              << strerror(err) );
                    break;
                }
            }
	  
            if ( has_video() && pkt.stream_index == video_stream_index() )
            {
                if ( !got_video )
                {
                    DecodeStatus status = decode_image( _frameStart, pkt ); 
                    if ( status == kDecodeOK )
                    {
                        got_video = true;
                    }
                    else
                    {
                        ++_frame_offset;
                        continue;
                    }
                }
                else
                {
                    _video_packets.push_back( pkt );
                    continue;
                }
            }
            else
                if ( has_audio() && pkt.stream_index == audio_stream_index() )
                {
                    boost::int64_t pktframe = get_frame( get_audio_stream(), 
                                                         pkt );
                    _adts = pktframe;

                    if ( playback() == kBackwards )
                    {
                        // Only add packet if it comes before first frame
                        if ( pktframe >= first_frame() )  
                            _audio_packets.push_back( pkt );
                        if ( !has_video() && pktframe < dts ) dts = pktframe;
                    }
                    else
                    {
                        _audio_packets.push_back( pkt );
                        if ( !has_video() && pktframe > dts ) dts = pktframe;
                    }

                    if ( !got_audio )
                    {
                        if ( pktframe > _frameStart ) got_audio = true;
                        else if ( pktframe == _frameStart )
                        {
                            audio_bytes += pkt.size;
                            if ( audio_bytes >= bytes_per_frame )
                                got_audio = true;
                        }
                    }

                    if ( !has_video() )
                    {
                        AVPacket pkt;
                        av_init_packet( &pkt );
                        pkt.size = 0;
                        pkt.data = NULL;
                        pkt.dts = pkt.pts = _dts;
                        _video_packets.push_back( pkt );
                    }

#ifdef DEBUG_DECODE_POP_AUDIO
                    fprintf( stderr, "\t[avi]POP. A f: %05" PRId64 " audio pts: %07" PRId64 
                             " dts: %07" PRId64 " as frame: %05" PRId64 "\n",
                             pktframe, pkt.pts, pkt.dts, pktframe );
#endif
                    continue;
                }

            av_packet_unref( &pkt );
	}

      
        if ( has_picture() && (!has_audio() || audio_context() == _context) )
            find_image( _frameStart );
    }
  

    _dts = dts;
    _frame = _audio_frame = _frameStart;
    _expected = dts + 1;
    _expected_audio = _adts + 1;

    if ( _frame_offset > 3 ) _frame_offset = 0;

    if ( !has_video() )
    {
        if ( !_hires )
        {
            _w = 640;
            _h = 480;
            allocate_pixels( _frameStart, 3, image_type::kRGB,
                             image_type::kByte );
            rgb_layers();
        }
        _hires->frame( _frameStart );
        uint8_t* ptr = (uint8_t*) _hires->data().get();
        memset( ptr, 0, 3*_w*_h*sizeof(uint8_t));
    }

    //
    // Format
    //
    if ( _context->iformat )
        _format = _context->iformat->name;
    else
        _format = "Unknown";

    //
    // Miscellaneous information
    //

    dump_metadata( _context->metadata );

    char buf[128];
  
    for (unsigned i = 0; i < _context->nb_chapters; ++i) 
    {
        AVChapter *ch = _context->chapters[i];
        sprintf( buf, "Chapter %d ", i+1 );
        dump_metadata(ch->metadata, buf);
    }

    if ( _context->nb_programs )
    {
        for (unsigned i = 0; i < _context->nb_programs; ++i) 
        {
            AVDictionaryEntry* tag = 
            av_dict_get(_context->programs[i]->metadata,
                        "name", NULL, 0);
            if ( tag ) 
            {
                sprintf( buf, "Program %d: %s", i+1, tag->key );
                _iptc.insert( std::make_pair(buf, tag->value) );
            }
            sprintf( buf, "Program %d ", i+1 );
            dump_metadata( _context->programs[i]->metadata, buf );
        }
    }
 
   
    if ( has_audio() )
    {
        AVStream* stream = get_audio_stream();
        if ( stream->metadata ) dump_metadata( stream->metadata, "Audio " );
    }
  
    if ( has_video() )
    {
        AVStream* stream = get_video_stream();
        if ( stream->metadata ) dump_metadata( stream->metadata, "Video " );
    }

}

void aviImage::probe_size( unsigned p ) 
{ 
    if ( !_context ) return;
   _context->probesize = p; 
}

bool aviImage::initialize()
{
  if ( _context == NULL )
    {

        avfilter_register_all();

      AVDictionary *opts = NULL;
      av_dict_set(&opts, "initial_pause", "1", 0);

      AVInputFormat*     format = NULL;
      // int error = avformat_open_input( &_context, filename(), 
      //   			       format, &opts );
      int error = avformat_open_input( &_context, fileroot(), 
				       format, &opts );


      if ( error >= 0 )
	{
	   // Change probesize and analyze duration to 30 secs 
	   // to detect subtitles.
	   if ( _context )
	   {
	      probe_size( 30 * AV_TIME_BASE );
	   }
	   error = avformat_find_stream_info( _context, NULL );
	}

      if ( error >= 0 )
	{

	  // Allocate an av frame
	  _av_frame = av_frame_alloc();

	  populate();
	}
      else
	{
	  _context = NULL;
          LOG_ERROR( filename() << _(" Could not open file") );
	  return false;
	}
    }

  return true;
}

void aviImage::preroll( const boost::int64_t frame )
{
  _dts = _adts = _frame = _audio_frame = frame;

  _images.reserve( _max_images );

}


boost::int64_t aviImage::queue_packets( const boost::int64_t frame,
                                        const bool is_seek,
                                        bool& got_video,
                                        bool& got_audio,
                                        bool& got_subtitle )
{

    boost::int64_t dts = frame;

    boost::int64_t vpts, apts, spts;

    if ( !got_video ) {
        vpts = frame2pts( get_video_stream(), frame );
    }

    if ( !got_audio ) {
        if ( _acontext )
        {
            assert( get_audio_stream() != NULL );
            apts = frame2pts( get_audio_stream(), frame + _audio_offset );
        }
        else
        {
            assert( get_audio_stream() != NULL );
            apts = frame2pts( get_audio_stream(), frame );
        }
    }

    if ( !got_subtitle ) {
        spts = frame2pts( get_subtitle_stream(), frame );
    }


    AVPacket pkt = {0};

    // Clear the packet
    av_init_packet( &pkt );
    pkt.size = 0;
    pkt.data = NULL;

    unsigned int bytes_per_frame = audio_bytes_per_frame();
    unsigned int audio_bytes = 0;

    bool eof = false;

    // Loop until an error or we have what we need
    while( !got_video || (!got_audio && audio_context() == _context) )
    {

        if (eof) {
            if (!got_video && video_stream_index() >= 0) {
                av_init_packet(&pkt);
                pkt.size = 0;
                pkt.data = NULL;
                pkt.stream_index = video_stream_index();
                _video_packets.push_back( pkt );
                got_video = true;
                got_subtitle = true;
                if ( is_seek || playback() == kBackwards )
                {
                    _video_packets.seek_end(vpts);
                }
            }

            AVStream* stream = get_audio_stream();
            if (!got_audio )
            {
                if (audio_context() == _context && _audio_ctx &&
                    _audio_ctx->codec->capabilities & CODEC_CAP_DELAY) {
                    av_init_packet(&pkt);
                    pkt.size = 0;
                    pkt.data = NULL;
                    pkt.stream_index = audio_stream_index();
                    _audio_packets.push_back( pkt );
                }

                got_audio = true;

                if ( is_seek || playback() == kBackwards )
                {
                    _audio_packets.seek_end(apts);
                }
            }

            if ( !got_subtitle && ( is_seek || playback() == kBackwards ) )
            {
                _subtitle_packets.seek_end(spts);
            }

            eof = false;
            break;
        }

        int error = av_read_frame( _context, &pkt );

        if ( error < 0 )
        {
            if ( error == AVERROR_EOF )
            {
                eof = true;
                continue;
            }
            int err = _context->pb ? _context->pb->error : 0;
            if ( err != 0 )
            {
                IMG_ERROR("fetch: Could not read frame " << frame << " error: "
                          << strerror(err) );
            }

            if ( is_seek )
            {
                if ( !got_video )    _video_packets.seek_end(vpts);
                if ( !got_audio && apts >= 0 ) _audio_packets.seek_end(apts);
                if ( !got_subtitle ) _subtitle_packets.seek_end(spts);
            }


            av_packet_unref( &pkt );

            break;
        }


        if ( has_video() && pkt.stream_index == video_stream_index() )
        {
            boost::int64_t pktframe = pts2frame( get_video_stream(), pkt.dts )                                      - _frame_offset; // needed

            if ( playback() == kBackwards )
            {
                if ( pktframe <= frame )
                {
                    _video_packets.push_back( pkt );
                }
                // should be pktframe without +1 but it works better with it.
                if ( pktframe < dts ) dts = pktframe + 1; 
            }
            else
            {
                _video_packets.push_back( pkt );
                if ( pktframe > dts ) dts = pktframe;
            }

            if ( !got_video && pktframe >= frame )
            {
                got_video = true;
                if ( is_seek ) _video_packets.seek_end(vpts);
            }
#ifdef DEBUG_DECODE
            char ftype = av_get_picture_type_char(_av_frame->pict_type );
            fprintf( stderr, "\t[avi] FETCH V f: %05" PRId64 
                     " video pts: %07" PRId64 
                     " dts: %07" PRId64 " %c as frame: %05" PRId64 "\n",
                     frame, pkt.pts, pkt.dts, ftype, pktframe );
#endif
            continue;
        }
        else if ( has_subtitle()  &&
                  pkt.stream_index == subtitle_stream_index() )
        {
            boost::int64_t pktframe = get_frame( get_subtitle_stream(), pkt );
            if ( playback() == kBackwards )
            {
                if ( pktframe <= frame )
                    _subtitle_packets.push_back( pkt );
            }
            else
            {
                _subtitle_packets.push_back( pkt );
            }
	
            if ( !got_subtitle && pktframe >= frame )
            {
                got_subtitle = true;
                if ( is_seek ) _subtitle_packets.seek_end(spts);
            }
            continue;
        }
        else
        {
	
            if ( has_audio() && audio_context() == _context &&
                 pkt.stream_index == audio_stream_index() )
            {
                boost::int64_t pktframe = pts2frame( get_audio_stream(), 
                                                     pkt.dts );
                _adts = pktframe;

                if ( playback() == kBackwards )
                {
                    // Only add packet if it comes before seek frame
                    //if ( pktframe <= frame )
                        _audio_packets.push_back( pkt );
                    if ( !has_video() && pktframe < dts ) dts = pktframe;
                }
                else
                {
                    // ffmpeg @bug:  audio seeks in long mp3s while playing can
                    // result in ffmpeg going backwards too far.
                    // This pktframe >= frame-10 is to avoid that.
                    _audio_packets.push_back( pkt );
                    got_audio = true;
                    if ( !has_video() && pktframe > dts ) dts = pktframe;
                }
                
                if ( got_audio && !has_video() )
                {
                    for (int64_t t = frame; t <= pktframe; ++t )
                    {
                        AVPacket pkt;
                        av_init_packet( &pkt );
                        pkt.size = 0;
                        pkt.data = NULL;
                        pkt.dts = pkt.pts = t;
                        _video_packets.push_back( pkt );
                    }
                }

                if ( is_seek && got_audio ) {
                    if ( !has_video() ) _video_packets.seek_end(vpts);
                    _audio_packets.seek_end(apts);
                }


#ifdef DEBUG_DECODE_AUDIO
                fprintf( stderr, "\t[avi] FETCH A f: %05" PRId64 
                         " audio pts: %07" PRId64 
                         " dts: %07" PRId64 "   as frame: %05" PRId64 "\n",
                         frame, pkt.pts, pkt.dts, pktframe );
#endif
                continue;
            }
        }
     
        av_packet_unref( &pkt );


    } // (!got_video || !got_audio)
    
    // For secondary audio
    if ( _acontext )
    {
        _adts = CMedia::queue_packets( frame + _audio_offset, is_seek,
                                       got_video, got_audio, got_subtitle );
        _expected_audio = _adts + 1;
    }

    //debug_video_packets( dts, "queue_packets");
  
    if ( dts > last_frame() ) dts = last_frame();
    else if ( dts < first_frame() ) dts = first_frame();

    return dts;
}



bool aviImage::fetch(const boost::int64_t frame)
{
#ifdef DEBUG_DECODE
    cerr << "FETCH BEGIN: " << frame << " EXPECTED: " << _expected
         << " DTS: " << _dts << endl;
#endif

    if ( _right_eye && (playback() == kStopped || playback() == kSaving) )
    {
       _right_eye->stop();
       _right_eye->fetch( frame );
    }

   bool got_video = !has_video();
   bool got_audio = !has_audio();
   bool got_subtitle = !has_subtitle();


   if ( (!got_video || !got_audio || !got_subtitle) && frame != _expected )
   {
       DBG( "frame " << frame << "  EXPECTED " << _expected );
       bool ok = seek_to_position( frame );
       if ( !ok )
           IMG_ERROR("seek_to_position: Could not seek to frame " 
                     << frame );
       return ok;
   }

   
#ifdef DEBUG_DECODE
  cerr << "------------------------------------------------------" << endl;
  cerr << "FETCH START: " << frame << " gotV:" << got_video << " gotA:" << got_audio << endl;
#endif

#ifdef DEBUG_VIDEO_PACKETS
  debug_video_packets(frame, "Fetch", true);
#endif
#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "Fetch", true);
#endif

#ifdef DEBUG_AUDIO_PACKETS
  debug_audio_packets(frame, "Fetch", true);
#endif
#ifdef DEBUG_AUDIO_STORES
  debug_audio_stores(frame, "Fetch");
#endif



  boost::int64_t dts = queue_packets( frame, false, got_video, 
				      got_audio, got_subtitle);


  _dts = dts;
  assert( _dts >= first_frame() && _dts <= last_frame() );

  _expected = _dts + 1;
  _expected_audio = _adts + 1;


#ifdef DEBUG_DECODE
  LOG_INFO( "------------------------------------------------------" );
  LOG_INFO( "FETCH DONE: " << _dts << "   expected: " << _expected 
	    << " gotV: " << got_video << " gotA: " << got_audio );
  LOG_INFO( "------------------------------------------------------" );
#endif

#ifdef DEBUG_VIDEO_PACKETS
  debug_video_packets(frame, "FETCH DONE");
#endif

#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "FETCH DONE");
#endif

#ifdef DEBUG_AUDIO_PACKETS
  debug_audio_packets(frame, "FETCH DONE", true);
#endif

#ifdef DEBUG_AUDIO_STORES
  debug_audio_stores(frame, "FETCH DONE");
#endif


  return true;
}


bool aviImage::frame( const boost::int64_t f )
{

    size_t vpkts = _video_packets.size();
    size_t apkts = _audio_packets.size();
    
    if ( playback() != kStopped && playback() != kSaving &&
         ( (_video_packets.bytes() +  _audio_packets.bytes() + 
            _subtitle_packets.bytes() )  >  kMAX_QUEUE_SIZE ) ||
         ( ( apkts > kMIN_FRAMES || !has_audio() ) &&
           ( vpkts > kMIN_FRAMES || !has_video() )
         ) )
    {
        // std::cerr << "false return: " << std::endl;
        std::cerr << "vp: " << vpkts
                  << " vs: " << _images.size()
                  << " ap: " << apkts
                  << " as: " << _audio.size()
                  << std::endl;
        // std::cerr << "sum: " <<
        // ( _video_packets.bytes() +  _audio_packets.bytes() + 
        // 	 _subtitle_packets.bytes() ) << " > " <<  kMAX_QUEUE_SIZE
        // 		 << std::endl;
        return true;
    }

    if ( f < _frameStart )    _dts = _adts = _frameStart;
    else if ( f > _frameEnd ) _dts = _adts = _frameEnd;
    // else                      _dts = _adts = f;



  bool ok = fetch(f);
  


#ifdef DEBUG_DECODE
  IMG_INFO( "------- FRAME DONE _dts: " << _dts << " _frame: " 
	    << _frame << " _expected: "  << _expected );
  debug_video_packets( _dts, "fetch", false );
#endif

  return ok;
}

CMedia::DecodeStatus aviImage::decode_vpacket( boost::int64_t& pktframe,
                                               const boost::int64_t& frame,
                                               const AVPacket& pkt )
{
    //boost::int64_t oldpktframe = pktframe;
    CMedia::DecodeStatus status = decode_video_packet( pktframe, frame, pkt );
    if ( status == kDecodeOK && !in_video_store(pktframe) )
    {
        store_image( pktframe, pkt.dts );
    }
    av_frame_unref(_av_frame);
    av_frame_unref(_filt_frame);
    return status;
}

CMedia::DecodeStatus 
aviImage::handle_video_packet_seek( boost::int64_t& frame, const bool is_seek )
{
#ifdef DEBUG_HSEEK_VIDEO_PACKETS
    debug_video_packets(frame, "BEFORE HSEEK", true);
#endif

#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "BEFORE HSEEK");
#endif

  Mutex& vpm = _video_packets.mutex();
  SCOPED_LOCK( vpm );


  if ( _video_packets.empty() || _video_packets.is_flush() )
      LOG_ERROR( _("Wrong packets in handle_video_packet_seek" ) );


  if ( is_seek && _video_packets.is_seek() )
  {
     _video_packets.pop_front();  // pop seek begin packet

  }
  else if ( !is_seek && _video_packets.is_preroll() )
  {
      _video_packets.pop_front();  // pop preroll begin packet
  }
  else
     IMG_ERROR( "handle_video_packet_seek error - no seek/preroll packet" );


  DecodeStatus got_video = kDecodeMissingFrame;
  DecodeStatus status;
  unsigned count = 0;


  while ( !_video_packets.empty() && !_video_packets.is_seek_end() )
  {
      const AVPacket& pkt = _video_packets.front();
      ++count;

      boost::int64_t pktframe;
      if ( pkt.dts != AV_NOPTS_VALUE )
          pktframe = pts2frame( get_video_stream(), pkt.dts );
      else
          pktframe = frame;

      if ( !is_seek && playback() == kBackwards )
      {
          // std::cerr << "pkt " << pktframe << " frame " << frame << std::endl;
          if (pktframe >= frame )
          {
              status = decode_vpacket( pktframe, frame, pkt );
          }
          else
          {
              if ( !in_video_store(pktframe) )
              {
                  status = decode_image( pktframe, (AVPacket&)pkt );
              }
              else
              {
                  status = decode_vpacket( pktframe, frame, pkt );
              }
          }

          if ( status == kDecodeOK )  got_video = status;
      }
      else
      {
	  if ( !in_video_store(pktframe) )
          {
              status = decode_image( pktframe, (AVPacket&)pkt );
          }
	  else
          {
              status = decode_vpacket( pktframe, frame, pkt );
          }

          if ( status == kDecodeOK && pktframe >= frame )
              got_video = status;
      }

      _video_packets.pop_front();
  }


  if ( _video_packets.empty() ) {
      LOG_ERROR( _("Empty packets for video seek") );
      return kDecodeError;
  }

  if ( count > 0 && is_seek )
  {
    const AVPacket& pkt = _video_packets.front();
    frame = pts2frame( get_video_stream(), pkt.dts );
  }

  if ( _video_packets.is_seek_end() )
     _video_packets.pop_front();  // pop seek end packet

  if ( count == 0 ) {
      LOG_ERROR( _("Empty seek or preroll") );
      return kDecodeError;
  }

#ifdef DEBUG_HSEEK_VIDEO_PACKETS
  debug_video_packets(frame, "AFTER HSEEK", true);
#endif

#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "AFTER HSEEK");
#endif
  return got_video;
}



void aviImage::wait_image()
{
  mrv::PacketQueue::Mutex& vpm = _video_packets.mutex();
  SCOPED_LOCK( vpm );

  for(;;)
    {
        if ( stopped() || saving() || ! _video_packets.empty() ) break;

        CONDITION_WAIT( _video_packets.cond(), vpm );
    }
  return;
}

bool aviImage::in_video_store( const boost::int64_t frame )
{
   SCOPED_LOCK( _mutex );

  // Check if video is already in video store
   video_cache_t::iterator end = _images.end();
   video_cache_t::iterator i = std::find_if( _images.begin(), end,
                                             EqualFunctor(frame) );
   if ( i != end ) return true;
   return false;
}


//
// This routine is a simplified copy of the one in ffplay,
// which is (c) 2003 Fabrice Bellard
//

CMedia::DecodeStatus
aviImage::audio_video_display( const boost::int64_t& frame )
{

    SCOPED_LOCK( _mutex );

    if (! _video_packets.empty() )
        _video_packets.pop_front();

    if ( frame > _frameEnd ) return kDecodeLoopEnd;
    else if ( frame < _frameStart ) return kDecodeLoopStart;


    SCOPED_LOCK( _audio_mutex );
    audio_type_ptr result;

    audio_cache_t::iterator end = _audio.end();
    audio_cache_t::iterator it = std::lower_bound( _audio.begin(), end,
                                                   frame,
                                                   LessThanFunctor() );
    if ( it == end ) {
        return kDecodeMissingFrame;
    }

    result = *it;

    _hires->frame( frame );
    uint8_t* ptr = (uint8_t*) _hires->data().get();
    memset( ptr, 0, 3*_w*_h*sizeof(uint8_t));

    int channels = result->channels();

    /* total height for one channel */
    int h = _h / channels;
    /* graph height / 2 */
    int h2 = (h * 9) / 20;
    int y1, y, ys, i;
    int i_start = 0;

    if ( _audio_ctx->sample_fmt == AV_SAMPLE_FMT_FLTP ||
         _audio_ctx->sample_fmt == AV_SAMPLE_FMT_FLT )
    {
        float* data = (float*)result->data();

        for (int ch = 0; ch < channels; ++ch)
        {
            i = i_start + ch;
            y1 = ch * h + ( h / 2 );
            for (unsigned x = 0; x < _w; ++x )
            {
                y = (int(data[i] * 24000 * h2)) >> 15;
                if (y < 0) {
                    y = -y;
                    ys = y1 - y;
                } else {
                    ys = y1;
                }
                fill_rectangle(ptr, x, ys, 1, y);
                i += channels;
            }
        }
    }
    else if ( _audio_ctx->sample_fmt == AV_SAMPLE_FMT_S16P ||
              _audio_ctx->sample_fmt == AV_SAMPLE_FMT_S16  )
    {
        int16_t* data = (int16_t*)result->data();

        for (int ch = 0; ch < channels; ch++)
        {
            i = i_start + ch;
            y1 = ch * h + ( h / 2 );
            for (unsigned x = 0; x < _w; ++x )
            {
                y = (data[i] * h2) >> 15;
                if (y < 0) {
                    y = -y;
                    ys = y1 - y;
                } else {
                    ys = y1;
                }
                fill_rectangle(ptr, x, ys, 1, y);
                i += channels;
            }
        }
    }




    _frame = frame;
    refresh();
    return kDecodeOK;

}

CMedia::DecodeStatus aviImage::decode_video( boost::int64_t& f )
{
    boost::int64_t frame = f;

    if ( !has_video() )
    {
        return audio_video_display(_audio_frame);
    }

#ifdef DEBUG_VIDEO_PACKETS
        debug_video_packets(frame, "decode_video", true);
#endif

  mrv::PacketQueue::Mutex& vpm = _video_packets.mutex();
  SCOPED_LOCK( vpm );

  if ( _video_packets.empty() )
  {
     bool ok = in_video_store( frame );
     if ( ok ) return kDecodeOK;
     return kDecodeError;
  }

  DecodeStatus got_video = kDecodeMissingFrame;


  while (  got_video != kDecodeOK && !_video_packets.empty()  )
    {
      if ( _video_packets.is_flush() )
	{
            flush_video();
            _video_packets.pop_front();
            continue;
	}
      else if ( _video_packets.is_seek() )
	{
	  return handle_video_packet_seek( frame, true );
	}
      else if ( _video_packets.is_preroll() )
	{
	   bool ok = in_video_store( frame );
	   if ( ok ) 
	   {
               SCOPED_LOCK( _mutex );
               video_cache_t::const_iterator iter = _images.begin();
               if ( (*iter)->frame() >= frame )
               {
                   got_video = handle_video_packet_seek( frame, false );
               }
               return kDecodeOK;
	   }

	   got_video = handle_video_packet_seek( frame, false );
	   continue;
	}
      else if ( _video_packets.is_loop_start() )
	{
	   // With prerolls, Loop indicator remains on before all frames
	   // in preroll have been shown.  That's why we check video
	   // store here.
	   bool ok = in_video_store( frame );

	   if ( ok && frame >= first_frame() )
	   {
	      return kDecodeOK;
	   }

	   if ( frame < first_frame() )
	   {
	      _video_packets.pop_front();
	      return kDecodeLoopStart;
	   }
	   else
	   {
               return got_video;
	   }
	}
      else if ( _video_packets.is_loop_end() )
	{
	  _video_packets.pop_front();
	  return kDecodeLoopEnd;
	}
      else
	{
	  AVPacket& pkt = _video_packets.front();

          boost::int64_t pktframe;
          if ( pkt.dts != AV_NOPTS_VALUE )
          {
              pktframe = pts2frame( get_video_stream(), pkt.dts );
          }
          else
          {
              pktframe = frame;
          }

          if ( playback() == kForwards &&
               pktframe > _frame + max_video_frames() )
          {
              got_video = kDecodeOK; continue;
          }

	  bool ok = in_video_store( pktframe );
	  if ( ok )
	    {
               // if ( pktframe == frame )
               {
                   decode_vpacket( pktframe, frame, pkt );
                   _video_packets.pop_front();
               }
               return kDecodeOK;
	    }


	  got_video = decode_image( pktframe, pkt );
	  _video_packets.pop_front();
          continue;
	}

    }



#ifdef DEBUG_VIDEO_STORES
  debug_video_stores(frame, "decode_video");
#endif

  return got_video;
}




void aviImage::debug_subtitle_stores(const boost::int64_t frame, 
				     const char* routine,
				     const bool detail)
{

  SCOPED_LOCK( _subtitle_mutex );

  subtitle_cache_t::const_iterator iter = _subtitles.begin();
  subtitle_cache_t::const_iterator last = _subtitles.end();
  
  std::cerr << name() << " S:" << _frame << " D:" << _dts << " V:" << frame 
       << " " << routine << " subtitle stores  #" 
       << _subtitles.size() << ": "
       << std::endl;
  
  if ( detail )
  {
     for ( ; iter != last; ++iter )
     {
	boost::int64_t f = (*iter)->frame();
	if ( f == frame )  std::cerr << "S";
	if ( f == _dts )   std::cerr << "D";
	if ( f == _frame ) std::cerr << "F";
	std::cerr << f << " ";
     }
     std::cerr << endl;
  }
}

void aviImage::debug_video_stores(const boost::int64_t frame, 
				  const char* routine,
				  const bool detail )
{

  SCOPED_LOCK( _mutex );

  video_cache_t::const_iterator iter = _images.begin();
  video_cache_t::const_iterator last = _images.end();
  
  std::cerr << name() << " S:" << _frame << " D:" << _dts << " V:" << frame 
	    << " " << routine << " video stores  #" 
	    << _images.size() << ": ";


  bool dtail = detail;

  if ( iter != last )
  {
      video_cache_t::const_iterator end = last - 1;
      
     std::cerr << (*iter)->frame() << "-" 
	       << (*end)->frame() 
	       << std::endl;

     if ( (*iter)->frame() > (*end)->frame() )
         dtail = true;
  }
  else
      std::cerr << std::endl;

  if ( dtail )
  {
     for ( ; iter != last; ++iter )
     {
	boost::int64_t f = (*iter)->frame();
	if ( f == frame )  std::cerr << "S";
	if ( f == _dts )   std::cerr << "D";
	if ( f == _frame ) std::cerr << "F";
	std::cerr << f << " ";
     }
     std::cerr << endl;
  }
}


void aviImage::debug_subtitle_packets(const boost::int64_t frame, 
				      const char* routine,
				      const bool detail )
{
  if ( !has_subtitle() ) return;

  mrv::PacketQueue::Mutex& spm = _subtitle_packets.mutex();
  SCOPED_LOCK( spm );

  mrv::PacketQueue::const_iterator iter = _subtitle_packets.begin();
  mrv::PacketQueue::const_iterator last = _subtitle_packets.end();
  std::cerr << name() << " S:" << _frame << " D:" << _dts << " V:" << frame 
	    << " " << routine << " subtitle packets #" 
	    << _subtitle_packets.size() << " (bytes:" 
	    << _subtitle_packets.bytes() << "): "
	    << std::endl;

  if ( detail )
  {
     bool in_preroll = false;
     bool in_seek = false;
     for ( ; iter != last; ++iter )
     {
	if ( _subtitle_packets.is_flush( *iter ) )
	{
	   std::cerr << "* "; continue;
	}
	else if ( _subtitle_packets.is_loop_start( *iter ) ||
		  _subtitle_packets.is_loop_end( *iter ) )
	{
	   std::cerr << "L "; continue;
	}
	
	assert( (*iter).dts != MRV_NOPTS_VALUE );
	boost::int64_t f = pts2frame( get_subtitle_stream(), (*iter).dts );
	if ( _subtitle_packets.is_seek_end( *iter ) )
	{
	   if ( in_preroll )
	   {
	      std::cerr << "[PREROLL END: " << f << "]";
	      in_preroll = false;
	   }
	   else if ( in_seek )
	   {
	      std::cerr << "<SEEK END:" << f << ">";
	      in_seek = false;
	   }
	   else
	   {
	      std::cerr << "+ERROR:" << f << "+";
	   }
	}
	else if ( _subtitle_packets.is_seek( *iter ) )
	{
	   std::cerr << "<SEEK:" << f << ">";
	   in_seek = true;
	}
	else if ( _subtitle_packets.is_preroll( *iter ) )
	{
	   std::cerr << "[PREROLL:" << f << "]";
	   in_preroll = true;
	}
	else
	{
	   if ( f == frame )  std::cerr << "S";
	   if ( f == _dts )   std::cerr << "D";
	   if ( f == _frame ) std::cerr << "F";
	   std::cerr << f << " ";
	}
     }
     std::cerr << std::endl;
  }

}


void aviImage::do_seek()
{
    // No need to set seek frame for right eye here
    if ( _right_eye )  _right_eye->do_seek();

    _dts = _adts = _seek_frame;

    bool got_video = !has_video();
    bool got_audio = !has_audio();


    if ( !got_audio || !got_video )
    {
        if ( _seek_frame != _expected )
            clear_packets();
        fetch( _seek_frame );
    }


  // Seeking done, turn flag off
  _seek_req = false;

  if ( stopped() || saving() )
    {

       DecodeStatus status;
       if ( has_audio() )
       {
           boost::int64_t f = _seek_frame;
           f += _audio_offset;
           status = decode_audio( f );
           if ( status > kDecodeOK )
               IMG_ERROR( _("Decode audio error: ") 
                          << decode_error( status ) 
                          << _(" for frame ") << _seek_frame );
           find_audio( _seek_frame + _audio_offset );
       }
       
       if ( has_video() || has_audio() )
       {
	  status = decode_video( _seek_frame );

	  if ( !find_image( _seek_frame ) && status != kDecodeOK )
              IMG_ERROR( _("Decode video error seek frame " )
                         << _seek_frame 
                         << _(" status: ") << decode_error( status ) );
       }
       
       if ( has_subtitle() && !saving() )
       {
	  decode_subtitle( _seek_frame );
	  find_subtitle( _seek_frame );
       }
       
#ifdef DEBUG_VIDEO_STORES
      debug_video_stores(_seek_frame, "doseek" );
#endif

      // Queue thumbnail for update
      image_damage( image_damage() | kDamageThumbnail );
    }

}

//
// SUBTITLE STUFF
//



void aviImage::subtitle_rect_to_image( const AVSubtitleRect& rect )
{
  int imgw = width();
  int imgh = height();


  int dstx = FFMIN(FFMAX(rect.x, 0), imgw);
  int dsty = FFMIN(FFMAX(rect.y, 0), imgh);
  int dstw = FFMIN(FFMAX(rect.w, 0), imgw - dstx);
  int dsth = FFMIN(FFMAX(rect.h, 0), imgh - dsty);

  boost::uint8_t* root = (boost::uint8_t*) _subtitles.back()->data().get();
  assert( root != NULL );

  unsigned char a;
  ImagePixel yuv, rgb;

  const unsigned* pal = (const unsigned*)rect.data[1];

  for ( int x = dstx; x < dstx + dstw; ++x )
  {
     for ( int y = dsty; y < dsty + dsth; ++y )
     {
  	boost::uint8_t* d = root + 4 * (x + y * imgw); 
	assert( d != NULL );

	boost::uint8_t* const s = rect.data[0] + (x-dstx) + 
                                  (y-dsty) * dstw;

	unsigned t = pal[*s];
	a = static_cast<unsigned char>( (t >> 24) & 0xff );
	yuv.b = float( (t >> 16) & 0xff );
	yuv.g = float( (t >> 8) & 0xff );
	yuv.r = float( t & 0xff );

	rgb = mrv::color::yuv::to_rgb( yuv );

        if ( rgb.r < 0x00 ) rgb.r = 0x00;
        else if ( rgb.r > 0xff ) rgb.r = 0xff;
        if ( rgb.g < 0x00 ) rgb.g = 0x00;
        else if ( rgb.g > 0xff ) rgb.g = 0xff;
        if ( rgb.b < 0x00 ) rgb.b = 0x00;
        else if ( rgb.b > 0xff ) rgb.b = 0xff;

        float w = a / 255.0f;
        rgb.r = rgb.g * w;
        rgb.g *= w;
        rgb.b *= w;

	*d++ = uint8_t( rgb.r );
	*d++ = uint8_t( rgb.g );
	*d++ = uint8_t( rgb.b );
	*d++ = a;
     }
  }
}


// Flush subtitle buffers
void aviImage::flush_subtitle()
{
  if ( _subtitle_ctx && _subtitle_index >= 0)
  {
      SCOPED_LOCK( _subtitle_mutex );
      avcodec_flush_buffers( _subtitle_ctx );
  }
}

void aviImage::subtitle_stream( int idx )
{
  if ( idx < -1 || unsigned(idx) >= number_of_subtitle_streams() )
      idx = -1;

  if ( idx == _subtitle_index ) return;

  mrv::PacketQueue::Mutex& apm = _subtitle_packets.mutex();
  SCOPED_LOCK( apm );

  flush_subtitle();
  close_subtitle_codec();
  _subtitle_packets.clear();

  _subtitle_index = idx;

  if ( _subtitle_index >= 0 && !filter_graph )
    {
      open_subtitle_codec();
      seek( _frame );
    }
}

void aviImage::store_subtitle( const boost::int64_t& frame,
			       const boost::int64_t& repeat )
{
  // if ( _sub.format != 0 )
  //   {
  //     IMG_ERROR("Subtitle format " << _sub.format << " not yet handled");
  //     subtitle_stream(-1);
  //     return;
  //   }

  unsigned w = width();
  unsigned h = height();

  image_type_ptr pic( new image_type(
				     frame,
				     w, h, 4,
				     image_type::kRGBA,
				     image_type::kByte,
				     repeat
				     ) 
		      );

  {
     SCOPED_LOCK( _subtitle_mutex );
     _subtitles.push_back( pic );

     boost::uint8_t* data = (boost::uint8_t*)
     _subtitles.back()->data().get();

     // clear image
     memset( data, 0, w*h*4 );
     
     for (unsigned i = 0; i < _sub.num_rects; ++i)
     {
	const AVSubtitleRect* rect = _sub.rects[i];

	assert( rect->type != SUBTITLE_NONE );

	switch( rect->type )
	{
	   case SUBTITLE_NONE:
	      break;
	   case SUBTITLE_BITMAP:
	      subtitle_rect_to_image( *rect );
	      break;
	   case SUBTITLE_TEXT:
	      // subtitle_text_to_image( *rect );
	      std::cerr << rect->text << std::endl;
	      break;
	   case SUBTITLE_ASS:
	      subtitle_rect_to_image( *rect );
	      break;
	}

     }
  }


  avsubtitle_free( &_sub );

}


CMedia::DecodeStatus
aviImage::decode_subtitle_packet( boost::int64_t& ptsframe, 
				  boost::int64_t& repeat,
				  const boost::int64_t frame, 
				  const AVPacket& pkt
				  )
{
  AVStream* stream = get_subtitle_stream();

  boost::int64_t endframe;
  if ( pkt.pts != MRV_NOPTS_VALUE )
    { 
        ptsframe = pts2frame( stream, boost::int64_t( double(pkt.pts) + 
						     _sub.start_display_time /
						     1000.0 ) );
       endframe = pts2frame( stream, boost::int64_t( double(pkt.pts) + 
						     _sub.end_display_time /
						     1000.0 ) );
       repeat = endframe - ptsframe + 1;
    }
  else
    {
        ptsframe = pts2frame( stream, boost::int64_t( double(pkt.dts) + 
						    _sub.start_display_time /
						    1000.0 ) );
      endframe = pts2frame( stream, boost::int64_t( double(pkt.dts) + 
						    _sub.end_display_time / 
						    1000.0 ) );
      repeat = endframe - ptsframe + 1;
      IMG_ERROR("Could not determine pts for subtitle frame, "
		"using " << ptsframe );
    }

  if ( repeat <= 1 )
  {
     repeat = boost::int64_t( fps() * 4 );
  }

  int got_sub = 0;
  avcodec_decode_subtitle2( _subtitle_ctx, &_sub, &got_sub, 
			    (AVPacket*)&pkt );
  if ( got_sub == 0 ) return kDecodeError;


  // AVSubtitle has a start display time in ms. relative to pts
  // ptsframe = ptsframe + boost::int64_t( _sub.start_display_time * fps() / 
  // 					1000 );

  return kDecodeOK;
}

// Decode the subtitle
CMedia::DecodeStatus
aviImage::decode_subtitle( const boost::int64_t frame, const AVPacket& pkt )
{
   boost::int64_t ptsframe, repeat;

  DecodeStatus status = decode_subtitle_packet( ptsframe, repeat, frame, pkt );
  if ( status != kDecodeOK )
    {
       IMG_WARNING("Could not decode subtitle frame " << ptsframe 
		   << " pts: " 
		   << pkt.pts << " dts: " << pkt.dts
		   << " data: " << (void*)pkt.data);
    }
  else
    {
       store_subtitle( ptsframe, repeat );
    }

  return status;
}

CMedia::DecodeStatus 
aviImage::handle_subtitle_packet_seek( boost::int64_t& frame, 
				       const bool is_seek )
{
#ifdef DEBUG_PACKETS
  debug_subtitle_packets(frame, "BEFORE PREROLL");
#endif

#ifdef DEBUG_SUBTITLE_STORES
  debug_subtitle_stores(frame, "BEFORE PREROLL");
#endif

  Mutex& mutex = _subtitle_packets.mutex();
  SCOPED_LOCK( mutex );

  _subtitle_packets.pop_front();  // pop seek begin packet

  DecodeStatus got_subtitle = kDecodeMissingFrame;

  while ( !_subtitle_packets.is_seek_end() && !_subtitle_packets.empty() )
    {
      const AVPacket& pkt = _subtitle_packets.front();
      
      boost::int64_t repeat = 0;
      boost::int64_t pktframe = get_frame( get_subtitle_stream(), pkt );


      if ( !is_seek && _playback == kBackwards && 
	   pktframe >= frame )
	{
	   boost::int64_t ptsframe, repeat;
	   DecodeStatus status = decode_subtitle_packet( ptsframe, repeat, 
							 frame, pkt );
	   if ( status == kDecodeOK || status == kDecodeMissingFrame )
	   {
	      store_subtitle( ptsframe, repeat );

	      if ( status == kDecodeOK ) got_subtitle = status;
	   }
	}
      else
	{
	  if ( pktframe >= frame )
	    {
	      got_subtitle = decode_subtitle( frame, pkt );
	    }
	  else
	    {
	       decode_subtitle_packet( pktframe, repeat, frame, pkt );
	    }
	}

      _subtitle_packets.pop_front();
    }

  if ( _subtitle_packets.empty() ) return kDecodeError;

  {
    const AVPacket& pkt = _subtitle_packets.front();
    frame = get_frame( get_subtitle_stream(), pkt );
  }

  if ( _subtitle_packets.is_seek_end() )
     _subtitle_packets.pop_front();  // pop seek end packet

      
#ifdef DEBUG_PACKETS
  debug_subtitle_packets(frame, "AFTER PREROLL");
#endif

#ifdef DEBUG_SUBTITLE_STORES
  debug_subtitle_stores(frame, "AFTER PREROLL");
#endif
  return got_subtitle;
}


int64_t aviImage::wait_subtitle()
{
  mrv::PacketQueue::Mutex& spm = _subtitle_packets.mutex();
  SCOPED_LOCK( spm );

  for(;;)
    {
        if ( stopped() || saving() ) break;

      if ( ! _subtitle_packets.empty() )
	{
	  const AVPacket& pkt = _subtitle_packets.front();
	  return pts2frame( get_subtitle_stream(), pkt.pts );
	}

      CONDITION_WAIT( _subtitle_packets.cond(), spm );
    }
  return _frame;
}

CMedia::DecodeStatus aviImage::decode_subtitle( const boost::int64_t f )
{
   if ( _subtitle_index < 0 ) return kDecodeOK;

   boost::int64_t frame = f;

  mrv::PacketQueue::Mutex& vpm = _subtitle_packets.mutex();
  SCOPED_LOCK( vpm );

  if ( _subtitle_packets.empty() )
    {
      bool ok = in_subtitle_store( frame );
      if ( ok ) return kDecodeOK;
      return kDecodeMissingFrame;
    }

  DecodeStatus got_video = kDecodeMissingFrame;

  while ( got_video != kDecodeOK && !_subtitle_packets.empty() )
    {
      if ( _subtitle_packets.is_flush() )
	{
	  flush_subtitle();
	  _subtitle_packets.pop_front();
	}
      else if ( _subtitle_packets.is_seek() )
	{
            return handle_subtitle_packet_seek( frame, true );
	}
      else if ( _subtitle_packets.is_preroll() )
	{
	  AVPacket& pkt = _subtitle_packets.front();
	  bool ok = in_subtitle_store( frame );
	  if ( ok && pts2frame( get_subtitle_stream(), pkt.pts ) != frame )
	    return kDecodeOK;

	  return handle_subtitle_packet_seek( frame, false );
	}
      else if ( _subtitle_packets.is_loop_start() )
	{
	  AVPacket& pkt = _subtitle_packets.front();
	  // with loops, packet dts is really frame
	  if ( frame <= pkt.pts )
	    {
	      flush_subtitle();
	      _subtitle_packets.pop_front();
	      return kDecodeLoopStart;
	    }

	  bool ok = in_subtitle_store( frame );	   
	  if ( ok ) return kDecodeOK;
	  return kDecodeError;
	}
      else if ( _subtitle_packets.is_loop_end() )
	{
	  AVPacket& pkt = _subtitle_packets.front();
	  // with loops, packet dts is really frame
	  if ( frame >= pkt.pts )
	    {
	      flush_subtitle();
	      _subtitle_packets.pop_front();
	      return kDecodeLoopEnd;
	    }

	  bool ok = in_subtitle_store( frame );	   
	  if ( ok ) return kDecodeOK;
	  return kDecodeError;
	}
      else
	{
	  AVPacket& pkt = _subtitle_packets.front();

	  bool ok = in_subtitle_store( frame );
	  if ( ok )
	    {
	      assert( pkt.pts != MRV_NOPTS_VALUE );
	      if ( pts2frame( get_subtitle_stream(), pkt.pts ) == frame )
		{
		   boost::int64_t ptsframe, repeat;
		   decode_subtitle_packet( ptsframe, repeat, frame, pkt );
		   _subtitle_packets.pop_front();
		}
	      return kDecodeOK;
	    }

	  got_video = decode_subtitle( frame, pkt );

	  _subtitle_packets.pop_front();
	}

    }


#ifdef DEBUG_SUBTITLE_STORES
  debug_subtitle_stores(frame, "decode_subtitle");
#endif

  return got_video;
}


bool aviImage::in_subtitle_store( const boost::int64_t frame )
{
   SCOPED_LOCK( _subtitle_mutex );

   // Check if audio is already in audio store
   // If so, no need to decode it again.  We just pop the packet and return.
   subtitle_cache_t::iterator end = _subtitles.end();
   subtitle_cache_t::iterator i = std::find_if( _subtitles.begin(), end,
						EqualFunctor(frame) );
   if ( i != end ) return true;
   return false;
}


} // namespace mrv

