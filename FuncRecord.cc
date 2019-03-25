#include "FuncRecord.h"

ewc::FuncRecord::FuncRecord(char* nm, char* desc, bool atsl)
{
  size_t nameLen = strlen(nm);
  debug("ewc::FuncRecord nm len: %u\n",nameLen);
  size_t descLen = strlen(desc);
  debug("ewc::FuncRecord desc len: %u\n",descLen);
  for(size_t i=0; i < nameLen && i < VARNAMESIZE; i++)
  {
    name[i] = nm[i];
  }
  if(nameLen < VARNAMESIZE-1)
  {
    name[nameLen] = '\0';
  }
  name[VARNAMESIZE-1] = '\0';
  for(size_t i=0; i < descLen && i < DESCRIPTIONSIZE; i++)
  {
    description[i] = desc[i];
  }
  if(descLen < DESCRIPTIONSIZE-1)
  {
    description[descLen] = '\0';
  }
  description[DESCRIPTIONSIZE-1] = '\0';
  addToStatusList = atsl;
} // FuncRecord constructor

void ewc::FuncRecord::operator()(HttpResponse* request, TCPSocket* socket)
{
  debug("ewc::FuncRecord::operator(): you called the base class method\n");
  // implement in derived class
} // operator()

bool ewc::FuncRecord::isMyName(ParsedHttpRequest* request) const
{
  const std::string& url = request->get_url();
  const std::string& varName = url.substr(5,VARNAMESIZE);
  return varName == name;
} // isMyName
