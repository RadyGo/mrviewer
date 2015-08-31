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
 * @file   guessImage.cpp
 * @author gga
 * @date   Sat Aug 25 00:55:56 2007
 * 
 * @brief  
 * 
 * 
 */

#include <iostream>

#include <fltk/run.h>


// Image types
#include "exrImage.h"
#include "stubImage.h"
#include "mapImage.h"
#include "shmapImage.h"
#include "pxrzImage.h"
#include "wandImage.h"
#include "aviImage.h"
#include "iffImage.h"
#include "mrayImage.h"
#include "ddsImage.h"
#include "hdrImage.h"

#include "Sequence.h"
#include "mrvIO.h"
#include "mrvOS.h"


namespace {
  const char* kModule = "guess";
}


namespace mrv {



  using namespace std;

  /*! Description of an Image file format */
  struct ImageTypes {
    // Function to test the filetype
    bool (*test)(const boost::uint8_t* datas, unsigned size);
    // MagickWand-type Function to test the filetype, not the bytes
    bool (*test_filename)(const char* filename);
    // Function to get/create an image of this type
    CMedia* (*get)(const char* name, const boost::uint8_t* datas);
  };


  ImageTypes image_filetypes[] =
    {
      { stubImage::test,  NULL,            stubImage::get },
      { exrImage::test,   NULL,            exrImage::get },
      { iffImage::test,   NULL,            iffImage::get },
      { mapImage::test,   NULL,            mapImage::get },
      { hdrImage::test,   NULL,            hdrImage::get },
      { aviImage::test,   NULL,            aviImage::get },
      { NULL,             wandImage::test, wandImage::get },
      { ddsImage::test,   NULL,            ddsImage::get },
      { shmapImage::test, NULL,            shmapImage::get },
      { mrayImage::test,  NULL,            mrayImage::get },
      { pxrzImage::test,  NULL,            pxrzImage::get },
      { NULL, NULL, NULL }, 
    };


  CMedia* test_image( const char* name, 
		      boost::uint8_t* datas, int size )
  {
    ImageTypes* type = image_filetypes;
    for ( ; type->get; ++type )
      {
	if ( type->test )
	  {
	    if ( type->test( datas, size ) ) 
	      return type->get( name, datas );
	    else
	       if ( type->test_filename && type->test_filename( name ) )
		  return type->get( name, datas );
	  }
	else
	  {
	    if ( type->test_filename( name ) ) 
	      return type->get( name, datas );
	  }
      }
    return NULL;
  }

std::string get_short_view( bool left )
{
    const char* pairs = getenv("MRV_STEREO_CHAR_PAIRS");
    if ( ! pairs ) pairs = "L:R";

    std::string view = pairs;
    int idx = view.find( ':' );
    if ( idx == std::string::npos )
    {
        LOG_ERROR( "MRV_STEREO_CHAR_PAIRS does not have two letters separated by colon" );
        if ( left )
            return "L";
        else
            return "R";
    }
    
    if (left)
        return view.substr( 0, idx );
    else
        return view.substr( idx+1, view.size() );
}

std::string get_long_view( bool left )
{
    const char* pairs = getenv("MRV_STEREO_NAME_PAIRS");
    if ( ! pairs ) pairs = "left:right";

    std::string view = pairs;
    int idx = view.find( ':' );
    if ( idx == std::string::npos )
    {
        LOG_ERROR( "MRV_STEREO_NAME_PAIRS does not have two names separated by colon" );
        if ( left )
            return "left";
        else
            return "right";
    }

    if ( left )
        return view.substr( 0, idx );
    else
        return view.substr( idx+1, view.size() );
}

std::string parse_view( const std::string& root, bool left )
{
    int idx = root.find( "%V" );
    std::string tmp = root;
    if ( idx != std::string::npos )
    {
        tmp = root.substr( 0, idx );
        tmp += get_long_view( left );
        tmp += root.substr( idx+2, root.size() );
    }
    else
    {
        idx = root.find( "%v" );
        if ( idx != std::string::npos )
        {
            tmp = root.substr( 0, idx );
            tmp += get_short_view( left );
            tmp += root.substr( idx+2, root.size() );
        }
    }
    return tmp;
}

  CMedia* CMedia::guess_image( const char* file,
			       const boost::uint8_t* datas,
			       const int len,
			       const boost::int64_t start,
			       const boost::int64_t end,
                               const bool use_threads )
  {
    int64_t lastFrame = end;
    int64_t frame = start;

    std::string tmp;
    std::string root = file;


    bool is_stereo = false;
    bool is_seq = false;

    if ( root.find( "%V" ) != std::string::npos ||
         root.find( "%v" ) != std::string::npos )
    {
        is_stereo = true;
    }

    if ( start != std::numeric_limits<boost::int64_t>::max() ||
         end   != std::numeric_limits<boost::int64_t>::min() )
    {
        if ( mrv::fileroot( tmp, root ) )
        {
            is_seq = true;
            root = tmp;
        }
    }

    if (( root.substr( root.size() - 4, root.size()) == ".xml" ) ||
        ( root.substr( root.size() - 4, root.size()) == ".XML" ) ||
        ( root.substr( root.size() - 1, root.size()) == "~" ))
        return NULL;

    char name[1024];
    if ( is_stereo )
    {
        tmp = parse_view( root, true );

        if ( is_seq )
        {
            sprintf( name, tmp.c_str(), frame );
        }
        else
        {
            strncpy( name, tmp.c_str(), 1024 );
        }
    }
    else
    {
        if ( is_seq )
        {
            sprintf( name, root.c_str(), frame );
        }
        else
        {
            strncpy( name, root.c_str(), 1024 );
        }
    }

    boost::uint8_t* read_data = 0;
    size_t size = len;
    const boost::uint8_t* test_data = datas;
    if (!datas) {
      size = 1024;
      FILE* fp = fltk::fltk_fopen(name, "rb");
      if (!fp) 
	{
	  if ( is_seq )
	    {
                std::string quoted( root );

                string::size_type loc = 0;
                while ( ( loc = quoted.find( '%', loc ) ) != string::npos )
		{
                    quoted = ( quoted.substr(0, loc) + "%" + 
                               quoted.substr( loc, quoted.size() ) );
                    loc += 2;
		}

                LOG_ERROR( _("Image sequence \"") << quoted 
                           << _("\" not found.") );
	    }
	  else
	    {
                LOG_ERROR( _("Image \"") << name << _("\" not found.") );
	    }
	  return NULL;
	}
      test_data = read_data = new boost::uint8_t[size + 1];
      read_data[size] = 0; // null-terminate so strstr() works
      size = fread(read_data, 1, size, fp);
      fclose(fp);
    }

    CMedia* image = test_image( name, (boost::uint8_t*)test_data, 
				(unsigned int)size );
    if ( image ) 
      {
	if ( is_seq )
	{
            image->sequence( root.c_str(), frame, lastFrame, use_threads );
	}
	else
	{
            image->filename( name );
	}
      }

    delete [] read_data;

    if (image == NULL )
          return NULL;

    return image;
  }



} // namespace mrv
