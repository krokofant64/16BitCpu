#include "OpcodeTable.h"

OpcodeTable::OpcodeTable()
{
   // Empty
}

// ----------------------------------------------------------------------------

bool
OpcodeTable::addOpcode(
   const std::string& theName,
   Opcode::Format     theFormat,
   uint16_t           theCode,
   uint16_t           theMask)
{
   Opcode opcode(theName);

   std::pair<OpcodeMap::iterator, bool> result =
      opcodeMapM.insert(OpcodeMap::value_type(theName, opcode));
   return (*result.first).second.addFormat(theFormat, theCode, theMask);
}

// ----------------------------------------------------------------------------

const Opcode*
OpcodeTable::getOpcode(
   const std::string& theName) const
{
   OpcodeMap::const_iterator iter = opcodeMapM.find(theName);
   if (iter != opcodeMapM.end())
   {
      return &(*iter).second;
   }
   return nullptr;
}

