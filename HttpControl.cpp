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

void ewc::HttpControl::addFunc(FuncRecord* funcRecord)
{
   _funcRecords.push_back(funcRecord);
} // addFunc

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
    else if (url.compare(0,5,"/fun/") == 0)
    {
      funcInterface(request,socket);
    } // if url starts with /fun/
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

      static const std::string responseStart =
"<!DOCTYPE html>"
"<html><head><title>Mbed Webserver</title>"
"<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\">"
"</head>"
"<body>"
"<h1>Mbed Webserver</h1>"
"<h2>Variable Table</h2>"
"<table id=\"vartable\" class=\"table\">"
"<tr>"
"<th>Name</th>"
"<th>Memory Address</th>"
"<th>Type</th>"
"<th>Value</th>"
"<th>Description</th>"
"</tr>"
"<tr>"
"<td>Name</td>"
"<td>Memory Address</td>"
"<td>Type</td>"
"<td>Value</td>"
"<td>Description</td>"
"</tr>"
"</table>"
"<h2>Function Table</h2>"
"<table id=\"funtable\" class=\"table\">"
"<tr>"
"<th>Name</th>"
"<th>Description</th>"
"<th>Input</th>"
"<th>Output</th>"
"</tr>"
;
  static const std::string responseEnd =
"</table>"
"</body>"
"<script>"
"document.querySelector('.funcsend').onclick = function() {\n"
"console.log(\"Clicked on button!!\")\n"
"console.log(event)\n"
"console.log(event.target)\n"
"var whichbutton = event.target\n"
"var inputElem = whichbutton.parentNode.querySelector(\"input\");\n"
"var message = inputElem.value;\n"
"var row = whichbutton.parentNode.parentNode;\n"
"var outputElem = row.querySelector('.funcoutput')\n"
"var funcname = row.querySelector('.funcname').innerHTML\n"
"console.log(row)\n"
"console.log(outputElem)\n"
"console.log(\"funcname:\")\n"
"console.log(funcname)\n"
"console.log(\"message:\")\n"
"console.log(message)\n"
"var req = new XMLHttpRequest()\n"
"req.open('POST', '/fun/'+funcname)\n"
"req.onreadystatechange = function() {\n"
"if(this.readyState == 4) {\n"
"if(this.status == 200) {\n"
"outputElem.className = \"\";\n"
"outputElem.innerHTML = req.responseText;\n"
"}\n"
"else {\n"
"outputElem.className = \"table-danger\";\n"
"outputElem.innerHTML = \"HTTP Error: \"+this.status;\n"
"}\n"
"}\n"
"}\n"
"req.timeout = 1000\n"
"req.ontimeout = function() {\n"
"outputElem.className = \"table-warning\";\n"
"outputElem.innerHTML = '<span class=\"error\">HTTP Timeout</span>'\n"
"}\n"
"req.send(message)\n"
"}\n"
"</script>"
"</html>"
;
      std::string response = responseStart;
      for(size_t iRec=0; iRec < _funcRecords.size(); iRec++)
      {
        if(! (_funcRecords[iRec]->addToStatusList)) continue;
        std::string row =
        "<tr>"
        "<td class=\"funcname\">NAME</td>"
        "<td>DESCRIPTION</td>"
        "<td><input></input><button class=\"funcsend btn btn-primary\">Send</button></td>"
        "<td class=\"funcoutput\"></td>"
        "</tr>"
        ;
        size_t nameLoc = row.find("NAME");
        if(nameLoc < row.size())
        {
          debug("Found NAME at %u\n",nameLoc);
          row.replace(nameLoc,nameLoc+4,_funcRecords[iRec]->name);
          debug("Replaced NAME\n");
        }
        size_t descLoc = row.find("DESCRIPTION");
        if( descLoc < row.size())
        {
          debug("Found DESCRIPTION at %u\n",descLoc);
          row.replace(descLoc,descLoc+11,_funcRecords[iRec]->description);
          debug("Replaced DESC\n");
        }
        response += row;
        debug("Appended row\n");
      }
      response += responseEnd;
      builder.send(socket, response.c_str(), response.size());
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
        const http_method & method = request->get_method();
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
        }
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
        }
        else
        {
          debug("ewc::HttpControl::varInterface invalid HTTP_METHOD\n");
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
          return;
        }
      } // if name == varName
    } // for iRec
    // Didn't find a record with the right name, so return 404 error
    debug("ewc::HttpControl::varInterface var not found, returning empty 404\n");
    HttpResponseBuilder builder(404); // not found
    builder.send(socket, NULL, 0);
} // varInterface

void ewc::HttpControl::funcInterface(ParsedHttpRequest* request, TCPSocket* socket)
{
  debug("ewc::HttpControl::funcInterface called\n");
  for(size_t iRec=0; iRec < _funcRecords.size(); iRec++)
  {
    if (_funcRecords[iRec]->isMyName(request))
    {
      (*(_funcRecords[iRec]))(request,socket);
      return;
    }
  }
  debug("ewc::HttpControl::funcInterface function not found, returning empty 404\n");
  HttpResponseBuilder builder(404); // not found
  builder.send(socket, NULL, 0);
} // funcInterface
