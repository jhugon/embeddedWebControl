#include "VarRecord.h"

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
  return "";
} //varTypeToStr

ewc::VarRecord::VarRecord(char* nm, void* addr, VarType t, char* desc, bool atsl)
{
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
} // VarRecord constructor

void ewc::VarRecord::toValString(char* outStr, size_t strLen)
{
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
    case DecUInteger8:
      snprintf(outStr,strLen,"%hhu",*((uint8_t*) address));
      break;
    case DecUInteger16:
      snprintf(outStr,strLen,"%hu",*((uint16_t*) address));
      break;
    case DecUInteger32:
      snprintf(outStr,strLen,"%lu",*((uint32_t*) address));
      break;
    case OctUInteger8:
      snprintf(outStr,strLen,"0o%hho",*((uint8_t*) address));
      break;
    case OctUInteger16:
      snprintf(outStr,strLen,"0o%ho",*((uint16_t*) address));
      break;
    case OctUInteger32:
      snprintf(outStr,strLen,"0o%lo",*((uint32_t*) address));
      break;
    case HexUInteger8:
      snprintf(outStr,strLen,"0x%hhX",*((uint8_t*) address));
      break;
    case HexUInteger16:
      snprintf(outStr,strLen,"0x%hX",*((uint16_t*) address));
      break;
    case HexUInteger32:
      snprintf(outStr,strLen,"0x%lX",*((uint32_t*) address));
      break;
    default:
      break;
  }; // switch varType
} // toValString

int ewc::VarRecord::fromValString(const char* inStr)
{
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
    case DecUInteger8:
      return sscanf(inStr,"%hhu",(uint8_t*) address);
      break;
    case DecUInteger16:
      return sscanf(inStr,"%hu",(uint16_t*) address);
      break;
    case DecUInteger32:
      return sscanf(inStr,"%lu",(uint32_t*) address);
      break;
    case OctUInteger8:
      return sscanf(inStr,"0o%hho",(uint8_t*) address);
      break;
    case OctUInteger16:
      return sscanf(inStr,"0o%ho",(uint16_t*) address);
      break;
    case OctUInteger32:
      return sscanf(inStr,"0o%lo",(uint32_t*) address);
      break;
    case HexUInteger8:
      return sscanf(inStr,"0x%hhX",(uint8_t*) address);
      break;
    case HexUInteger16:
      return sscanf(inStr,"0x%hX",(uint16_t*) address);
      break;
    case HexUInteger32:
      return sscanf(inStr,"0x%lX",(uint32_t*) address);
      break;
    default:
      break;
  }; // switch varType
  return -1;
} // fromValString
