#include "HttpControl.h"

const char* ewc::varTypeToStr(const VarType v)
{
  using namespace ewc;
  switch(v)
  {
    case Nothing:
      return "Nothing";
      break;
    case DecInteger8:
      return "int8";
      break;
    case DecInteger16:
      return "int16";
      break;
    case DecInteger32:
      return "int32";
      break;
    case DecUInteger8:
      return "uint8";
      break;
    case DecUInteger16:
      return "uint16";
      break;
    case DecUInteger32:
      return "uint32";
      break;
    case OctUInteger8:
      return "Oct uint8";
      break;
    case OctUInteger16:
      return "Oct uint16";
      break;
    case OctUInteger32:
      return "Oct uint32";
      break;
    case HexUInteger8:
      return "Hex uint8";
      break;
    case HexUInteger16:
      return "Hex uint16";
      break;
    case HexUInteger32:
      return "Hex uint32";
      break;
    case FixFloat32:
      return "Fix float32";
      break;
    case FixFloat64:
      return "Fix float64";
      break;
    case ExpFloat32:
      return "Exp float32";
      break;
    case ExpFloat64:
      return "Exp float64";
      break;
    case Boolean:
      return "bool";
      break;
    case Character:
      return "char";
      break;
    case StringNullTerm:
      return "string";
      break;
  } //switch v
} //varTypeToStr

void ewc::VarFuncRecord::formatVal(char * buffer, size_t bufferSize) const
{
  using namespace ewc;
  switch(v)
  {
    case Nothing:
      *buffer = "\0";
      break;
    case DecInteger8:
      if(isFunction) snprintf(buffer,bufferSize,"%i",(int8_t) (*address()));
      else snprintf(buffer,bufferSize,"%i",(int8_t) (*address));
      break;
    case DecInteger16:
      if(isFunction) snprintf(buffer,bufferSize,"%i",(int16_t) (*address));
      else snprintf();
      break;
    case DecInteger32:
      if(isFunction) snprintf(buffer,bufferSize,"%i",(int32_t) (*address));
      else snprintf();
      break;
    case DecUInteger8:
      if(isFunction) snprintf(buffer,bufferSize,"%u",(uint8_t) (*address));
      else snprintf();
      break;
    case DecUInteger16:
      if(isFunction) snprintf(buffer,bufferSize,"%u",(uint16_t) (*address));
      else snprintf();
      break;
    case DecUInteger32:
      if(isFunction) snprintf(buffer,bufferSize,"%u",(uint32_t) (*address));
      else snprintf();
      break;
    case OctUInteger8:
      if(isFunction) snprintf(buffer,bufferSize,"%o",(uint8_t) (*address));
      else snprintf();
      break;
    case OctUInteger16:
      return "Oct int16";
      break;
    case OctUInteger32:
      return "Oct int32";
      break;
    case HexUInteger8:
      return "Hex int8";
      break;
    case HexUInteger16:
      return "Hex int16";
      break;
    case HexUInteger32:
      return "Hex int32";
      break;
    case FixFloat32:
      return "Fix float32";
      break;
    case FixFloat64:
      return "Fix float64";
      break;
    case ExpFloat32:
      return "Exp float32";
      break;
    case ExpFloat64:
      return "Exp float64";
      break;
    case Boolean:
      return "bool";
      break;
    case Character:
      return "char";
      break;
    case StringNullTerm:
      return "string";
      break;
  } //switch v
} // VarFuncRecord::formatVal

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
    else if (url.compare(0,5,"/var/") == 0)
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

      std::string title = "Hello World";
      std::string page = "<!DOCTYPE html>\r\n<html><head><title>";
      page += title;
      page += "</title></head>\r\n";
      page += "<body><h1>";
      page += title;
      page += "</h1>\r\n";
      page += "<table id=\"varfunctable\"><tr><th>Name</th><th>MemoryAddress</th><th>Type</th><th>IsFunction</th><th>Value</th><th>Description</th></tr>\r\n";
      const size_t& nRecs = _varFuncRecords.size();
      for(size_t iRec=0; iRec < nRecs; iRec++)
      {
        const VarFuncRecord& rec = _varFuncRecords[iRec];
        if(rec.addToStatusList)
        {
          page += "<td>";
          page += rec.name;
          page += "</td><td>";
          page += (uint32_t) rec.address;
          page += "</td><td>";
          page += varTypeToStr(rec.varType);
          page += "</td><td>";
          page += rec.isFunction;
          page += "</td><td>";
          if(rec.isFunction)
          {
          } // if isFunction
          else // is var
          {
          } // else is var
          page += "</td><td>";
          page += rec.description;
          page += "</td>\r\n";
        } // if addToStatsList
      } // for iRec
      page += "\r\n</table>";
      page += "\r\n</body></html>";
      builder.send(socket, page.c_str(), page.size());
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
        debug("ewc::HttpControl::varInterface found record: %s\n",_varRecords[iRec].name);
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
          }
          else
          {
            debug("ewc::HttpControl::varInterface HTTP_POST error parsing data: \"%s\", status code %d\n",message.c_str(),status);
            HttpResponseBuilder builder(500);
            builder.send(socket, NULL, 0);
          }
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
        }
        else
        {
          debug("ewc::HttpControl::varInterface invalid HTTP_METHOD\n");
          HttpResponseBuilder builder(405); // method not allowed
          builder.send(socket, NULL, 0);
        }
        break;
      } // if name == varName
    } // for iRec
} // varInterface
