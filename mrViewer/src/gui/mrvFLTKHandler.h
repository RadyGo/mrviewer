/*
    mrViewer - the professional movie and flipbook playback
    Copyright (C) 2007-2014  Gonzalo GarramuÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±o

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
 * @file   mrvFLTKHandler.cpp
 * @author gga
 * @date   Sat Jan 19 23:11:02 2008
 *
 * @brief  fltk handler to read mrViewer images and return thumbnail for
 *         file requesters.
 *
 *
 */

#ifndef mrvFLTKHandler_h
#define mrvFLTKHandler_h

#include "FL/Fl_Shared_Image.H"

namespace mrv {

Fl_Image* fltk_handler( const char* filename, uchar* header,
                        int len );

}

#endif // mrvFLTKHandler_h
