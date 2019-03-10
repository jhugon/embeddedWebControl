#include "HttpControl.h"

ewc::HttpControl::HttpControl(NetworkInterface* network):
    _httpServer(new HttpServer(network))
{
} // HttpControl constructor

ewc::HttpControl::~HttpControl()
{
  delete _httpServer;
} // HttpControl destructor


void ewc::HttpControl::addVar(char* varName, void* varAddress, VarType varType, char* description)
{
    _varRecords.push_back(VarRecord(varName,varAddress,varType,description));
} // addVar

void ewc::HttpControl::addFunc(char* varName, void* funcAddress, VarType returnType, char* description, bool addToStatusList)
{
  size_t nameLen = strlen(varName);
  size_t descLen = strlen(description);
  if(nameLen <= VARNAMESIZE && descLen <= DESCRIPTIONSIZE)
    _funcRecords.push_back(FuncRecord(varName,funcAddress,returnType,description,addToStatusList));
} // addFunc

nsapi_error_t ewc::HttpControl::start(uint16_t port)
{
  return _httpServer->start(port,callback(this,&ewc::HttpControl::handle));
} // start

void ewc::HttpControl::handle(ParsedHttpRequest* request, TCPSocket* socket)
{
    http_method method = request->get_method();
    std::string url = request->get_url();
    if (url == "/")
    {
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
    } // if url == /
    else {
        HttpResponseBuilder builder(404); // not found
        builder.send(socket, NULL, 0);
    }
} // handle
