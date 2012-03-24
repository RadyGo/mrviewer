/**
 * @file   exrImage.h
 * @author gga
 * @date   Fri Sep 21 01:13:09 2007
 * 
 * @brief  Exr Image Loader
 * 
 * 
 */

#ifndef exrImage_h
#define exrImage_h

#include <CMedia.h>

#include <ImfLineOrder.h>
#include <ImfCompression.h>
#include <ImfPixelType.h>
#include <ImfHeader.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>



namespace mrv {

  class exrImage : public CMedia 
  {
    exrImage();
    ~exrImage();

    static CMedia* create() { return new exrImage(); }

    static const char* kCompression[];

    static const char* kLineOrder[];

  public:
    static bool test(const boost::uint8_t* datas, unsigned size=0);
    static CMedia* get(const char* name, const boost::uint8_t* datas = 0) {
      return CMedia::get(create, name, datas);
    }

    virtual const char* const format() const { return "ILM OpenEXR"; }

    virtual void compression( const unsigned idx ) { 
      _compression = (Imf::Compression) idx;
    };

    /// Returns the image compression (if any)
    virtual const char* const compression() const { return kCompression[_compression]; }

    virtual stringArray valid_compressions() const;

    /// Returns the image line order (if any)
    virtual const char* const line_order() const { return kLineOrder[_lineOrder]; }

    virtual bool fetch( const boost::int64_t frame );

    static bool save( const char* file, const CMedia* img );

       int levelX() { return _levelX; }
       int levelY() { return _levelY; }

       void levelX( const int lx ) { _levelX = lx; }
       void levelY( const int ly ) { _levelY = ly; }

  protected:
       bool channels_order( 
			   const boost::int64_t frame,
			   Imf::ChannelList::ConstIterator s,
			   Imf::ChannelList::ConstIterator e,
			   Imf::ChannelList& channels,
			   const Imf::Header& hdr, 
			   Imf::FrameBuffer& fb
			    );
       void ycc2rgba( const Imf::Header& hdr, const boost::int64_t frame );
       bool fetch_mipmap( const boost::int64_t frame );
       bool find_channels( const Imf::Header& h, Imf::FrameBuffer& fb,
			   boost::int64_t frame );
       void read_header_attr( const Imf::Header& h, boost::int64_t frame );

    image_type::PixelType pixel_type_conversion( Imf::PixelType pixel_type );

       int _levelX, _levelY; //<- r/mipmap levels
       bool          _has_yca;
    Imf::LineOrder   _lineOrder;
    Imf::Compression _compression;
  };

}

#endif // exrImage_h

