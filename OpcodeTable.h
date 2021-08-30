#ifndef OPCODETABLE_H
#define OPCODETABLE_H

#include <unordered_map>

#include "Opcode.h"

class OpcodeTable
{
public:

   OpcodeTable();

   bool
   addOpcode(
      const std::string& theName,
      Opcode::Format     theFormat,
      uint16_t           theCode,
      uint16_t           theMask);

   const Opcode*
   getOpcode(
      const std::string& theName) const;

private:

   typedef std::unordered_map<std::string, Opcode> OpcodeMap;

   OpcodeMap opcodeMapM;
};


#endif