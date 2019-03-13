#include "HttpControl.h"

ewc::HttpControl::HttpControl(NetworkInterface* network):
    _httpServer(new HttpServer(network))
{
} // HttpControl constructor

ewc::HttpControl::~HttpControl()
{
  delete _httpServer;
} // HttpControl destructor


void ewc::HttpControl::addVar(char* name, void* address, VarType varType, char* description, bool addToStatsList)
{
    _varRecords.push_back(VarRecord(name,address,varType,description,addToStatsList));
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
    else if (url.compare(0,5,"/var/"))
    {
      varInterface(request,socket);
    } // if url starts with /var/
    else {
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
      HttpResponseBuilder builder(405); // method not allowed
      builder.send(socket, NULL, 0);
    } // else method isn't GET
} // indexHtml

void ewc::HttpControl::varInterface(ParsedHttpRequest* request, TCPSocket* socket)
{
    const std::string& url = request->get_url();
    debug("ewc::HttpControl::varInterface called with url: %s\n",url.c_str());
    const std::string& varName = url.substr(5,VARNAMESIZE);
    for(size_t iRec=0; iRec < _varRecords.size(); iRec++)
    {
      if (_varRecords[iRec].name == varName)
      {
        const http_method & method = request->get_method();
        if (method == HTTP_GET)
        {
          char outVal[16];
          _varRecords[iRec].toValString(outVal,16);
          HttpResponseBuilder builder(200);
          builder.set_header("Content-Type", "text/plain");
          builder.send(socket, outVal, 16);
        }
        else if (method == HTTP_POST)
        {
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
        }
        else
        {
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
        }
      } // if name == varName
    } // for iRec
} // varInterface
