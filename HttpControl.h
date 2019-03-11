/** \brief Web server class for embedded web control
 */

#include "mbed.h"
#include "http_server.h"
#include "http_response_builder.h"
#include <vector>

#define VARNAMESIZE 40
#define DESCRIPTIONSIZE 100

namespace ewc
{
  enum VarType
  {
      Nothing,
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

  struct VarFuncRecord
  {
    char name[VARNAMESIZE];
    void* address;
    VarType varType;
    bool isFunction;
    char description[DESCRIPTIONSIZE];
    bool addToStatusList;

    VarFuncRecord(char* nm, void* addr, VarType t, bool isf, char* desc, bool atsl){
      size_t nameLen = strlen(nm);
      size_t descLen = strlen(desc);
      for(size_t i=0; i < nameLen && i < VARNAMESIZE; i++)
      {
        name[i] = nm[i];
      }
      for(size_t i=0; i < descLen && i < DESCRIPTIONSIZE; i++)
      {
        description[i] = desc[i];
      }
      address = addr;
      varType = t;
      isFunction = isf;
      addToStatusList = atsl;
    };
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

      /** \brief Make a variable or function available to the web server
       *
       *  Maybe an array of these should be passed to the constructor, 
       *  then it is statically constructed?
       *
       *  Is it useful that I bounds check and put strings in fixed length char arrays?
       */
      void addVarFunc(char* name, void* address, VarType varType, bool isFunction, char* description, bool addToStatsList);

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

      /** \brief Handle requests for variables/functions
       *
       *  This is the REST API for variables that is used if
       *  the URL starts with "/vf/"
       */
      void varFuncInterface(ParsedHttpRequest* request, TCPSocket* socket);

      /////////////////////////////////

      HttpServer* _httpServer;

      std::vector<VarFuncRecord> _varFuncRecords;
    
  };
}
