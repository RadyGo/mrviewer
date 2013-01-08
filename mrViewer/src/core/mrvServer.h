#ifndef mrvServer_h
#define mrvServer_h

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio.hpp>

namespace mrv {

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

class ViewerUI;

class Parser
{
   public:
     Parser( boost::asio::io_service& io_service, mrv::ViewerUI* v );
     ~Parser();
     
     bool parse( const std::string& m );
     void write( std::string s )
     {
	deliver( s );
     }
     virtual void deliver( std::string m ) = 0;

   public:
     tcp::socket socket_;
     mrv::ViewerUI* ui;
};


class tcp_session : public boost::enable_shared_from_this< tcp_session >,
		    public Parser
{
   public:
     tcp_session(boost::asio::io_service& io_service,
		 mrv::ViewerUI* const v);
     ~tcp_session();

     tcp::socket& socket();
     void start();

     bool stopped();

     void start_read();
     void handle_read(const boost::system::error_code& ec);
     void await_output();

     void deliver( const std::string m );

     void stop();

     void start_write();
     void handle_write(const boost::system::error_code& ec);
     void check_deadline(deadline_timer* deadline);

   protected:
     boost::asio::streambuf input_buffer_;
     deadline_timer input_deadline_;
     deadline_timer non_empty_output_queue_;
     deadline_timer output_deadline_;
     std::deque< std::string > output_queue_;

};

struct ServerData
{
     std::string host;
     std::string group;
     unsigned    port;
     mrv::ViewerUI* ui;
};

void server_thread( const ServerData* s );


}  // namespace mrv

#endif

