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
 * @file   mrvGLEngine.cpp
 * @author gga
 * @date   Fri Jul 13 23:47:14 2007
 *
 * @brief  OpenGL engine
 *
 */

//
// Set these for testing some features as if using a lower gfx card
//
// #define TEST_NO_SHADERS  // test without hardware shaders
// #define TEST_NO_YUV      // test in rgba mode only

#define USE_NV_SHADERS
#define USE_OPENGL2_SHADERS
#define USE_ARBFP1_SHADERS
//#define USE_STEREO_GL

#include <vector>
#include <iostream>
#include <sstream>


#if defined(WIN32) || defined(WIN64)
#  include <winsock2.h>  // to avoid winsock issues
#  include <windows.h>
#  undef min
#  undef max
#endif

#if defined(WIN32) || defined(WIN64)
#  include <fltk/win32.h>   // for fltk::getDC()
#endif

#include <GL/glew.h>

#if defined(WIN32) || defined(WIN64)
#  include <GL/wglew.h>
#elif defined(LINUX)
#  include <GL/glxew.h>
#endif

#include <GL/glu.h>
#include <GL/glut.h>




#include <half.h>
#include <ImfArray.h>
#include <ImfHeader.h>
#include <ImfStringAttribute.h>
#include <Iex.h>
#include <CtlExc.h>
#include <halfLimits.h>

#include "core/mrvMath.h"
#include "core/mrvThread.h"
#include "core/mrvRectangle.h"

#include "gui/mrvIO.h"
#include "gui/mrvImageView.h"
#include "gui/mrvMainWindow.h"
#include "gui/mrvPreferences.h"
#include "mrViewer.h"

#include "video/mrvGLShape.h"
#include "video/mrvGLShader.h"
#include "video/mrvGLEngine.h"
#include "video/mrvGLQuad.h"
#include "video/mrvGLSphere.h"
#include "video/mrvGLCube.h"
#include "video/mrvGLLut3d.h"

#undef TRACE
#define TRACE(x)


namespace
{
  const char* kModule = N_("opengl");
}


namespace fltk {

#ifdef WIN32
  extern HINSTANCE	xdisplay;
#else
  extern Display*	xdisplay;
#endif

}

// #define glPushMatrix() do {                     \
//     glPushMatrix(); \
// std::cerr << "push matrix " << __FUNCTION__ << " " << __LINE__ << std::endl; \
// } while(0)


// #define glPopMatrix() do {                      \
//     glPopMatrix(); \
// std::cerr << "pop matrix " << __FUNCTION__ << " " << __LINE__ << std::endl; \
// } while(0)




namespace mrv {

  typedef CMedia::Mutex Mutex;

  GLShader* GLEngine::_rgba   = NULL;
  GLShader* GLEngine::_YCbCr  = NULL;
  GLShader* GLEngine::_YCbCrA = NULL;
  GLShader* GLEngine::_YByRy  = NULL;
  GLShader* GLEngine::_YByRyA = NULL;

  GLint  GLEngine::_maxTexUnits     = 1;
  bool   GLEngine::_floatTextures   = false;
  bool   GLEngine::_halfTextures    = false;
  bool   GLEngine::_pow2Textures    = true;
  bool   GLEngine::_pboTextures     = false;
  bool   GLEngine::_sdiOutput       = false;
  bool   GLEngine::_fboRenderBuffer = false;

  GLuint GLEngine::sCharset = 0;   // display list for characters
  unsigned int GLEngine::_maxTexWidth;
  unsigned int GLEngine::_maxTexHeight;


  //
  // Check for opengl errors and print function name where it happened.
  //
void GLEngine::handle_gl_errors(const char* where, const unsigned line,
                                const bool print )
  {
      GLenum error = glGetError();
      if ( error == GL_NO_ERROR ) return;

      while (error != GL_NO_ERROR)
      {
          if ( print )
          {
              LOG_ERROR( where << " (" << line << ")"
                         << _(": Error ") << error << " "
                         << gluErrorString(error) );
          }
          error = glGetError();
      }
  }



void zrot2offsets( double& x, double& y,
                   const CMedia* img,
                   const mrv::ImageView::FlipDirection flip,
                   const double zdeg )
{
    x = 0.0; y = 0.0;
    double rad = zdeg * M_PI / 180.0;
    double sn = sin( rad );
    double cs = cos( rad );
    mrv::Recti dpw = img->display_window();
    if ( is_equal( sn, -1.0 ) )
    {
        if ( flip & ImageView::kFlipVertical )    y = (double)-dpw.w();
        if ( flip & ImageView::kFlipHorizontal )  x = (double)-dpw.h();
    }
    else if ( (is_equal( sn, 0.0, 0.001 ) && is_equal( cs, -1.0, 0.001 )) )
    {
        if ( flip & ImageView::kFlipVertical )    x = (double)dpw.w();
        if ( flip & ImageView::kFlipHorizontal )  y = (double)-dpw.h();
    }
    else if ( (is_equal( sn, 1.0, 0.001 ) && is_equal( cs, 0.0, 0.001 )) )
    {
        if ( flip & ImageView::kFlipVertical )    y = (double)dpw.w();
        if ( flip & ImageView::kFlipHorizontal )  x = (double)dpw.h();
    }
    else
    {
        if ( flip & ImageView::kFlipVertical )    x = (double)-dpw.w();
        if ( flip & ImageView::kFlipHorizontal )  y = (double) dpw.h();
    }
}

  std::string GLEngine::options()
  {
    using std::endl;
    std::ostringstream o;

    char* vendorString = (char*)glGetString(GL_VENDOR);
    if ( !vendorString ) vendorString = (char*)_("Unknown");

    char* rendererString = (char*)glGetString(GL_RENDERER);
    if ( !rendererString ) rendererString = (char*)_("Unknown");

    char* versionString = (char*)glGetString(GL_VERSION);
    if ( !versionString ) versionString = (char*)_("Unknown");

    o << _("Vendor:\t") << vendorString << endl
      << _("Renderer:\t") << rendererString << endl
      << _("Version:\t")  << versionString << endl
      << _("Hardware Shaders:\t") << shader_type_name() << endl
      << _("PBO Textures:\t") << (_pboTextures   ? _("Yes") : _("No")) << endl
      << _("Float Textures:\t") << (_floatTextures ? _("Yes") : _("No")) << endl
      << _("Half Textures:\t") << (_halfTextures  ? _("Yes") : _("No")) << endl
      << _("Non-POT Textures:\t")
      << (_pow2Textures  ? _("No")  : _("Yes")) << endl
      << _("Max. Texture Size:\t")
      << _maxTexWidth << N_(" x ") << _maxTexHeight << endl
      << _("Texture Units:\t") << _maxTexUnits << endl
      << _("YUV  Support:\t") << (supports_yuv() ? _("Yes") : _("No")) << endl
      << _("YUVA Support:\t") << (supports_yuva() ? _("Yes") : _("No")) << endl
      << _("SDI Output:\t") << (_sdiOutput ? _("Yes") : _("No")) << endl;
    return o.str();
  }



void GLEngine::init_charset()
{
  unsigned numChars = 255;
  int fontsize = 16;

#ifdef WIN32
    HDC   hDC = fltk::getDC();
    HGLRC hRC = wglGetCurrentContext();
    if (hRC == NULL ) hRC = wglCreateContext( hDC );

    LOGFONT     lf;
    memset(&lf,0,sizeof(LOGFONT));
    lf.lfHeight               =   -fontsize ;
    lf.lfWeight               =   FW_NORMAL ;
    lf.lfCharSet              =   ANSI_CHARSET ;
    lf.lfOutPrecision         =   OUT_RASTER_PRECIS ;
    lf.lfClipPrecision        =   CLIP_DEFAULT_PRECIS ;
    lf.lfQuality              =   DRAFT_QUALITY ;
    lf.lfPitchAndFamily       =   FF_DONTCARE|DEFAULT_PITCH;
    lstrcpy (lf.lfFaceName, N_("Helvetica") ) ;


    HFONT    fid = CreateFontIndirect(&lf);
    HFONT oldFid = (HFONT)SelectObject(hDC, fid);

    sCharset = glGenLists( numChars );

    wglMakeCurrent( hDC, hRC );
    wglUseFontBitmaps(hDC, 0, numChars-1, sCharset);

    SelectObject(hDC, oldFid);
#else
    // Find Window's default font
    Display* gdc = fltk::xdisplay;

    // Load XFont to user's specs
    char font_name[256];
    sprintf( font_name, N_("-*-fixed-*-r-normal--%d-0-0-0-*-*-iso8859-1"),
             fontsize );
    XFontStruct* hfont = XLoadQueryFont( gdc, font_name );
    if (!hfont) {
       LOG_ERROR( _("Could not open any font of size ") << fontsize);
       hfont = XLoadQueryFont( gdc, "fixed" );
       if ( !hfont ) return;
    }

    // Create GL lists out of XFont
    sCharset = glGenLists( numChars );
    glXUseXFont(hfont->fid, 0, numChars-1, sCharset);

    // Free font and struct
    XFreeFont( gdc, hfont );
#endif

  CHECK_GL;
}



/**
 * Initialize opengl textures
 *
 */
void GLEngine::init_textures()
{
  // Get maximum texture resolution for gfx card
  GLint glMaxTexDim;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);
  CHECK_GL;

#ifndef TEST_NO_PBO_TEXTURES // test not using pbo textures
  _pboTextures = ( GLEW_ARB_pixel_buffer_object != GL_FALSE );
#endif

  _has_yuv = false;

  _maxTexUnits = 1;
  if ( GLEW_ARB_multitexture )
    {
#ifndef TEST_NO_YUV
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &_maxTexUnits);
      CHECK_GL;

      if ( _maxTexUnits >= 3 )  _has_yuv = true;
#endif
    }

  _maxTexWidth = _maxTexHeight = glMaxTexDim;

}

/**
 * Initialize GLEW features
 *
 */
void GLEngine::init_GLEW()
{
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        LOG_ERROR( _("GLEW Initialize Error: ") << glewGetErrorString(err) );
        exit(1);
    }

// #if defined(WIN32) || defined(WIN64)
//   err = wglewInit();
//   if (GLEW_OK != err)
//     {
//       /* Problem: wglewInit failed, something is seriously wrong. */
//       std::cerr << "WGLEW Initialize Error: "
//              << glewGetErrorString(err) << std::endl;
//       exit(1);
//     }
// #else
//   err = glxewInit();
//   if (GLEW_OK != err)
//     {
//       /* Problem: glxewInit failed, something is seriously wrong. */
//       std::cerr << "GLXEW Initialize Error: "
//              << glewGetErrorString(err) << std::endl;
//       exit(1);
//     }
// #endif
}


/**
 * Initialize OpenGL context, textures, and get opengl features for
 * this gfx card.
 *
 */
void GLEngine::initialize()
{
  init_GLEW();

  init_charset();

  init_textures();

  static bool glut_init = false;

  if ( !glut_init )
  {
      int argc = 1;
      static char* args[] = { (char*)"GlEngine", NULL };
      glutInit( &argc, args );
      glut_init = true;
  }

// #if defined(WIN32) || defined(WIN64)
//   if ( WGLEW_WGL_swap_control )
//     {
//       std::cerr << "swap control vsync? "
//              << wglGetSwapIntervalEXT() << std::endl;
//     }
// #else
//   if ( GLX_SGI_video_sync )
//     {
//     }
// #endif


#if defined(WIN32) || defined(WIN64)
  if ( wglewIsSupported( N_("WGL_NV_video_out") ) )
    {
      _sdiOutput = true;
    }
#else
  if ( glxewIsSupported( N_("GLX_NV_video_out") ) ||
       glxewIsSupported( N_("GLX_NV_video_output") ) )
    {
      _sdiOutput = true;
    }
#endif

  if ( _hardwareShaders == kAuto )
    {
      _hardwareShaders = kNone;
#ifndef TEST_NO_SHADERS

#ifdef USE_ARBFP1_SHADERS
      if ( GLEW_ARB_fragment_program )
        _hardwareShaders = kARBFP1;
#endif

#ifdef USE_OPENGL2_SHADERS
      if ( GLEW_VERSION_2_0 )
        _hardwareShaders = kGLSL;
#endif

#ifdef USE_NV_SHADERS
      if ( GLEW_NV_fragment_program )
        _hardwareShaders = kNV30;
#endif

#endif

    }

  if ( _hardwareShaders != kNone )
  {
      LOG_INFO( _("Using hardware shader profile: ") << shader_type_name() );

      std::string directory;

      if ( _has_yuv )
        {
          _has_yuva = false;
          if ( _maxTexUnits > 4 )  // @todo: bug fix
            {
                _has_yuva = true;
            }
        }


      const char* env = getenv( N_("MRV_SHADER_PATH") );
      if ( !env )
        {
          env = getenv( N_("MRV_ROOT") );
          if ( env )
            {
              directory = env;
              directory += N_("/shaders");
            }
        }
      else
        {
          directory = env;
        }

      if ( !directory.empty() )
        {
          char shaderFile[256];

          try
            {
              const char* ext = NULL;
              switch( _hardwareShaders )
                {
                case kNV30:
                  ext = N_("fp30"); break;
                case kGLSL:
                  ext = N_("glsl"); break;
                case kARBFP1:
                  ext = N_("arbfp1"); break;
                default:
                  break;
                }

              const char* dir = directory.c_str();

              sprintf( shaderFile, N_("%s/%s.%s"), dir, N_("rgba"), ext );
              _rgba = new GLShader( shaderFile );


              if ( _has_yuv )
                {
                  sprintf( shaderFile, N_("%s/%s.%s"), dir, N_("YCbCr"), ext );
                  _YCbCr = new GLShader( shaderFile );

                  sprintf( shaderFile, N_("%s/%s.%s"), dir, N_("YByRy"), ext );
                  _YByRy = new GLShader( shaderFile );
                }

              if ( _has_yuva )
                {
                  sprintf( shaderFile, N_("%s/%s.%s"), dir, N_("YCbCrA"), ext );
                  _YCbCrA = new GLShader( shaderFile );

                  sprintf( shaderFile, N_("%s/%s.%s"), dir, N_("YByRyA"), ext );
                  _YByRyA = new GLShader( shaderFile );
                }

            }
          catch ( const std::exception& e )
            {
              LOG_ERROR( shaderFile << ": " <<e.what() );
              directory.clear();
              _has_yuv  = false;
              _has_yuva = false;
            }
        }
      else
        {
          LOG_WARNING( _("Environment variable MRV_SHADER_PATH not found, "
                          "using built-in shader.") );
        }

      if ( directory.empty() )
        {
          directory = N_(".");
          loadBuiltinFragShader();
        }

    }
  else
    {
      LOG_INFO( _("Hardware shaders not available.") );
      _has_yuv  = false;
      _has_yuva = false;
    }

  if ( _has_yuv )
    {
      if ( _has_yuva )
        {
          LOG_INFO( _("mrViewer supports YUVA images through shaders.") );
        }
      else
        {
          LOG_INFO( _("mrViewer supports YUV images through shaders.") );
        }
    }
  else
    {
      LOG_INFO( _("mrViewer does not support YUV images.") );
    }

  _floatTextures     = ( GLEW_ARB_color_buffer_float != GL_FALSE );
  _halfTextures      = ( GLEW_ARB_half_float_pixel != GL_FALSE );
  _pow2Textures      = !GLEW_ARB_texture_non_power_of_two;
  _fboRenderBuffer   = ( GLEW_ARB_framebuffer_object != GL_FALSE );

  ImageView::VRType t = _view->vr();
  if ( t == ImageView::kVRSphericalMap )
      alloc_spheres( 4 );
  else if ( t == ImageView::kVRCubeMap )
      alloc_cubes( 4 );
  else
      alloc_quads( 4 );


  CHECK_GL;
}


/**
 * Resets the view matrix and sets the projection to match the window's viewport
 *
 */
void GLEngine::reset_view_matrix()
{
    glMatrixMode(GL_PROJECTION);


    if ( _view->vr() != vr )
    {
        vr = _view->vr();
        clear_quads();
    }

    ImageView* view = const_cast< ImageView* >( _view );
    if ( view->vr() == ImageView::kNoVR )
    {
        CHECK_GL;
        view->ortho();
        _rotX = _rotY = 0.0;
        CHECK_GL;
    }
    else
    {
        unsigned w = _view->w();
        unsigned h = _view->h();
        glLoadIdentity();
        glViewport(0, 0, w, h);
        gluPerspective( vr_angle, (float)w / (float)h, 0.1, 3.0);
        gluLookAt( 0, 0, 1, 0, 0, -1, 0, 1, 0 );
        CHECK_GL;
    }

    // Makes gl a tad faster
    glDisable(GL_DEPTH_TEST);
    CHECK_GL;
    glDisable(GL_LIGHTING);
    CHECK_GL;
}

void GLEngine::evaluate( const CMedia* img,
                         const Imath::V3f& rgb, Imath::V3f& out )
{
  QuadList::iterator q = _quads.begin();
  QuadList::iterator e = _quads.end();
  out = rgb;
  for ( ; q != e; ++q )
  {
      if ( (*q)->image() == img )
      {
          const GLLut3d* lut = (*q)->lut();
          if ( !lut ) {
              out = rgb;
              return;
          }

          lut->evaluate( rgb, out );
          return;
      }

  }


}

void GLEngine::rotate( const double z )
{
    glRotated( z, 0, 0, 1 );
}

void GLEngine::refresh_luts()
{
  QuadList::iterator q = _quads.begin();
  QuadList::iterator e = _quads.end();
  for ( ; q != e; ++q )
    {
      (*q)->clear_lut();
    }
}

/**
 * Clears the opengl canvas to a certain color
 *
 * @param r red component
 * @param g green component
 * @param b blue component
 * @param a alpha component
 */
void GLEngine::clear_canvas( float r, float g, float b, float a )
{
    CHECK_GL;
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    CHECK_GL;
    glClearColor(r, g, b, a );
    CHECK_GL;
    glClear( GL_STENCIL_BUFFER_BIT );
    CHECK_GL;
    glClear( GL_COLOR_BUFFER_BIT );
    CHECK_GL;
    glShadeModel( GL_FLAT );
    CHECK_GL;
}

void GLEngine::set_blend_function( int source, int dest )
{
  // So compositing works properly
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc( (GLenum) source, (GLenum) dest );
  CHECK_GL;
}

void GLEngine::color( uchar r, uchar g, uchar b, uchar a = 255 )
{
  glColor4ub( r, g, b, a );
}

void GLEngine::color( float r, float g, float b, float a = 1.0 )
{
  glColor4f( r, g, b, a );
}

bool GLEngine::init_fbo( ImageList& images )
{

   if ( ! _fboRenderBuffer ) return false;


   glGenTextures(1, &textureId);
   CHECK_GL;
   glBindTexture(GL_TEXTURE_2D, textureId);
   CHECK_GL;
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
   CHECK_GL;
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   CHECK_GL;
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   CHECK_GL;

   GLenum internalFormat = GL_RGBA32F_ARB;
   GLenum dataFormat = GL_RGBA;
   GLenum pixelType = GL_FLOAT;

   Image_ptr img = images.back();

   mrv::image_type_ptr pic = img->hires();
   if (!pic) return false;

   unsigned w = pic->width();
   unsigned h = pic->height();

   glTexImage2D(GL_TEXTURE_2D,
                0, // level
                internalFormat, // internal format
                w, h,
                0, // border
                dataFormat,  // texture data format
                pixelType, // texture pixel type
                NULL);    // texture pixel data
   CHECK_GL;

   glGenFramebuffers(1, &id);
   CHECK_GL;
   glBindFramebuffer(GL_FRAMEBUFFER, id);
   CHECK_GL;

   glGenRenderbuffers(1, &rid);
   CHECK_GL;
   glBindRenderbuffer( GL_RENDERBUFFER, rid );
   CHECK_GL;



   if ( w > GL_MAX_RENDERBUFFER_SIZE ) return false;
   if ( h > GL_MAX_RENDERBUFFER_SIZE ) return false;

   glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_STENCIL,
                          w, h );
   CHECK_GL;
   glBindRenderbuffer( GL_RENDERBUFFER, 0 );
   CHECK_GL;

   // attach a texture to FBO color attachement point
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, textureId, 0);
   CHECK_GL;

   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_RENDERBUFFER, rid);
   CHECK_GL;

   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                             GL_RENDERBUFFER, rid);
   CHECK_GL;

   GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
   if ( status != GL_FRAMEBUFFER_COMPLETE )
   {
      switch( status )
      {
         case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG_ERROR( _("Unsupported internal format") );
            return false;
         case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG_ERROR( _("Framebuffer incomplete: Attachment is NOT complete.") );
            return false;

         case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG_ERROR( _("Framebuffer incomplete: No image is attached to FBO.") );
            return false;
         case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG_ERROR( _("Framebuffer incomplete: Draw buffer." ) );
            return false;

         case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG_ERROR( _("Framebuffer incomplete: Read buffer.") );
            return false;
      }
   }
   return true;
}

void GLEngine::end_fbo( ImageList& images )
{
   if ( ! _fboRenderBuffer ) return;

   glBindTexture(GL_TEXTURE_2D, textureId);
   CHECK_GL;

   Image_ptr img = images.back();
   mrv::image_type_ptr pic = img->hires();
   if (!pic) return;

   unsigned w = pic->width();
   unsigned h = pic->height();
   glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
   CHECK_GL;
   glBindTexture(GL_TEXTURE_2D, 0);
   CHECK_GL;

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   CHECK_GL;
   glDeleteFramebuffers(1, &id);
   CHECK_GL;
   glDeleteRenderbuffers(1, &rid);
   CHECK_GL;

}

void GLEngine::draw_title( const float size,
                           const int y, const char* text )
{
  if ( !text ) return;

  void* font = GLUT_STROKE_MONO_ROMAN;

  glMatrixMode(GL_MODELVIEW);
  CHECK_GL;
  glPushMatrix();
  CHECK_GL;
  glLoadIdentity();
  CHECK_GL;

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  CHECK_GL;
  glEnable(GL_BLEND);
  CHECK_GL;
  glEnable(GL_LINE_SMOOTH);
  CHECK_GL;

  glLineWidth(4.0);
  CHECK_GL;

  int sum = 0;
  for (const char* p = text; *p; ++p)
      sum += glutStrokeWidth( font, *p );
  CHECK_GL;

  float x = ( float( _view->w() ) - float(sum) * size ) / 2.0f;

  float rgb[4];
  glGetFloatv( GL_CURRENT_COLOR, rgb );
  CHECK_GL;

  glColor4f( 0.f, 0.f, 0.f, 1.0f );
  glLoadIdentity();
  translate( x, GLfloat( y ), 0 );
  glScalef( size, size, 1.0 );
  for (const char* p = text; *p; ++p)
    glutStrokeCharacter( font, *p );
  CHECK_GL;

  glColor4f( rgb[0], rgb[1], rgb[2], rgb[3] );
  CHECK_GL;
  glLoadIdentity();
  CHECK_GL;
  translate( x-2, float(y+2), 0 );
  CHECK_GL;
  glScalef( size, size, 1.0 );
  CHECK_GL;
  for (const char* p = text; *p; ++p)
    glutStrokeCharacter( font, *p );
  CHECK_GL;

  glMatrixMode( GL_MODELVIEW );
  CHECK_GL;
  glPopMatrix();

  CHECK_GL;
  glDisable(GL_BLEND);
  CHECK_GL;
  glDisable(GL_LINE_SMOOTH);
  CHECK_GL;
  glLineWidth(1.0);
  CHECK_GL;
}

/**
 * Draw a line of text at a raster position
 *
 * @param x raster x pos
 * @param y raster y pos
 * @param s line of text to write
 */
void GLEngine::draw_text( const int x, const int y, const char* s )
{
    if (! sCharset ) return;

    glLoadIdentity();
    glRasterPos2i( x, y );

    glPushAttrib( GL_LIST_BIT | GL_DEPTH_TEST );
    glDisable( GL_DEPTH_TEST );

    glListBase(sCharset);
    glCallLists( GLsizei( strlen(s) ), GL_UNSIGNED_BYTE, s);

    glPopAttrib();
}

void GLEngine::draw_cursor( const double x, const double y )
{
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   double pr = 1.0;
   if ( _view->main()->uiPixelRatio->value() ) pr /= _view->pixel_ratio();

   double zoomX = _view->zoom();
   double zoomY = _view->zoom();

   double tw = double(texWidth)  / 2.0;
   double th = double(texHeight) / 2.0;

   double sw = ((double)_view->w() - texWidth  * zoomX) / 2;
   double sh = ((double)_view->h() - texHeight * zoomY) / 2;

   translate(_view->offset_x() * zoomX + sw,
                _view->offset_y() * zoomY + sh, 0);
   translate(tw * zoomX, th * zoomY, 0);

   glScaled(zoomX, zoomY * pr, 1.0);

   glColor4f( 1, 0, 0, 1 );

   glPointSize( float(_view->main()->uiPaint->uiPenSize->value()) );

   glBegin( GL_POINTS );
   glVertex2d( x, y );
   glEnd();
}

void GLEngine::draw_square_stencil( const int x, const int y,
                                    const int W, const int H)
{
    CHECK_GL;
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    CHECK_GL;
    glDepthMask( GL_FALSE );
    CHECK_GL;
    glColor4f( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable( GL_STENCIL_TEST );
    CHECK_GL;
    glStencilFunc( GL_ALWAYS, 0x1, 0xffffffff );
    CHECK_GL;
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
    CHECK_GL;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    double pr = 1.0;
    if ( _view->main()->uiPixelRatio->value() )
    {
        pr /= _view->pixel_ratio();
        glScaled( 1.0, pr, 1.0 );
    }


    glBegin( GL_QUADS );
    {
        glVertex2d(x, -y);
        glVertex2d(W, -y);
        glVertex2d(W, -H);
        glVertex2d(x, -H);
    }
    glEnd();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    // just draw where inside of the mask
    glStencilFunc(GL_EQUAL, 0x1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDisable(GL_BLEND);
}

inline
void GLEngine::set_matrix( const mrv::ImageView::FlipDirection flip,
                           const bool pixel_ratio )
{
    if ( _view->vr() ) return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //
    // Translate to center of screen
    //
    translate( double(_view->w())/2, double(_view->h())/2, 0 );


    //
    // Scale to zoom factor
    //
    glScaled( _view->zoom(), _view->zoom(), 1.0);


    //
    // Offset to user translation
    //
    translate( _view->offset_x(), _view->offset_y(), 0.0 );

    //
    // Handle flip
    //
    if ( flip != ImageView::kFlipNone )
    {
        float x = 1.0f, y = 1.0f;
        if ( flip & ImageView::kFlipVertical )   x = -1.0f;
        if ( flip & ImageView::kFlipHorizontal ) y = -1.0f;

        glScalef( x, y, 1.0f );
    }

    //
    // Handle pixel ratio
    //
    if ( pixel_ratio )
    {
        double pr = 1.0;
        if ( _view->main()->uiPixelRatio->value() ) pr /= _view->pixel_ratio();
        glScaled( 1.0, pr, 1.0 );
    }

    CHECK_GL;

}

/**
 * Draws the mask
 *
 * @param pct percent of mask to draw
 */
void GLEngine::draw_mask( const float pct )
{
  mrv::media fg = _view->foreground();
  if ( !fg ) return;



  Image_ptr img = fg->image();

  mrv::Recti dpw2 = img->display_window2();
  mrv::Recti dpw = img->display_window();

  if ( img->stereo_output() & CMedia::kStereoSideBySide )
  {
      dpw.w( dpw.w() + dpw2.w() );
  }
  else if ( img->stereo_output() & CMedia::kStereoTopBottom )
  {
      dpw.h( dpw.h() + dpw2.h() );
  }

  glColor3f( 0.0f, 0.0f, 0.0f );
  glDisable( GL_STENCIL_TEST );

  ImageView::FlipDirection flip = _view->flip();
  
  set_matrix( flip, true );

  double zdeg = img->rot_z();

  double x=0.0, y = 0.0;
  //zrot2offsets( x, y, img, flip, zdeg );

  
  glRotated( zdeg, 0, 0, 1 );
  translate( img->x() + x + dpw.x(), img->y() + y - dpw.y(), 0 );

  glScaled( dpw.w(), dpw.h(), 1.0 );
  translate( 0.5, -0.5, 0.0 );


  double aspect = (double) dpw.w() / (double) dpw.h();   // 1.3
  double target_aspect = 1.0 / pct;
  double amount = (0.5 - target_aspect * aspect / 2);

  //
  // Bottom mask
  //
  glBegin( GL_POLYGON );
  {
    glVertex2d( -0.5,  -0.5 + amount );
    glVertex2d(  0.5,  -0.5 + amount );
    glVertex2d(  0.5,  -0.5 );
    glVertex2d( -0.5,  -0.5 );
  }
  glEnd();

  //
  // Top mask
  //
  glBegin( GL_POLYGON );
  {
    glVertex2d( -0.5,  0.5 );
    glVertex2d(  0.5,  0.5 );
    glVertex2d(  0.5,  0.5 - amount );
    glVertex2d( -0.5,  0.5 - amount );
  }
  glEnd();


}


/**
 * Draw an overlay rectangle (like selection)
 *
 * @param r rectangle to draw
 */
void GLEngine::draw_rectangle( const mrv::Rectd& r,
                               const mrv::ImageView::FlipDirection flip,
                               const double zdeg )
{

    mrv::media fg = _view->foreground();
    if (!fg) return;

    Image_ptr img = fg->image();

    mrv::Recti daw = img->data_window();


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glPushAttrib( GL_STENCIL_TEST );
    glDisable( GL_STENCIL_TEST );

    set_matrix( flip, true );

    double x = 0.0, y = 0.0;
    zrot2offsets( x, y, img, flip, zdeg );

    glRotated( zdeg, 0, 0, 1 );
    translate( x + r.x(), y - r.y(), 0 );

    double rw = r.w();
    double rh = r.h();

    glLineWidth( 1.0 );

    // glEnable(GL_COLOR_LOGIC_OP);
    // glLogicOp(GL_XOR);

    glBegin(GL_LINE_LOOP);

    glVertex2d(0.0, 0.0);
    glVertex2d(rw,  0.0);
    glVertex2d(rw,  -rh);
    glVertex2d(0.0, -rh);

    glEnd();

    // glDisable(GL_COLOR_LOGIC_OP);

    glPopAttrib();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}

void GLEngine::draw_safe_area_inner( const double tw, const double th,
                                     const char* name )
{
  glLineWidth( 1.0 );

  glBegin(GL_LINE_LOOP);

  glVertex2d(-tw,-th);
  glVertex2d(tw, -th);
  glVertex2d(tw,  th);
  glVertex2d(-tw, th);

  glEnd();

  if ( name )
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      translate(tw+5, th, 0);
      glScalef( 0.1f, 0.1f, 1.0f );
      for (const char* p = name; *p; ++p)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }

}

/**
 * Draw an unfilled rectangle (for safe area display)
 *
 * @param percentX  percentage in X of area
 * @param percentY  percentage in Y of area
 */
void GLEngine::draw_safe_area( const double percentX, const double percentY,
                               const char* name )
{

    mrv::media fg = _view->foreground();
    if (!fg) return;
    
    mrv::media bg = _view->background();
    Image_ptr bimg = NULL;
    if (bg)
    {
        bimg = bg->image();
    }

    Image_ptr img = fg->image();

    mrv::Recti dpw2;
    if ( bimg) {
        dpw2 = bimg->display_window();
        dpw2.x( dpw2.x() + bimg->x() );
        dpw2.y( dpw2.y() - bimg->y() );
        dpw2.w( dpw2.w() * bimg->scale_x() );
        dpw2.h( dpw2.h() * bimg->scale_y() );
    }
    mrv::Recti dpw = img->display_window();

    ImageView::FlipDirection flip = _view->flip();

    glDisable( GL_STENCIL_TEST );

    set_matrix( flip, true );

    double x = dpw.x();
    double y = dpw.y();
    int tw = dpw.w() / 2.0;
    int th = dpw.h() / 2.0;

    dpw.merge( dpw2 );

    double zdeg = img->rot_z();
    
    zrot2offsets( x, y, img, flip, zdeg );
    
    glRotated( zdeg, 0, 0, 1 );
    translate(  x + tw, - y - th, 0 );

    
    tw *= percentX;
    th *= percentY;

    draw_safe_area_inner( tw, th, name );

    if ( img->stereo_output() & CMedia::kStereoSideBySide )
    {
        translate( dpw.w(), 0, 0 );
        draw_safe_area_inner( tw, th, name );
    }
    else if ( img->stereo_output() & CMedia::kStereoTopBottom )
    {
        translate( 0, -dpw.h(), 0 );
        draw_safe_area_inner( tw, th, name );
    }

}




inline double GLEngine::rot_y() const
{
    return _rotY;
}

inline double GLEngine::rot_x() const
{
    return _rotX;
}



inline void GLEngine::rot_x( double t )
{
    _rotX = t;
}

inline void GLEngine::rot_y( double t )
{
    _rotY = t;
}

void GLEngine::alloc_cubes( size_t num )
{
  size_t num_quads = _quads.size();
  _quads.reserve( num );
  for ( size_t q = num_quads; q < num; ++q )
    {
      mrv::GLCube* s = new mrv::GLCube( _view );
      _quads.push_back( s );
    }
}

void GLEngine::alloc_spheres( size_t num )
{
  size_t num_quads = _quads.size();
  _quads.reserve( num );
  for ( size_t q = num_quads; q < num; ++q )
    {
      mrv::GLSphere* s = new mrv::GLSphere( _view );
      _quads.push_back( s );
    }
}

void GLEngine::alloc_quads( size_t num )
{
  size_t num_quads = _quads.size();
  _quads.reserve( num );
  for ( size_t q = num_quads; q < num; ++q )
    {
      mrv::GLQuad* quad = new mrv::GLQuad( _view );
      _quads.push_back( quad );
    }
}


void GLEngine::draw_selection_marquee( const mrv::Rectd& r )
{
    Image_ptr img = _view->selected_image();
    if ( img == NULL ) return;

    ImageView::FlipDirection flip = _view->flip();

    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    if ( _view->action_mode() == ImageView::kMovePicture )
    {
        glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
    }
    else
    {
        glColor4f( 1.0f, 0.3f, 0.0f, 1.0f );
    }
    
    double zdeg = 0.0;
    if ( img ) zdeg = img->rot_z();

    draw_rectangle( r, _view->flip(), 0.0 );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glPushAttrib( GL_STENCIL_TEST );
    glDisable( GL_STENCIL_TEST );

    set_matrix( flip, true );


    mrv::Recti dpw = img->display_window();
    double x = 0.0, y = 0.0;
    if ( flip & ImageView::kFlipVertical )    x = (double)-dpw.w();
    if ( flip & ImageView::kFlipHorizontal )  y = (double) dpw.h();

    translate( x + r.x(), y - r.y(), 0 );

    if ( _view->action_mode() == ImageView::kScalePicture )
    {
        glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
    }
    else
    {
        glColor4f( 1.0f, 0.3f, 0.0f, 1.0f );
    }
    glBegin( GL_TRIANGLES );

    const double kSize = 20; //r.w() / 64.0;
    {
        // glVertex2d(  0.0,  0.0 );
        // glVertex2d( kSize,  0.0 );
        // glVertex2d(  0.0, -kSize );

        // glVertex2d(  r.w(),        0.0 );
        // glVertex2d(  r.w()-kSize,  0.0 );
        // glVertex2d(  r.w(),       -kSize );

        glVertex2d(  r.w(),       -r.h() );
        glVertex2d(  r.w()-kSize, -r.h() );
        glVertex2d(  r.w(),       -r.h()+kSize );

        // glVertex2d(  0.0,  -r.h() );
        // glVertex2d( kSize, -r.h() );
        // glVertex2d(  0.0,  -r.h()+kSize );
    }

    glEnd();

    // Draw Crosshair
    glColor4f( 1.0f, 0.3f, 0.0f, 1.0f );
    double rw = r.w() / 2.0;
    double rh = -r.h() / 2.0;
    glBegin( GL_LINES );
    {
        glVertex2d( rw, rh );
        glVertex2d( rw, rh + kSize );

        glVertex2d( rw, rh );
        glVertex2d( rw + kSize, rh );

        glVertex2d( rw, rh );
        glVertex2d( rw, rh - kSize );

        glVertex2d( rw, rh );
        glVertex2d( rw - kSize, rh );
    }
    glEnd();

    char buf[128];

    if ( _view->action_mode() == ImageView::kScalePicture )
    {
        double x = img->scale_x();
        double y = img->scale_y();
        sprintf( buf, "Scale: %g, %g", x, y );
    }
    else
    {
        int xi = round(img->x());
        int yi = round(img->y());
        sprintf( buf, "Pos: %d, %d", xi, yi );
    }
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    translate(rw+kSize, rh-kSize, 0);
    glScalef( 0.2f, 0.2f, 1.0f );
    for (const char* p = buf; *p; ++p)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GLEngine::draw_data_window( const mrv::Rectd& r )
{
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glColor4f( 0.5f, 0.5f, 0.5f, 0.0f );
    glLineStipple( 1, 0x00FF );
    glEnable( GL_LINE_STIPPLE );
    mrv::media fg = _view->foreground();
    CMedia* img = fg->image();
    double zdeg = 0.0;
    if ( img ) zdeg = img->rot_z();
    draw_rectangle( r, _view->flip(), 0.0 );
    glDisable( GL_LINE_STIPPLE );
    if ( _view->display_window() && !_view->vr() )
        glEnable( GL_STENCIL_TEST );
}

void GLEngine::translate( const double x, const double y, const double z )
{
   glTranslated( x, y, 0 );
}


void GLEngine::draw_images( ImageList& images )
{
    TRACE( "" );
    CHECK_GL;

  // Check if lut types changed since last time
  static int  RT_lut_old_algorithm = Preferences::kLutPreferCTL;
  static int ODT_lut_old_algorithm = Preferences::kLutPreferCTL;
  static int LUT_quality           = 2;
  static std::string ODT_ICC_old_profile;
  static std::string ODT_CTL_old_transform;
  static unsigned kNumStops = 10;

  if ( _view->use_lut() )
    {
      mrv::PreferencesUI* uiPrefs = _view->main()->uiPrefs;
      int RT_lut_algorithm = uiPrefs->RT_algorithm->value();
      int ODT_lut_algorithm = uiPrefs->ODT_algorithm->value();
      const char* ODT_ICC_profile = uiPrefs->uiODT_ICC_profile->text();
      int lut_quality = uiPrefs->uiLUT_quality->value();
      unsigned num_stops = (unsigned)uiPrefs->uiPrefsNumStops->value();

      // Check if there was a change effecting lut.
      if ( ( RT_lut_algorithm != RT_lut_old_algorithm ) ||
           ( ODT_lut_algorithm != ODT_lut_old_algorithm ) ||
           ( ODT_ICC_old_profile != ODT_ICC_profile ) ||
           ( ODT_CTL_old_transform != mrv::Preferences::ODT_CTL_transform ) ||
           ( LUT_quality != lut_quality ) ||
           ( kNumStops != num_stops) )
        {
          RT_lut_old_algorithm = RT_lut_algorithm;
          ODT_lut_old_algorithm = ODT_lut_algorithm;
          if ( ODT_ICC_profile )
            ODT_ICC_old_profile = ODT_ICC_profile;
          else
            ODT_ICC_old_profile.clear();

          ODT_CTL_old_transform = mrv::Preferences::ODT_CTL_transform;

          refresh_luts();

          if ( LUT_quality != lut_quality ||
               kNumStops != num_stops )
            {
                LUT_quality = lut_quality;
                kNumStops = num_stops;
                mrv::GLLut3d::clear();
            }
        }

    }


    TRACE( "" );
    if ( _view->normalize() )
    {
        minmax(); // calculate min-max
        minmax( _normMin, _normMax ); // retrieve them
    }


    TRACE( "" );
    size_t num_quads = 0;
    ImageList::iterator i = images.begin();
    ImageList::iterator e = images.end();

    for ( ; i != e; ++i )
    {
        const Image_ptr& img = *i;
        bool stereo = img->stereo_output() != CMedia::kNoStereo;
        if ( img->has_subtitle() ) num_quads += 1 + stereo;
        if ( img->has_picture()  ) ++num_quads;
        if ( stereo )    ++num_quads;
    }

    TRACE( "" );

    CHECK_GL;

    size_t num = _quads.size();
    if ( num_quads > num )
    {
        ImageView::VRType t = _view->vr();
        if ( t == ImageView::kVRSphericalMap )
        {
            alloc_spheres( num_quads );
        }
        else if ( t == ImageView::kVRCubeMap )
        {
            alloc_cubes( num_quads );
        }
        else
        {
            alloc_quads( num_quads );
        }
        for ( i = images.begin(); i != e; ++i )
        {
            const Image_ptr& img = *i;
            img->image_damage( img->image_damage() | CMedia::kDamageContents );
        }
    }



    glColor4f(1.0f,1.0f,1.0f,1.0f);



    TRACE( "" );

    double x = _view->spin_x();
    double y = _view->spin_y();
    if ( x >= 1000.0 )  // dummy value used to reset view
    {
        ImageView* v = const_cast< ImageView* >( _view );
        v->spin_x( 0.0 );
        v->spin_y( 0.0 );
        _rotX = _rotY = 0.0;
    }
    else
    {
        _rotX += x;
        _rotY += y;
    }

    QuadList::iterator q = _quads.begin();
    assert( q != _quads.end() );

    e = images.end();

    const Image_ptr& fg = images.back();
    const Image_ptr& bg = images.front();

    glDisable( GL_BLEND );
    CHECK_GL;

    for ( i = images.begin(); i != e; ++i, ++q )
    {
        const Image_ptr& img = *i;
        mrv::image_type_ptr pic = img->hires();
        if (!pic)  continue;


        CMedia::StereoOutput stereo = img->stereo_output();

        const boost::int64_t& frame = pic->frame();

        mrv::Recti dpw = img->display_window(frame);
        mrv::Recti daw = img->data_window(frame);


        if ( stereo & CMedia::kStereoRight )
        {
            dpw = img->display_window2(frame);
            daw = img->data_window2(frame);
        }

      // Handle background image size
        if ( fg != img && stereo == CMedia::kNoStereo )
        {
            mrv::PreferencesUI* uiPrefs = _view->main()->uiPrefs;
            if ( uiPrefs->uiPrefsResizeBackground->value() == 0 )
            {   // DO NOT SCALE BG IMAGE
                texWidth = dpw.w();
                texHeight = dpw.h();
                const mrv::Recti& dp = fg->display_window();
                daw.x( img->x() + daw.x() );
                daw.y( daw.y() - img->y() );
                dpw.x( daw.x() );
                dpw.y( daw.y() );
            }
            else
            {
                // NOT display_window(frame)
                const mrv::Recti& dp = fg->display_window();
                texWidth = dp.w();
                texHeight = dp.h();
            }
        }
        else
        {
            texWidth = daw.w();
            texHeight = daw.h();
        }

        if ( texWidth == 0 ) texWidth = fg->width();
        if ( texHeight == 0 ) texHeight = fg->height();

        texWidth *= img->scale_x();
        texHeight *= img->scale_y();
        
        ImageView::FlipDirection flip = _view->flip();

        set_matrix( flip, false );
        
        if ( flip && !_view->vr() )
        {
            const mrv::Recti& dp = fg->display_window();
            double x = 0.0, y = 0.0;
            if ( flip & ImageView::kFlipVertical )   x = (double)-dp.w();
            if ( flip & ImageView::kFlipHorizontal ) y = (double)dp.h();
            translate( x, y, 0.0f );
        }


        if ( dpw != daw && ! _view->vr() )
        {
            if ( _view->display_window() )
            {
                int x = img->x();
                int y = img->y();
                draw_square_stencil( dpw.x() - x, dpw.y() + y,
                                     dpw.w() + x, dpw.h() - y );
            }
           
            if ( _view->data_window()  )
            {
                double x = img->x(), y = -img->y();
                if ( stereo & CMedia::kStereoSideBySide )
                    x += dpw.w();
                else if ( stereo & CMedia::kStereoTopBottom )
                    y += dpw.h();
                mrv::Rectd r( daw.x() + x, daw.y() + y,
                              daw.w(), daw.h() );
                draw_data_window( r );
            }
        }

        glDisable( GL_BLEND );
        CHECK_GL;

        glMatrixMode(GL_MODELVIEW);
        CHECK_GL;
        glPushMatrix();
        CHECK_GL;

        if ( !_view->vr() )
        {
            glRotated( img->rot_z(), 0, 0, 1 );
            translate( img->x(), img->y(), 0 );
            translate( double(daw.x() - img->eye_separation()),
                       double(-daw.y()), 0 );
            CHECK_GL;

            if ( _view->main()->uiPixelRatio->value() )
                glScaled( double(texWidth),
                          double(texHeight) / _view->pixel_ratio(),
                          1.0 );
            else
                glScaled( double(texWidth), double(texHeight), 1.0 );

            CHECK_GL;
            translate( 0.5, -0.5, 0.0 );
            CHECK_GL;
        }

        GLQuad* quad = *q;
        quad->minmax( _normMin, _normMax );
        quad->image( img );
        // Handle rotation of cube/sphere
        quad->rot_x( _rotX );
        quad->rot_y( _rotY );

        if ( _view->use_lut() )
        {
            if ( img->image_damage() & CMedia::kDamageLut )
                quad->clear_lut();

            quad->lut( img );

            if ( stereo != CMedia::kNoStereo )
            {
                if ( img->image_damage() & CMedia::kDamageLut )
                    (*(q+1))->clear_lut();
                (*(q+1))->lut( img );
            }

            img->image_damage( img->image_damage() & ~CMedia::kDamageLut  );
        }

        if ( i+1 == e ) wipe_area();

        float g = img->gamma();

        int mask = 0;

        if ( stereo != CMedia::kNoStereo &&
             img->left() && img->right() )
        {
            if ( stereo & CMedia::kStereoRight )
            {
                pic = img->right();
                CMedia* right = img->right_eye();
                if ( right ) g = right->gamma();
            }
            else
            {
                pic = img->left();
            }


            if ( stereo & CMedia::kStereoAnaglyph )
                glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE );
            else
                glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
            CHECK_GL;

#ifdef USE_STEREO_GL
            if ( stereo & CMedia::kStereoOpenGL )
            {
                glDrawBuffer( GL_BACK_LEFT );
                CHECK_GL;
            }
#endif

            quad->mask( 0 );
            quad->mask_value( 10 );
            if ( stereo & CMedia::kStereoInterlaced )
            {
                if ( stereo == CMedia::kStereoInterlaced )
                    mask = 1; // odd even rows
                else if ( stereo == CMedia::kStereoInterlacedColumns )
                    mask = 2; // odd even columns
                else if ( stereo == CMedia::kStereoCheckerboard )
                    mask = 3; // checkerboard
                quad->mask( mask );
                quad->mask_value( 1 );
            }

            glDisable( GL_BLEND );
            CHECK_GL;
            if ( img->image_damage() & CMedia::kDamageContents )
            {
                if ( stereo & CMedia::kStereoRight )
                    quad->right( true );
                else
                    quad->right( false );
                quad->bind( pic );
            }
            quad->gamma( g );
            quad->draw( texWidth, texHeight );

            if ( img->has_subtitle() )
            {
                image_type_ptr sub = img->subtitle();
                if ( sub )
                {
                    glEnable( GL_BLEND );
                    glDisable( GL_SCISSOR_TEST );
                    ++q;
                    quad = *q;
                    quad->mask( 0 );
                    quad->mask_value( -10 );
                    quad->bind( sub );
                    quad->gamma( 1.0 );
                    // Handle rotation of cube/sphere
                    quad->rot_x( _rotX );
                    quad->rot_y( _rotY );
                    quad->draw( texWidth, texHeight );
                }
                img->image_damage( img->image_damage() &
                                   ~CMedia::kDamageSubtitle );
            }

            ++q;
            quad = *q;
            quad->minmax( _normMin, _normMax );
            quad->image( img );
            // Handle rotation of cube/sphere
            quad->rot_x( _rotX );
            quad->rot_y( _rotY );

            if ( stereo != CMedia::kStereoLeft &&
                 stereo != CMedia::kStereoRight )
            {
                CHECK_GL;
                glMatrixMode( GL_MODELVIEW );
                CHECK_GL;
                glPopMatrix();
                CHECK_GL;

                if ( stereo & CMedia::kStereoSideBySide )
                {
                    translate( dpw.w(), 0, 0 );
                }
                else if ( stereo & CMedia::kStereoTopBottom )
                    translate( 0, -dpw.h(), 0 );

                CHECK_GL;
                mrv::Recti dpw2 = img->display_window2(frame);
                mrv::Recti daw2 = img->data_window2(frame);

                if ( stereo & CMedia::kStereoRight )
                {
                    dpw2 = img->display_window(frame);
                    daw2 = img->data_window(frame);
                }


                CHECK_GL;
                glMatrixMode(GL_MODELVIEW);
                CHECK_GL;
                glPushMatrix();
                CHECK_GL;


                if ( dpw2 != daw2 )
                {
                    if ( _view->display_window() &&
                         ( !( stereo & CMedia::kStereoAnaglyph ) &&
                           !( stereo & CMedia::kStereoInterlaced ) &&
                           !( _view->vr() ) ) )
                    {
                        int x = img->x();
                        int y = img->y();
                        draw_square_stencil( dpw.x() + x,
                                             dpw.y() - y, dpw.w() - x,
                                             dpw.h() + y );
                    }

                    if ( _view->data_window() )
                    {
                        double x = img->x(), y = img->y();
                        if ( stereo & CMedia::kStereoSideBySide )
                            x += dpw.w();
                        else if ( stereo & CMedia::kStereoTopBottom )
                            y += dpw.h();

                        mrv::Rectd r( daw2.x() + x, daw2.y() - y,
                                      daw2.w(), daw2.h() );
                        draw_data_window( r );
                    }
                }

                g = img->gamma();

                if ( stereo & CMedia::kStereoRight )
                {
                    pic = img->left();
                }
                else
                {
                    pic = img->right();
                    CMedia* right = img->right_eye();
                    if ( right ) g = right->gamma();
                }

                if ( daw2.w() > 0 )
                {
                    texWidth = daw2.w();
                    texHeight = daw2.h();
                }
                else
                {
                    texWidth = pic->width();
                    texHeight = pic->height();
                }
                glRotated( img->rot_z(), 0, 0, 1 );

                translate( img->x(), img->y(), 0 );
                translate( float(daw2.x()), float(-daw2.y()), 0 );
                CHECK_GL;

                if ( _view->main()->uiPixelRatio->value() )
                    glScaled( double(texWidth),
                              double(texHeight) / _view->pixel_ratio(),
                              1.0 );
                else
                    glScaled( double(texWidth), double(texHeight), 1.0 );
                CHECK_GL;

   
                translate( 0.5, -0.5, 0 );
                CHECK_GL;

            }
        }
        else if ( img->hires() || img->has_subtitle() )
        {
            stereo = CMedia::kNoStereo;
            pic = img->hires();

            if ( shader_type() == kNone && img->stopped() &&
                 pic->pixel_type() != image_type::kByte )
            {
                pic = display( pic, img );
            }

      }

      if ( stereo & CMedia::kStereoAnaglyph )
          glColorMask( GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE );
      else
          glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

#ifdef USE_STEREO_GL
      if ( stereo & CMedia::kStereoOpenGL )
      {
          glDrawBuffer( GL_BACK_RIGHT );
          CHECK_GL;
      }
#endif

      quad->mask( 0 );
      quad->mask_value( 10 );
      if ( stereo & CMedia::kStereoInterlaced )
      {
          if ( stereo == CMedia::kStereoInterlaced )
              mask = 1; // odd even rows
          else if ( stereo == CMedia::kStereoInterlacedColumns )
              mask = 2; // odd even columns
          else if ( stereo == CMedia::kStereoCheckerboard )
              mask = 3; // checkerboard
          quad->mask( mask );
          quad->mask_value( 0 );
          glEnable( GL_BLEND );
      }

      if ( fg == img && bg != fg &&
           _view->show_background() ) glEnable( GL_BLEND );

      if ( img->image_damage() & CMedia::kDamageContents )
      {
          if ( stereo )
          {
              bool rightView = false;
              if ( stereo & CMedia::kStereoRight )
              {
                  rightView = false;
              }
              else
              {
                  rightView = true;
              }

              if ( stereo == CMedia::kStereoRight )
                  rightView = true;
              else if ( stereo == CMedia::kStereoLeft )
                  rightView = false;
              quad->right( rightView );
          }
          quad->bind( pic );
          img->image_damage( img->image_damage() & ~CMedia::kDamageContents );
      }

      quad->gamma( g );
      quad->draw( texWidth, texHeight );
 
      if ( ( _view->action_mode() == ImageView::kMovePicture ||
             _view->action_mode() == ImageView::kScalePicture ) &&
           _view->selected_image() == img )
      {
          mrv::Rectd r( img->x() + dpw.x(), dpw.y() - img->y(),
                        dpw.w() * img->scale_x(), dpw.h() * img->scale_y() );
          draw_selection_marquee( r );
      }


      if ( img->has_subtitle() )
        {
            image_type_ptr sub = img->subtitle();
            if ( sub )
            {
                glEnable( GL_BLEND );
                glDisable( GL_SCISSOR_TEST );
                ++q;
                quad = *q;
                quad->mask( 0 );
                quad->mask_value( -10 );
                quad->bind( sub );
                quad->gamma( 1.0 );
                // Handle rotation of cube/sphere
                quad->rot_x( _rotX );
                quad->rot_y( _rotY );
                quad->draw( texWidth, texHeight );
           }
           img->image_damage( img->image_damage() & ~CMedia::kDamageSubtitle );
        }

      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();


    }

  glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  glDisable( GL_SCISSOR_TEST );
  glDisable( GL_BLEND );
  FLUSH_GL_ERRORS;
}

void GLEngine::draw_shape( GLShape* const shape )
{
   double zoomX = _view->zoom();
    if ( _view->ghost_previous() )
    {
        if ( shape->frame == _view->frame() - 1 )
        {
            float a = shape->a;
            shape->a *= 0.25f;
            shape->draw(zoomX);
            shape->a = a;
            return;
         }
      }

      if ( _view->ghost_next() )
      {
         if ( shape->frame == _view->frame() + 1 )
         {
            float a = shape->a;
            shape->a *= 0.25f;
            shape->draw(zoomX);
            shape->a = a;
            return;
         }
      }

      if ( shape->frame == MRV_NOPTS_VALUE ||
           shape->frame == _view->frame() )
      {
         shape->draw(zoomX);
      }
}


void GLEngine::draw_annotation( const GLShapeList& shapes )
{
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   double pr = 1.0;
   if ( _view->main()->uiPixelRatio->value() ) pr /= _view->pixel_ratio();

   double zoomX = _view->zoom();
   double zoomY = _view->zoom();

   double tw = double( texWidth  ) / 2.0;
   double th = double( texHeight ) / 2.0;

   double sw = ((double)_view->w() - texWidth  * zoomX) / 2;
   double sh = ((double)_view->h() - texHeight * zoomY) / 2;

   translate( (tw + _view->offset_x()) * zoomX + sw,
              (th + _view->offset_y()) * zoomY + sh, 0);

   glScaled(zoomX, zoomY * pr, 1.0f);


   glClear(GL_STENCIL_BUFFER_BIT);

   glEnable( GL_STENCIL_TEST );

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable( GL_LINE_SMOOTH );
   glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

   {
       GLShapeList::const_reverse_iterator i = shapes.rbegin();
       GLShapeList::const_reverse_iterator e = shapes.rend();


       for ( ; i != e; ++i )
       {
           GLShape* shape = (*i).get();
           draw_shape( shape );
       }

   }

   glDisable(GL_BLEND);
   glDisable(GL_STENCIL_TEST);
}


void GLEngine::wipe_area()
{
  int  w = _view->w();
  int  h = _view->h();

  if ( _view->wipe_direction() == ImageView::kNoWipe )
     return;
  else if ( _view->wipe_direction() & ImageView::kWipeVertical )
  {
      w = (int) ( (float) w * _view->wipe_amount() );
  }
  else if ( _view->wipe_direction() & ImageView::kWipeHorizontal )
  {
      h = (int) ( (float) h * _view->wipe_amount() );
  }
  else
  {
     LOG_ERROR( _("Unknown wipe direction") );
  }

  glEnable( GL_SCISSOR_TEST );

  int  x = 0;
  int  y = 0;
  glScissor( x, y, w, h );
}


namespace {
const char* ARBFP1Shader =
"!!ARBfp1.0"
"# cgc version 3.1.0013, build date Apr 24 2012"
"# command line args: -I/media/gga/Datos/code/applications/mrViewer/shaders -profile arbfp1"
"# source file: rgba.cg"
"#vendor NVIDIA Corporation"
"#version 3.1.0.13"
"#profile arbfp1"
"#program main"
"#semantic main.fgImage : TEXUNIT0"
"#semantic main.lut : TEXUNIT3"
"#semantic main.mask"
"#semantic main.mask_value"
"#semantic main.height"
"#semantic main.width"
"#semantic main.gain"
"#semantic main.gamma"
"#semantic main.channel"
"#semantic main.premult"
"#semantic main.unpremult"
"#semantic main.enableNormalization"
"#semantic main.normMin"
"#semantic main.normSpan"
"#semantic main.enableLut"
"#semantic main.lutF"
"#semantic main.lutMin"
"#semantic main.lutMax"
"#semantic main.scale"
"#semantic main.offset"
"#semantic main.lutM"
"#semantic main.lutT"
"#var float2 tc : $vin.TEXCOORD0 : TEX0 : 0 : 1"
"#var sampler2D fgImage : TEXUNIT0 : texunit 0 : 1 : 1"
"#var sampler3D lut : TEXUNIT3 : texunit 3 : 2 : 1"
"#var int mask :  : c[0] : 3 : 1"
"#var int mask_value :  : c[1] : 4 : 1"
"#var int height :  : c[2] : 5 : 1"
"#var int width :  : c[3] : 6 : 1"
"#var half gain :  : c[4] : 7 : 1"
"#var half gamma :  : c[5] : 8 : 1"
"#var int channel :  : c[6] : 9 : 1"
"#var bool premult :  : c[7] : 10 : 1"
"#var bool unpremult :  : c[8] : 11 : 1"
"#var bool enableNormalization :  : c[9] : 12 : 1"
"#var half normMin :  : c[10] : 13 : 1"
"#var half normSpan :  : c[11] : 14 : 1"
"#var bool enableLut :  : c[12] : 15 : 1"
"#var bool lutF :  : c[13] : 16 : 1"
"#var half lutMin :  : c[14] : 17 : 1"
"#var half lutMax :  : c[15] : 18 : 1"
"#var half scale :  : c[16] : 19 : 1"
"#var half offset :  : c[17] : 20 : 1"
"#var half lutM :  : c[18] : 21 : 1"
"#var half lutT :  : c[19] : 22 : 1"
"#var float4 main.pixel : $vout.COLOR : COL : -1 : 1"
"#const c[20] = 0.5 0.33333334 1 0"
"#const c[21] = 2 3 1000 4"
"#const c[22] = 5 6 0.00010001659 2.718282"
"#const c[23] = 2.71875 0.69335938"
"#default mask = 0"
"#default mask_value = 0"
"#default height = 256"
"#default width = 256"
"#default gain = 1"
"#default gamma = 0.44995117"
"#default channel = 0"
"#default premult = 0"
"#default unpremult = 0"
"#default enableNormalization = 0"
"#default normMin = 0"
"#default normSpan = 1"
"#default enableLut = 0"
"#default lutF = 0"
"#default scale = 1"
"#default offset = 0"
"PARAM c[24] = { program.local[0..19],"
"		{ 0.5, 0.33333334, 1, 0 },"
"		{ 2, 3, 1000, 4 },"
"		{ 5, 6, 0.00010001659, 2.718282 },"
"		{ 2.71875, 0.69335938 } };"
"TEMP R0;"
"TEMP R1;"
"TEMP R2;"
"TEMP R3;"
"TEMP R4;"
"TEMP R5;"
"TEMP R6;"
"TEMP R7;"
"TEX R0, fragment.texcoord[0], texture[0], 2D;"
"RCP R3.w, c[13].x;"
"RCP R1.w, c[11].x;"
"ADD R1.xyz, R0, -c[10].x;"
"MUL R1.xyz, R1, R1.w;"
"CMP R0.xyz, -c[9].x, R1, R0;"
"MUL R3.xyz, R0, c[4].x;"
"MIN R0.xyz, R3, c[15].x;"
"MAX R0.xyz, R0, c[14].x;"
"MOV R1.w, c[19].x;"
"LG2 R0.x, R0.x;"
"LG2 R0.y, R0.y;"
"LG2 R0.z, R0.z;"
"MUL R0.xyz, R0, c[18].x;"
"MAD R0.xyz, R0, c[23].y, R1.w;"
"MUL R1.xyz, R0, c[13].x;"
"FLR R0.xyz, R1;"
"ADD R4.xyz, -R0, R1;"
"ADD R2.xyz, R0, c[20].z;"
"MUL R7.xyz, R3.w, R2;"
"MUL R2.xyz, R0, R3.w;"
"TEX R1.xyz, R7, texture[3], 3D;"
"ADD R2.w, -R4.x, c[20].z;"
"ADD R3.w, -R4.y, c[20].z;"
"MUL R1.xyz, R4.x, R1;"
"MOV R0.x, R2;"
"MOV R0.yz, R7;"
"TEX R0.xyz, R0, texture[3], 3D;"
"MAD R1.xyz, R2.w, R0, R1;"
"MUL R5.xyz, R4.y, R1;"
"TEX R1.xyz, R2, texture[3], 3D;"
"MOV R0.y, R2;"
"MOV R0.xz, R7;"
"TEX R0.xyz, R0, texture[3], 3D;"
"MUL R0.xyz, R4.x, R0;"
"MAD R6.xyz, R2.w, R1, R0;"
"MAD R5.xyz, R6, R3.w, R5;"
"MOV R6.yz, R2;"
"MOV R6.x, R7;"
"MOV R0.z, R2;"
"MOV R0.xy, R7;"
"TEX R0.xyz, R0, texture[3], 3D;"
"MOV R2.y, R7;"
"MUL R0.xyz, R4.x, R0;"
"TEX R2.xyz, R2, texture[3], 3D;"
"MAD R0.xyz, R2, R2.w, R0;"
"TEX R6.xyz, R6, texture[3], 3D;"
"MUL R2.xyz, R4.x, R6;"
"MAD R1.xyz, R2.w, R1, R2;"
"MUL R0.xyz, R4.y, R0;"
"MAD R0.xyz, R1, R3.w, R0;"
"MUL R1.xyz, R4.z, R5;"
"ADD R2.y, -R4.z, c[20].z;"
"MAD R0.xyz, R0, R2.y, R1;"
"MOV R2.x, c[13];"
"MUL R1.x, R2, c[12];"
"CMP R2.xyz, -R1.x, R0, R3;"
"POW R0.x, c[23].x, R2.x;"
"POW R0.z, c[23].x, R2.z;"
"POW R0.y, c[23].x, R2.y;"
"CMP R0.xyz, -R1.x, R0, R2;"
"MIN R1.xyz, R0, c[15].x;"
"MAX R1.xyz, R1, c[14].x;"
"LG2 R1.x, R1.x;"
"LG2 R1.z, R1.z;"
"LG2 R1.y, R1.y;"
"MUL R2.xyz, R1, c[18].x;"
"ABS R1.x, c[13];"
"MAD R2.xyz, R2, c[23].y, R1.w;"
"CMP R1.x, -R1, c[20].w, c[20].z;"
"MUL R1.w, R1.x, c[12].x;"
"CMP R1.xyz, -R1.w, R2, R0;"
"MOV R2.w, c[17].x;"
"MAD R0.xyz, R1, c[16].x, R2.w;"
"TEX R0.xyz, R0, texture[3], 3D;"
"POW R0.x, c[22].w, R0.x;"
"POW R0.z, c[22].w, R0.z;"
"POW R0.y, c[22].w, R0.y;"
"CMP R0.xyz, -R1.w, R0, R1;"
"RCP R1.y, R0.w;"
"SLT R1.x, c[22].z, R0.w;"
"MUL R2.xyz, R0, R1.y;"
"MUL R1.x, R1, c[8];"
"CMP R0.xyz, -R1.x, R2, R0;"
"MOV R2.xyz, c[21].xyww;"
"ADD R1.y, -R2.x, c[6].x;"
"MOV R1.z, c[20];"
"ADD R1.x, -R1.z, c[6];"
"ABS R1.y, R1;"
"ADD R2.x, -R2, c[0];"
"ADD R1.z, -R1, c[0].x;"
"ABS R1.z, R1;"
"ABS R1.x, R1;"
"CMP R1.y, -R1, c[20].w, c[20].z;"
"ADD R2.z, -R2, c[6].x;"
"ABS R2.x, R2;"
"POW R0.y, R0.y, c[5].x;"
"POW R0.z, R0.z, c[5].x;"
"POW R0.x, R0.x, c[5].x;"
"CMP R0.xyz, -R1.x, R0, R0.x;"
"CMP R1.x, -R1, c[20].w, c[20].z;"
"ABS R1.x, R1;"
"CMP R1.x, -R1, c[20].w, c[20].z;"
"MUL R1.w, R1.x, R1.y;"
"CMP R0.xyz, -R1.w, R0.y, R0;"
"ADD R1.w, -R2.y, c[6].x;"
"ADD R2.y, -R2, c[0].x;"
"ABS R3.y, R2;"
"CMP R2.y, -R2.x, c[20].w, c[20].z;"
"CMP R3.w, -R3.y, c[20], c[20].z;"
"MUL R3.y, fragment.texcoord[0], c[2].x;"
"ABS R1.y, R1;"
"CMP R1.y, -R1, c[20].w, c[20].z;"
"ABS R1.w, R1;"
"MUL R1.x, R1, R1.y;"
"CMP R1.w, -R1, c[20], c[20].z;"
"MUL R1.y, R1.x, R1.w;"
"CMP R0.xyz, -R1.y, R0.z, R0;"
"ABS R1.y, R1.w;"
"ABS R1.w, R2.z;"
"CMP R1.y, -R1, c[20].w, c[20].z;"
"MUL R2.z, R1.x, R1.y;"
"CMP R1.w, -R1, c[20], c[20].z;"
"MUL R1.x, R2.z, R1.w;"
"CMP R0.xyz, -R1.x, R0.w, R0;"
"MOV R1.xy, c[22];"
"ADD R2.w, -R1.x, c[6].x;"
"ABS R1.x, R1.w;"
"ABS R1.w, R2;"
"CMP R1.x, -R1, c[20].w, c[20].z;"
"CMP R2.x, -R1.z, c[20].w, c[20].z;"
"FLR R4.y, R3;"
"MUL R3.x, R0, c[20];"
"CMP R1.w, -R1, c[20], c[20].z;"
"MUL R1.x, R2.z, R1;"
"MUL R2.z, R1.x, R1.w;"
"CMP R0.x, -R2.z, R3, R0;"
"MAD R2.w, R0, c[20].x, R0.x;"
"CMP R0.x, -R2.z, R2.w, R0;"
"ADD R2.z, R0.x, R0.y;"
"ADD R2.z, R2, R0;"
"ABS R3.x, R2.y;"
"ABS R2.w, R2.x;"
"CMP R2.w, -R2, c[20], c[20].z;"
"CMP R3.x, -R3, c[20].w, c[20].z;"
"MUL R3.z, R2.w, R3.x;"
"MUL R3.x, fragment.texcoord[0], c[3];"
"FLR R4.x, R3;"
"ADD R4.z, R4.x, R4.y;"
"MUL R3.z, R3, R3.w;"
"SLT R3.w, R4.x, -R4.y;"
"MUL R4.z, R4, c[20].x;"
"ABS R4.x, R4.z;"
"ABS R3.w, R3;"
"FRC R4.x, R4;"
"CMP R3.w, -R3, c[20], c[20].z;"
"MUL R2.y, R2.w, R2;"
"MUL R4.x, R4, c[21];"
"MUL R3.w, R3.z, R3;"
"CMP R3.w, -R3, R4.x, -R4.x;"
"SLT R4.x, R3, c[20].w;"
"ABS R2.w, R4.x;"
"SLT R4.x, R3.y, c[20].w;"
"CMP R2.w, -R2, c[20], c[20].z;"
"ABS R4.x, R4;"
"MUL R2.w, R2.y, R2;"
"CMP R4.x, -R4, c[20].w, c[20].z;"
"MUL R4.y, R3, c[20].x;"
"MUL R3.y, R2.x, R4.x;"
"MUL R2.x, R3, c[20];"
"ABS R4.x, R4.y;"
"FRC R3.x, R4;"
"ABS R2.x, R2;"
"MUL R3.x, R3, c[21];"
"CMP R3.x, -R3.y, R3, -R3;"
"ABS R3.y, R3.x;"
"FRC R2.x, R2;"
"MUL R2.x, R2, c[21];"
"CMP R2.x, -R2.w, R2, -R2;"
"SLT R2.w, R3.x, c[20];"
"FLR R3.y, R3;"
"CMP R3.x, -R2.w, -R3.y, R3.y;"
"ABS R2.w, R2.x;"
"CMP R3.x, -R1.z, c[21].z, R3;"
"SLT R1.z, R2.x, c[20].w;"
"FLR R2.w, R2;"
"CMP R1.z, -R1, -R2.w, R2.w;"
"CMP R2.x, -R2.y, R1.z, R3;"
"SLT R1.z, R3.w, c[20];"
"CMP R1.z, -R3, R1, R2.x;"
"ADD R2.x, -R1.y, c[6];"
"ABS R1.y, R1.w;"
"CMP R1.y, -R1, c[20].w, c[20].z;"
"ABS R1.w, R2.x;"
"MUL R1.x, R1, R1.y;"
"CMP R1.w, -R1, c[20], c[20].z;"
"MUL R1.x, R1, R1.w;"
"MUL R2.z, R2, c[20].y;"
"CMP R0.xyz, -R1.x, R2.z, R0;"
"ADD R1.x, R1.z, -c[1];"
"ABS R1.x, R1;"
"CMP R0, -R1.x, R0, c[20].w;"
"MUL R1.xyz, R0, R0.w;"
"CMP result.color.xyz, -c[7].x, R1, R0;"
"MOV result.color.w, R0;"
"END"
"# 205 instructions, 8 R-regs";

const char* NVShader =
"!!FP1.0"
"# cgc version 3.1.0013, build date Apr 24 2012"
"# command line args: -I/media/gga/Datos/code/applications/mrViewer/shaders -profile fp30"
"# source file: rgba.cg"
"#vendor NVIDIA Corporation"
"#version 3.1.0.13"
"#profile fp30"
"#program main"
"#semantic main.fgImage : TEXUNIT0"
"#semantic main.lut : TEXUNIT3"
"#semantic main.mask"
"#semantic main.mask_value"
"#semantic main.height"
"#semantic main.width"
"#semantic main.gain"
"#semantic main.gamma"
"#semantic main.channel"
"#semantic main.premult"
"#semantic main.unpremult"
"#semantic main.enableNormalization"
"#semantic main.normMin"
"#semantic main.normSpan"
"#semantic main.enableLut"
"#semantic main.lutF"
"#semantic main.lutMin"
"#semantic main.lutMax"
"#semantic main.scale"
"#semantic main.offset"
"#semantic main.lutM"
"#semantic main.lutT"
"#var float2 tc : $vin.TEXCOORD0 : TEX0 : 0 : 1"
"#var sampler2D fgImage : TEXUNIT0 : texunit 0 : 1 : 1"
"#var sampler3D lut : TEXUNIT3 : texunit 3 : 2 : 1"
"#var int mask :  : mask : 3 : 1"
"#var int mask_value :  : mask_value : 4 : 1"
"#var int height :  : height : 5 : 1"
"#var int width :  : width : 6 : 1"
"#var half gain :  : gain : 7 : 1"
"#var half gamma :  : gamma : 8 : 1"
"#var int channel :  : channel : 9 : 1"
"#var bool premult :  : premult : 10 : 1"
"#var bool unpremult :  : unpremult : 11 : 1"
"#var bool enableNormalization :  : enableNormalization : 12 : 1"
"#var half normMin :  : normMin : 13 : 1"
"#var half normSpan :  : normSpan : 14 : 1"
"#var bool enableLut :  : enableLut : 15 : 1"
"#var bool lutF :  : lutF : 16 : 1"
"#var half lutMin :  : lutMin : 17 : 1"
"#var half lutMax :  : lutMax : 18 : 1"
"#var half scale :  : scale : 19 : 1"
"#var half offset :  : offset : 20 : 1"
"#var half lutM :  : lutM : 21 : 1"
"#var half lutT :  : lutT : 22 : 1"
"#var half4 main.pixel : $vout.COLOR : COL : -1 : 1"
"#default mask = 0"
"#default mask_value = 0"
"#default height = 256"
"#default width = 256"
"#default gain = 1"
"#default gamma = 0.44995117"
"#default channel = 0"
"#default premult = 0"
"#default unpremult = 0"
"#default enableNormalization = 0"
"#default normMin = 0"
"#default normSpan = 1"
"#default enableLut = 0"
"#default lutF = 0"
"#default scale = 1"
"#default offset = 0"
"DECLARE enableNormalization = {0};"
"DECLARE normMin = {0};"
"DECLARE normSpan = {1};"
"DECLARE gain = {1};"
"DECLARE enableLut = {0};"
"DECLARE lutF = {0};"
"DECLARE lutMax;"
"DECLARE lutMin;"
"DECLARE lutT;"
"DECLARE lutM;"
"DECLARE scale = {1};"
"DECLARE offset = {0};"
"DECLARE unpremult = {0};"
"DECLARE gamma = {0.44995117};"
"DECLARE channel = {0};"
"DECLARE mask = {0};"
"DECLARE height = {256};"
"DECLARE width = {256};"
"DECLARE mask_value = {0};"
"DECLARE premult = {0};"
"TEX   H1, f[TEX0], TEX0, 2D;"
"ADDH  H0.xyz, H1, -normMin.x;"
"MULR  R1.x, f[TEX0], width;"
"MULR  R1.y, f[TEX0], height.x;"
"RCPH  H0.w, normSpan.x;"
"MOVXC RC.x, enableNormalization;"
"MULH  H1.xyz(NE.x), H0, H0.w;"
"MULH  H1.xyz, H1, gain.x;"
"MINH  H0.xyz, H1, lutMax.x;"
"MAXH  H0.xyz, H0, lutMin.x;"
"MOVH  H2.w, lutT.x;"
"RCPH  H3.w, lutF.x;"
"MOVR  R0.w, {3}.x;"
"LG2H  H0.x, H0.x;"
"LG2H  H0.z, H0.z;"
"LG2H  H0.y, H0.y;"
"MULH  H0.xyz, H0, lutM.x;"
"MADH  H0.xyz, H0, {0.69335938}.x, H2.w;"
"MULH  H0.xyz, H0, lutF.x;"
"FLRH  H4.xyz, H0;"
"ADDH  H2.xyz, -H4, H0;"
"ADDH  H0.xyz, H4, {1}.x;"
"MULH  H0.xyw, H3.w, H0.yzzx;"
"MULH  H7.xyz, H4, H3.w;"
"TEX   H5.xyz, H0.wxyw, TEX3, 3D;"
"ADDH  H3.xyz, -H2, {1}.x;"
"MULH  H5.xyz, H2.x, H5;"
"MOVH  H4.yz, H0.xxyw;"
"MOVH  H4.x, H7;"
"TEX   H4.xyz, H4, TEX3, 3D;"
"MADH  H4.xyz, H3.x, H4, H5;"
"MULH  H6.xyz, H2.y, H4;"
"MOVH  H4.xy, H0.wxzw;"
"MOVH  H4.z, H7;"
"TEX   H4.xyz, H4, TEX3, 3D;"
"MULH  H5.xyz, H2.x, H4;"
"MOVH  H4.y, H0.x;"
"MOVH  H0.xz, H0.wyyw;"
"MOVH  H4.xz, H7;"
"TEX   H4.xyz, H4, TEX3, 3D;"
"MADH  H4.xyz, H4, H3.x, H5;"
"MOVH  H0.y, H7;"
"TEX   H0.xyz, H0, TEX3, 3D;"
"MULH  H5.xyz, H2.x, H0;"
"TEX   H0.xyz, H7, TEX3, 3D;"
"MADH  H5.xyz, H3.x, H0, H5;"
"MADH  H5.xyz, H3.y, H5, H6;"
"MOVH  H6.x, H0.w;"
"MOVX  H0.w, lutF.x;"
"MULXC HC.x, H0.w, enableLut;"
"MOVH  H6.yz, H7;"
"TEX   H6.xyz, H6, TEX3, 3D;"
"MULH  H6.xyz, H2.x, H6;"
"MULH  H4.xyz, H2.y, H4;"
"MADH  H0.xyz, H3.x, H0, H6;"
"MOVX  H0.w, {0}.x;"
"MADH  H0.xyz, H0, H3.y, H4;"
"MULH  H2.xyz, H2.z, H5;"
"MADH  H1.xyz(NE.x), H0, H3.z, H2;"
"MOVH  H2.xyz, H1;"
"POWH  H0.x, {2.71875}.x, H1.x;"
"POWH  H0.y, {2.71875}.x, H1.y;"
"POWH  H0.z, {2.71875}.x, H1.z;"
"MOVH  H2.xyz(NE.x), H0;"
"MINH  H0.xyz, H2, lutMax.x;"
"MAXH  H0.xyz, H0, lutMin.x;"
"SEQX  H0.w, lutF.x, H0;"
"MOVH  H1.xyz, H2;"
"MULXC HC.x, H0.w, enableLut;"
"LG2H  H0.x, H0.x;"
"LG2H  H0.z, H0.z;"
"LG2H  H0.y, H0.y;"
"MULH  H0.xyz, H0, lutM.x;"
"MADH  H1.xyz(NE.x), H0, {0.69335938}.x, H2.w;"
"MOVH  H0.x, offset;"
"MADH  H0.xyz, H1, scale.x, H0.x;"
"TEX   R0.xyz, H0, TEX3, 3D;"
"POWR  H0.x, {2.718282}.x, R0.x;"
"POWR  H0.y, {2.718282}.x, R0.y;"
"POWR  H0.z, {2.718282}.x, R0.z;"
"MOVH  H1.xyz(NE.x), H0;"
"MOVR  R0.x, {1};"
"MOVR  R0.y, {2}.x;"
"SGTH  H0.x, H1.w, {0.00010001659};"
"SEQR  H2.x, channel, R0.y;"
"MULXC HC.x, H0, unpremult;"
"RCPH  H0.y, H1.w;"
"MULH  H1.xyz(NE.x), H1, H0.y;"
"SEQR  H0.w, channel.x, R0.x;"
"MOVXC RC.x, H0.w;"
"POWH  H0.x, H1.x, gamma.x;"
"POWH  H0.z, H1.z, gamma.x;"
"POWH  H0.y, H1.y, gamma.x;"
"MOVH  H1.xyz, H0;"
"MOVH  H1.xyz(NE.x), H0.x;"
"SEQX  H0.w, H0, {0}.x;"
"MOVH  H0.xyz, H1;"
"MULXC HC.x, H0.w, H2;"
"MOVH  H0.xyz(NE.x), H1.y;"
"MOVH  H1.xyz, H0;"
"SEQX  H0.x, H2, {0};"
"MULX  H0.x, H0.w, H0;"
"SEQR  H0.y, channel.x, R0.w;"
"MULXC HC.x, H0, H0.y;"
"SEQX  H0.y, H0, {0}.x;"
"MOVH  H1.xyz(NE.x), H0.z;"
"MOVR  R0.z, {4}.x;"
"SEQR  H0.z, channel.x, R0;"
"MULX  H0.y, H0.x, H0;"
"MULXC HC.x, H0.y, H0.z;"
"MOVH  H1.xyz(NE.x), H1.w;"
"MOVR  R0.z, {5}.x;"
"SEQR  H2.x, channel, R0.z;"
"SEQX  H0.z, H0, {0}.x;"
"MULX  H0.w, H0.y, H0.z;"
"MOVH  H0.x, H1;"
"MULXC HC.x, H0.w, H2;"
"MULH  H0.x(NE), H1, {0.5};"
"MADH  H0.x(NE), H1.w, {0.5}, H0;"
"ADDH  H0.y, H0.x, H1;"
"ADDH  H1.x, H0.y, H1.z;"
"MOVH  H0.yz, H1;"
"SEQX  H1.y, H2.x, {0}.x;"
"MULX  H0.w, H0, H1.y;"
"MOVR  R0.z, {6}.x;"
"SEQR  H1.z, channel.x, R0;"
"MULXC HC.x, H0.w, H1.z;"
"SLTR  H2.x, R1, {0};"
"MULH  H0.xyz(NE.x), H1.x, {0.33333334}.x;"
"SEQR  H0.w, mask.x, R0.x;"
"MULR  R0.z, R1.x, {0.5}.x;"
"SEQR  H1.y, mask.x, R0;"
"SEQX  H1.x, H0.w, {0};"
"MULX  H1.z, H1.x, H1.y;"
"FRCR  R0.x, |R0.z|;"
"MULR  R0.y, R0.x, {2}.x;"
"SEQX  H2.x, H2, {0};"
"MULXC HC.x, H1.z, H2;"
"MOVR  R0.x, -R0.y;"
"MOVR  R0.x(NE), R0.y;"
"FLRR  R0.y, |R0.x|;"
"MOVRC RC.x, R0;"
"MOVR  R0.z, R0.y;"
"MULR  R0.x, R1.y, {0.5};"
"FRCR  R0.x, |R0|;"
"SLTR  H2.x, R1.y, {0};"
"SEQX  H1.y, H1, {0}.x;"
"MOVR  R0.z(LT.x), -R0.y;"
"MULR  R0.x, R0, {2};"
"SEQX  H2.x, H2, {0};"
"MOVR  R0.y, -R0.x;"
"MULXC HC.x, H0.w, H2;"
"MOVR  R0.y(NE.x), R0.x;"
"FLRR  R1.z, |R0.y|;"
"MOVRC RC.x, R0.y;"
"MOVR  R0.x, R1.z;"
"MOVR  R0.x(LT), -R1.z;"
"MOVXC RC.x, H0.w;"
"MOVR  R0.x(EQ), {1000};"
"MOVXC RC.x, H1.z;"
"MOVR  R0.x(NE), R0.z;"
"FLRR  R0.y, R1;"
"FLRR  R0.z, R1.x;"
"ADDR  R1.x, R0.z, R0.y;"
"SLTR  H0.w, R0.z, -R0.y;"
"MULR  R1.x, R1, {0.5};"
"FRCR  R1.x, |R1|;"
"MULR  R1.x, R1, {2};"
"MOVR  R0.y, -R1.x;"
"SEQX  H0.w, H0, {0}.x;"
"SEQR  H1.z, mask.x, R0.w;"
"MULX  H1.x, H1, H1.y;"
"MULX  H1.x, H1, H1.z;"
"MULXC HC.x, H1, H0.w;"
"MOVR  R0.y(NE.x), R1.x;"
"MOVXC RC.x, H1;"
"SLTR  R0.x(NE), R0.y, {1};"
"MOVH  H0.w, H1;"
"SEQRC HC.x, R0, mask_value;"
"MOVH  H0(NE.x), {0}.x;"
"MOVH  o[COLH].xyz, H0;"
"MOVXC RC.x, premult;"
"MULH  o[COLH].xyz(NE.x), H0, H0.w;"
"MOVH  o[COLH].w, H0;"
"END"
"# 184 instructions, 2 R-regs, 8 H-regs";

} // namespace


void GLEngine::handle_cg_errors()
{
//   std::cerr << cgGetErrorString (cgGetError()) << std::endl;
//   std::cerr << cgGetLastListing (cgContext) << std::endl;
  exit(1);
}


void
GLEngine::loadBuiltinFragShader()
{

    _rgba = new GLShader();

    try {
        if ( _hardwareShaders == kNV30 )
        {
            LOG_INFO( _("Loading built-in NV3.0 rgba shader") );
            _rgba->load( N_("builtin"), NVShader );
        }
        else
        {
            LOG_INFO( _("Loading built-in arbfp1 rgba shader") );
            _hardwareShaders = kARBFP1;
            _rgba->load( N_("builtin"), ARBFP1Shader );
        }
    }
    catch( const std::exception& e )
    {
      LOG_ERROR( e.what() );
    }

}


void GLEngine::clear_quads()
{
    TRACE("");
    QuadList::iterator i = _quads.begin();
    QuadList::iterator e = _quads.end();
    for ( ; i != e; ++i )
    {
        delete *i;
    }
    _quads.clear();

}


void GLEngine::release()
{
    TRACE("");
    clear_quads();

    TRACE("");
    GLLut3d::clear();

    TRACE("");
    if ( sCharset ) {
        glDeleteLists( sCharset, 255 );
        CHECK_GL;
    }

    TRACE("");
    if (_rgba)  delete _rgba;
    TRACE("");
    if (_YByRy) delete _YByRy;
    TRACE("");
    if (_YCbCr) delete _YCbCr;
}


void GLEngine::resize_background()
{
}

GLEngine::GLEngine(const mrv::ImageView* v) :
DrawEngine( v ),
texWidth( 0 ),
texHeight( 0 ),
vr( ImageView::kNoVR ),
vr_angle( 45.0 ),
_rotX( 0.0 ),
_rotY( 0.0 )
{
  initialize();
}

GLEngine::~GLEngine()
{
  release();
}





} // namespace mrv
