/** \brief Web server class for embedded web control
 */

#include "mbed.h"
#include "http_server.h"
#include "http_response_builder.h"

namespace ewc
{
  enum VarType
  {
      DecInteger8,
      DecInteger16,
      DecInteger32,
      OctInteger8,
      OctInteger16,
      OctInteger32,
      HexInteger8,
      HexInteger16,
      HexInteger32,
      FixFloat32,
      FixFloat64,
      ExpFloat32,
      ExpFloat64,
      Boolean,
      Character,
      StringNullTerm
  };

  struct VarRecord
  {
    char varName[40];
    void* varAddress;
    VarType varType;
    char description[100];
  };

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
       *  Is it useful that I bounds check and put these in fixed length char arrays?
       */
      void addVar(char* varName, void* varAddress, VarType varType, char* description);

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
    
      /////////////////////////////////

      HttpServer* _httpServer;
  };
}
