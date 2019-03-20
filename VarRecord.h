/** \brief Variable Record for HttpControl
 */

#include "mbed.h"

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

  const char* varTypeToStr(const VarType v);

  struct VarRecord
  {
    char name[VARNAMESIZE];
    void* address;
    VarType varType;
    char description[DESCRIPTIONSIZE];
    bool addToStatusList;

    VarRecord(char* nm, void* addr, VarType t, char* desc, bool atsl);
    void toValString(char* outStr, size_t strLen);
    int fromValString(const char* inStr);
  }; 

} // namespace ewc
