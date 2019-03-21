/** \brief Variable Record for HttpControl
 */
#ifndef VAR_RECORD_H
#define VAR_RECORD_H

#include "mbed.h"
#include "http_server.h"

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
      DecUInteger8,
      DecUInteger16,
      DecUInteger32,
      OctUInteger8,
      OctUInteger16,
      OctUInteger32,
      HexUInteger8,
      HexUInteger16,
      HexUInteger32,
      FixFloat32,
      FixFloat64,
      ExpFloat32,
      ExpFloat64,
      Boolean,
      Character,
      StringNullTerm
  };

  /**
   * The function is responsible for allocating outStr, the caller will free it
   *
   */
  typedef void (*FuncRecordFuncType)(const char* inStr, size_t inLen, const http_method & method, char* outStr, size_t& outLen, int& http_return_code);

  const char* varTypeToStr(const VarType v);

  struct VarRecord
  {
    char name[VARNAMESIZE];
    void* address;
    VarType varType;
    char description[DESCRIPTIONSIZE];
    bool addToStatusList;
    FuncRecordFuncType function;

    VarRecord(char* nm, void* addr, VarType t, char* desc, bool atsl, FuncRecordFuncType f=NULL);
    void toValString(char* outStr, size_t strLen);
    int fromValString(const char* inStr);
  }; 

} // namespace ewc
#endif
