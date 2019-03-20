/** \brief Web server class for embedded web control
 */

#include "mbed.h"
#include "http_server.h"
#include "http_response_builder.h"
#include "VarRecord.h"
#include <vector>

namespace ewc
{
  /** \brief Web server class for embedded web control
   */
  class HttpControl
  {
    public:
      /** \brief Create webserver using the given network stack
       * 
       *  You have to use the start method to start the server
       *
       */
      HttpControl(NetworkInterface* network);

      ~HttpControl();

      /** \brief Make a variable available to the web server
       *
       *  Maybe an array of these should be passed to the constructor, 
       *  then it is statically constructed?
       *
       *  Is it useful that I bounds check and put strings in fixed length char arrays?
       */
      void addVar(char* name, void* address, VarType varType, char* description, bool addToStatsList);

      /** \brief Start the server
       *
       *  Start the server and connect it to its own port
       *  this will start a new thread
       */
      nsapi_error_t start(uint16_t port);

    private:

      /** \brief Handle request
       *
       *  This is where we parse the request URL, and use 
       *  that to decide if to return a json string (REST)
       *  or some kind of status web page
       */
      void handle(ParsedHttpRequest* request, TCPSocket* socket);

      /** \brief Handle request for main page
       *
       *  This returns a webpage if the
       *  the URL is "/"
       */
      void indexHtml(ParsedHttpRequest* request, TCPSocket* socket);

      /** \brief Handle requests for variables
       *
       *  This is the REST API for variables that is used if
       *  the URL starts with "/var/"
       */
      void varInterface(ParsedHttpRequest* request, TCPSocket* socket);

      /////////////////////////////////

      HttpServer* _httpServer;

      std::vector<VarRecord> _varRecords;
    
  };
}
