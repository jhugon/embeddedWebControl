/** \brief Function Record for HttpControl
 */
#ifndef FUNC_RECORD_H
#define FUNC_RECORD_H

#include "mbed.h"
#include "VarRecord.h"
#include "http_server.h"

namespace ewc
{
  class FuncRecord
  {
    public:

      char name[VARNAMESIZE];
      char description[DESCRIPTIONSIZE];
      bool addToStatusList;

      FuncRecord(char* nm, char* desc, bool atsl);
      virtual void operator()(ParsedHttpRequest* request, TCPSocket* socket);
      bool isMyName(ParsedHttpRequest* request) const;
  }; 

} // namespace ewc
#endif
