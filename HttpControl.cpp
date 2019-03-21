#include "HttpControl.h"

ewc::HttpControl::HttpControl(NetworkInterface* network):
    _httpServer(new HttpServer(network))
{
} // HttpControl constructor

ewc::HttpControl::~HttpControl()
{
  delete _httpServer;
} // HttpControl destructor


void ewc::HttpControl::addVar(char* name, void* address, VarType varType, char* description, bool addToStatsList, FuncRecordFuncType f)
{
    _varRecords.push_back(VarRecord(name,address,varType,description,addToStatsList,f));
} // addVar

nsapi_error_t ewc::HttpControl::start(uint16_t port)
{
  return _httpServer->start(port,callback(this,&ewc::HttpControl::handle));
} // start

void ewc::HttpControl::handle(ParsedHttpRequest* request, TCPSocket* socket)
{
    const std::string& url = request->get_url();
    debug("ewc::HttpControl::handle called with url: %s\n",url.c_str());
    if (url == "/")
    {
      indexHtml(request,socket);
    } // if url == /
    else if (url.compare(0,5,"/var/") == 0)
    {
      varInterface(request,socket);
    } // if url starts with /var/
    else {
        debug("ewc::HttpControl::handle URL not found, returning empty 404\n");
        HttpResponseBuilder builder(404); // not found
        builder.send(socket, NULL, 0);
    }
} // handle

void ewc::HttpControl::indexHtml(ParsedHttpRequest* request, TCPSocket* socket)
{
    debug("ewc::HttpControl::indexHtml called\n");
    const http_method & method = request->get_method();
    if (method == HTTP_GET)
    {
      HttpResponseBuilder builder(200);
      builder.set_header("Content-Type", "text/html; charset=utf-8");

      char response[] = "<html><head><title>Hello from mbed</title></head>"
          "<body>"
              "<h1>mbed webserver</h1>"
              "<button id=\"toggle\">Toggle LED</button>"
              "<script>document.querySelector('#toggle').onclick = function() {"
                  "var x = new XMLHttpRequest(); x.open('POST', '/toggle'); x.send();"
              "}</script>"
          "</body></html>";

      builder.send(socket, response, sizeof(response) - 1);
    } // if method == GET
    else
    {
      debug("ewc::HttpControl::indexHtml method other than GET called, return empty 405\n");
      HttpResponseBuilder builder(405); // method not allowed
      builder.send(socket, NULL, 0);
    } // else method isn't GET
} // indexHtml

void ewc::HttpControl::varInterface(ParsedHttpRequest* request, TCPSocket* socket)
{
    const std::string& url = request->get_url();
    debug("ewc::HttpControl::varInterface called with url: %s\n",url.c_str());
    const std::string& varName = url.substr(5,VARNAMESIZE);
    debug("ewc::HttpControl::varInterface var name: \"%s\"\n",varName.c_str());
    for(size_t iRec=0; iRec < _varRecords.size(); iRec++)
    {
      std::string thisVarName = _varRecords[iRec].name;
      //debug("ewc::HttpControl::varInterface thisVarName: %s\n",thisVarName.c_str());
      if (thisVarName == varName)
      {
        debug("ewc::HttpControl::varInterface found record: %s\n",thisVarName.c_str());
        debug("ewc::HttpControl::varInterface function address: %p\n",(void*) _varRecords[iRec].function);
        const http_method & method = request->get_method();
        if (_varRecords[iRec].function)
        {
          debug("ewc::HttpControl::varInterface record is function, calling");
          const std::string& message = request->get_body_as_string();
          char* outVal=NULL;
          size_t outValLen=0;
          int http_status=0;
          _varRecords[iRec].function(message.c_str(),message.size(),method,outVal,outValLen,http_status);
          HttpResponseBuilder builder(http_status);
          //builder.set_header("Content-Type", "text/plain");
          builder.set_header("Content-Type", "text/html");
          builder.send(socket, outVal, outValLen);
          return;
        }  // else function is good pointer
        if (method == HTTP_GET)
        {
          char outVal[16];
          outVal[0] = '\0';
          _varRecords[iRec].toValString(outVal,16);
          size_t outValLen = 0;
          for(size_t iChar=0; iChar < 16; iChar++)
          {
            if(outVal[iChar] == '\0')
            {
              outValLen = iChar;
              break;
            }
            else if (iChar == 15)
            {
              outValLen = iChar;
              outVal[iChar] = '\0';
            }
          }
          debug("ewc::HttpControl::varInterface HTTP_GET returning: \"%s\"\n",outVal);
          HttpResponseBuilder builder(200);
          builder.set_header("Content-Type", "text/plain");
          builder.send(socket, outVal, outValLen);
          return;
        } // if method == GET
        else if (method == HTTP_POST)
        {
          const std::string& message = request->get_body_as_string();
          int status = _varRecords[iRec].fromValString(message.c_str());
          if (status == 1)
          {
            debug("ewc::HttpControl::varInterface HTTP_POST success, data: \"%s\"\n",message.c_str());
            HttpResponseBuilder builder(200);
            builder.send(socket, NULL, 0);
            return;
          }
          else
          {
            debug("ewc::HttpControl::varInterface HTTP_POST error parsing data: \"%s\", status code %d\n",message.c_str(),status);
            HttpResponseBuilder builder(500);
            builder.send(socket, NULL, 0);
            return;
          }
        } // if method == POST
        else // method not GET/POST
        {
          debug("ewc::HttpControl::varInterface invalid HTTP_METHOD\n");
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
          return;
        } // else method not GET/POST
      } // if name == varName
    } // for iRec
    // Didn't find a record with the right name, so return 404 error
    debug("ewc::HttpControl::varInterface var not found, returning empty 404\n");
    HttpResponseBuilder builder(404); // not found
    builder.send(socket, NULL, 0);
} // varInterface
