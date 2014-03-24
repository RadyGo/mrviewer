/**
 * @file   mrvImageBrowser.h
 * @author gga
 * @date   Wed Jul 11 18:51:10 2007
 * 
 * @brief  
 * 
 * 
 */

#ifndef mrvImageBrowser_h
#define mrvImageBrowser_h

#include <vector>
#include <string>

#include "fltk/Browser.h"

#include "core/Sequence.h"
#include "core/mrvServer.h"
#include "gui/mrvReelList.h"
#include "gui/mrvMedia.h"

namespace fltk
{
  class Choice;
  class Button;
}

namespace mrv
{
  class Database;
  class Element;
  class ViewerUI;
  class Timeline;
  class EDLGroup;
  class ImageView;

  void start_button_cb(fltk::Button* o, mrv::ViewerUI* v);
  void end_button_cb(fltk::Button* o, mrv::ViewerUI* v);

  class ImageBrowser : public fltk::Browser
  {
  public:
    ImageBrowser(int x, int y, int w, int h);
    ~ImageBrowser();

    void clone_all_current();
    void clone_current();
    void open();
    void open_single();
    void save();
    void save_sequence();
    void save_reel();
    void load_reel( const char* name );
    void remove_reel();

    size_t number_of_reels() const { return _reels.size(); }
    mrv::Reel new_reel( const char* name = "reel" );
    mrv::Reel current_reel() const;
    mrv::Reel reel( const char* name );
    mrv::Reel reel( unsigned int idx );
       mrv::Reel reel_at( unsigned int idx );

    mrv::media current_image();

    const mrv::media current_image() const;

    void reel_choice( fltk::Choice* c ) { _reel_choice = c; }

    void attach_profile();
    void remove_current();
    void remove_all();

    void last_image();
    void change_image( unsigned i );
    void previous_image();
    void next_image();

    void insert( unsigned idx, mrv::media img );

       // @todo: these should be factored to a database helper class
    void add_image( const mrv::media& img );
    void add_video( const mrv::media& img );
    void add_audio( const mrv::media& img );

    mrv::media add( mrv::media& img );
    mrv::media add( CMedia* img );
    mrv::media add( const char* filename, 
		    const boost::int64_t start = -999999,
		    const boost::int64_t end = -999999 );
    

    void load( LoadList& files, bool progressBar = true );
       void load( const stringArray& files, bool progressBar = true );


    mrv::media replace( const char* file, const char* root );
    void remove( mrv::media m );
    void remove( int idx );

    void refresh( mrv::media img );

    void seek( const int64_t f );

    void frame( const int64_t f );

       void clear_edl();
       void set_edl();

    void toggle_edl();

    void change_background();

    void attach_icc_profile();
    
    void attach_ctl_script();


    void handle_dnd();

       void value( int idx );
       int value() const;

    virtual void draw();
    virtual int handle( int event );

    void main( mrv::ViewerUI* m ) { uiMain = m; }
    mrv::ViewerUI* main() { return uiMain; }

     public:
       static mrv::Element* new_item(mrv::media img);

  protected:
    void db_envvars( char*& login, std::string& shot_id );

    void change_reel();
    void change_image();
    void adjust_timeline();
    mrv::media load_image( const char* name, 
			   const int64_t start, const int64_t end );


    int mouseDrag( int x, int y );
    int mousePush( int x, int y );
    int mouseRelease( int x, int y );

    mrv::Timeline* timeline();
       mrv::EDLGroup* edl_group() const;

       mrv::ImageView* view() const;

    unsigned       _reel;
    mrv::ReelList  _reels;
    fltk::Choice*  _reel_choice;

    mrv::Element* dragging;
    int lastX, lastY;

    mrv::ViewerUI* uiMain;

    mrv::Database* db;
  };

} // namespace mrv


#endif  // mrvImageBrowser_h

