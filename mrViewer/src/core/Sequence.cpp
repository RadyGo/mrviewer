/**
 * @file   Sequence.cpp
 * @author gga
 * @date   Sat Jul 21 04:03:15 2007
 * 
 * @brief  
 * 
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(WIN64)
#include <direct.h>
#define getcwd _getcwd
#define S_ISDIR(x) _S_IFDIR(x)
#else
#include <unistd.h>
#endif


#define __STDC_FORMAT_MACROS
#include <inttypes.h>  // for PRId64 macro

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <limits>

#include <fltk/run.h>

using namespace std;

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
namespace fs = boost::filesystem;


#define LOG_ERROR(x) std::cerr << x << std::endl

#include "Sequence.h"
#include "mrvString.h"
#include "mrvI8N.h"
#include "mrvOS.h"


namespace mrv
{
  const boost::int64_t kMinFrame = std::numeric_limits< boost::int64_t >::min();
  const boost::int64_t kMaxFrame = std::numeric_limits< boost::int64_t >::max();

  bool is_valid_frame_spec( const std::string framespec )
  {
    const char* c = framespec.c_str();
    if ( *c == '%' || *c == '#' || *c == '@' ) return true;

    bool range_found = false;
    for ( ++c; *c != 0; ++c )
      {
	 if ( !range_found && *c == '-' ) { range_found = true; continue; }

	if ( *c != '+' && *c >= '0' && *c <= '9' ) continue;

	return false;
      }

    return range_found;
  }

  /** 
   * Given a filename of a possible sequence, split it into
   * root name, frame string, and extension
   * 
   * @param root    root name of file sequence
   * @param frame   frame part of file name
   * @param ext     extension of file sequence
   * @param file    original filename, potentially part of a sequence.
   * 
   * @return true if a sequence, false if not.
   */
  bool split_sequence(
		      std::string& root,
		      std::string& frame,
		      std::string& ext,
		      const std::string& file
		      )
  {

    int idx[2];
    int count = 0;  // number of periods found (from end)

    int minus = 0; // number of minus signs found

    const char* e = file.c_str();
    const char* i = e + file.size() - 1;
    for ( ; i >= e; --i )
      {
	if ( *i == '/' || *i == '\\' ) break;
	if ( *i == '.' || ( count > 0 && (*i == '_' ) ) )
	  {
	    idx[count] = (int)( i - e );
	    ++count;
	    if ( count == 2 ) break;
	    continue;
	  }

	if ( count == 1 && (*i != '@' && *i != '#' && *i != 'd' && 
			    *i != 'l' && *i != '%' && *i != '-' &&
			    *i != 'I' && (*i < '0' || *i > '9')) )
	  break;
        if ( count == 1 && *i == '-' ) minus++;
      }

    if ( count == 0 ) return false;

    if ( count == 2 && minus < 2 )
      {
	root  = file.substr( 0, idx[1]+1 );
	frame = file.substr( idx[1]+1, idx[0]-idx[1]-1 );
	ext   = file.substr( idx[0], file.size()-idx[0] );


	std::string tmp = ext;
	std::transform( tmp.begin(), tmp.end(), tmp.begin(),
			(int(*)(int)) tolower);

	// If extension is one of a movie/audio file, return false
	if ( tmp == ".avi" || tmp == ".mov"  || tmp == ".divx" ||
	     tmp == ".wmv" || tmp == ".mpeg" || tmp == ".mpg"  ||
	     tmp == ".qt"  || tmp == ".wav"  || tmp == ".vob" )
	  return false;

	bool ok = is_valid_frame_spec( frame );
	return ok;
      }
    else
      {
	root = file.substr( 0, idx[0]+1 );
	ext  = file.substr( idx[0]+1, file.size() );
	i = e = ext.c_str();
	for ( ; *i != 0; ++i )
	  {
	    if ( (*i >= '0' && *i <= '9') ||
		 *i == '-' || *i == '+' ) continue;
	    frame = "";
	    return false;
	  }

	frame = ext;
	ext   = "";
	return true;
      }
  }


  bool get_sequence_limits( boost::int64_t& frameStart, 
			    boost::int64_t& frameEnd,
			    std::string& fileroot )
  {

    frameStart = frameEnd = mrv::kMinFrame;
  
    // My encoding type
    // Create and install global locale
    std::locale::global(boost::locale::generator().generate(""));

    fs::path::imbue( std::locale() );

    fs::path file = fs::path( fileroot.c_str() );
    fs::path dir = file.branch_path();

    char buf[1024];
    if ( dir.string() == "" ) {
       dir = fs::path( getcwd(buf,1024) );
    }

    if ( ( !fs::exists( dir ) ) || ( !fs::is_directory( dir ) ) )
    {
       LOG_ERROR( _("Directory '") << dir << 
		  _("' does not exist or no directory") );
       return false;
    }

    // Check if sequence is in ILM format first  ( image.1-30.exr )
    stringArray tokens;
    mrv::split_string( tokens, fileroot, "." );
    if ( tokens.size() > 2 )
      {

	const std::string& range = tokens[ tokens.size()-2 ];

	if ( mrv::matches_chars(range.c_str(), "0123456789-") )
	  {
	    stringArray frames;
	    mrv::split_string( frames, range, "-" );
	    if ( frames.size() > 1 )
	      {
		unsigned digits = (unsigned) frames[0].size();

		frameStart = atoi( frames[0].c_str() );
		frameEnd   = atoi( frames[1].c_str() );

		stringArray::iterator i = tokens.begin();
		stringArray::iterator e = tokens.end();
		fileroot = tokens[0]; ++i;
		for ( ; i != e; ++i )
		  {
		    fileroot += ".";
		    if ( *i == range )
		      {
			 char buf[256];
			 sprintf( buf, "%%0%d" PRId64, digits );
			 fileroot += buf;
		      }
		    else
		      {
			 fileroot += *i;
		      }
		  }

		return true;
	      }
	  }
      }



    std::string root, frame, ext;
    if ( ! split_sequence( root, frame, ext, file.leaf().string() ) )
    {
      return false; // NOT a recognized sequence
    }

    std::string croot;
    std::string cext;
    std::string cframe;
    frameStart = mrv::kMaxFrame;
    unsigned pad = 1;

    fs::directory_iterator e; // default constructor yields path iter. end
    for ( fs::directory_iterator i( dir ) ; i != e; ++i )
      {
	if ( !fs::exists( *i ) || fs::is_directory( *i ) ) continue;

	split_sequence( croot, cframe, cext, (*i).path().leaf().string() );
	if ( cext != ext || croot != root ) continue;  // not this sequence

	if ( cframe[0] == '0' ) pad = (unsigned) cframe.size();


	boost::int64_t f = atoi( cframe.c_str() );
	if ( f < frameStart ) frameStart = f;
	if ( f > frameEnd )   frameEnd   = f;
      }


    sprintf( buf, "%%0%d" PRId64, pad );
    split_sequence( root, frame, ext, fileroot );

    fileroot = root;
    fileroot += buf;
    fileroot += ext;


    return true;
  }


void parse_reel( mrv::LoadList& sequences, bool& edl,
		 const char* reelfile )
  {
     edl = false;

     FILE* f = fltk::fltk_fopen( reelfile, "r" );
     if (!f ) return;

    char buf[1024];
    while ( !feof(f) )
      {
	char* c;
	while ( (c = fgets( buf, 1023, f )) )
	  {
	    if ( c[0] == '#' ) continue;  // comment line
	    while ( *c != 0 && ( *c == ' ' || *c == '\t' ) ) ++c;
	    if ( strlen(c) <= 1 ) continue; // empty line
	    c[ strlen(c)-1 ] = 0;  // remove newline

	    if ( strncmp( "audio: ", c, 7 ) == 0 )
	    {
	       if ( !sequences.empty() )
		  sequences.back().audio = c+7;
	       continue;
	    }

	    boost::int64_t start = kMinFrame;
	    boost::int64_t end   = kMaxFrame;

	    char* root = c;
	    char* range = NULL;
	    char* s = c + strlen(c) - 1;
	    if ( std::string(c) == "EDL" ) {
	       edl = true;
	       continue;
	    }
	    for ( ; s != c; --s )
	      {

		if ( *s == ' ' || *s == '\t' )
		  {
		    range = s + 1;

		    for ( ; (*s == ' ' || *s == '\t') && s != c; --s ) 
		      *s = 0;
		    break;
		  }
		else if ( *s != '-' && (*s < '0' || *s > '9') ) break;
	      }

	    if ( range && range[0] != 0 )
	      {
		s = range;
		for ( ; *s != 0; ++s )
		  {
		    if ( *s == '-' )
		      {
			*s = 0;
			start = end = atoi( range );
			if ( *(s+1) != 0 )
			  end   = atoi( s + 1 );
			break;
		      }
		  }
	      }
	    sequences.push_back( LoadInfo( root, start, end ) );
	  }
      }

    fclose(f);

  }


  bool is_valid_sequence( const char* filename )
  {
    std::string root, frame, ext;
    return split_sequence( root, frame, ext, filename );
  }

  int  padded_digits( const std::string& frame )
  {
    if ( frame == "#" ) return 4;
    std::string c = frame.substr(0, 1);
    if ( c == "@" || c == "0" ) return (int)frame.size();
    if ( c == "%" ) return atoi(frame.substr(1, frame.size()-2).c_str());
    return 1;
  }


  bool fileroot( std::string& fileroot, const std::string& file )
  {
    std::string root, frame, ext;
    fs::path path = fs::path( file );


    bool sequence = split_sequence( root, frame, ext, file );
    if ( !sequence ) 
    {
      fileroot = file;
      return false;
    }

    int pad = padded_digits(frame);

    char full[1024];
    if ( pad == 0 )
    {
      sprintf( full, "%s%%" PRId64 "%s", root.c_str(), ext.c_str() );
    }
    else
    {
      sprintf( full, "%s%%0%d" PRId64 "%s", root.c_str(), pad, ext.c_str() );
    }

    fileroot = full;
    return true;
  }

} // namespace mrv

