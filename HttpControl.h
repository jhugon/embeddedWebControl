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

  struct VarRecord
  {
    char name[VARNAMESIZE];
    void* address;
    VarType varType;
    char description[DESCRIPTIONSIZE];
    bool addToStatusList;

    VarRecord(char* nm, void* addr, VarType t, char* desc, bool atsl){
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
      addToStatusList = atsl;
    };

    void toValString(char* outStr, size_t strLen){
      switch(varType)
      {
        case DecInteger8:
          snprintf(outStr,strLen,"%hhd",*((int8_t*) address));
          break;
        case DecInteger16:
          snprintf(outStr,strLen,"%hd",*((int16_t*) address));
          break;
        case DecInteger32:
          snprintf(outStr,strLen,"%ld",*((int32_t*) address));
          break;
        default:
          break;
      }; // switch varType
    }; // toValString
    int fromValString(char* inStr){
      bool isNullTerm=false;
      for(size_t i=0; i<16; i++)
      {
        if (inStr[i] == '\0')
        {
          isNullTerm = true;
          break;
        } // if char is null
      } // for i
      if (!isNullTerm) // bad input could cause big issues!
      {
        debug("ewc::VarRecord::fromValString Error: string too long or not null terminated");
        return -1;
      } // if !isNullTerm
      switch(varType)
      {
        case DecInteger8:
          return sscanf(inStr,"%hhd",(int8_t*) address);
          break;
        case DecInteger16:
          return sscanf(inStr,"%hd",(int16_t*) address);
          break;
        case DecInteger32:
          return sscanf(inStr,"%ld",(int32_t*) address);
          break;
        default:
          break;
      }; // switch varType
    }; // toValString
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
