#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>
#include <string>
#include <vector>

class Opcode
{
public:

   enum class Format
   {
      EmptyE,
      NoArgE,      // E.g. CLI
      Reg3E,       // E.g. PSH R0
      Reg3Imm8E,   // E.g. LDL R4 0xF2
      Reg2Offs7E,  // E.g. BCS R2 -56 or BCS R3 or BCS +56
      Reg3Offs10E, // E.g. JMP R5 +10 or JMP R4 or JMP +100
      Reg21E,      // E.g. CMP R2 R3
      Reg32E,      // E.g. SHR R3 R2
      Reg321E,     // E.g. ADD R3 R2 R1
      Reg32Offs7E  // E.g. LD R3 [R2 +60] or STO R3 [R2] or LD R3 [+60]
   };

   Opcode();

   Opcode(
      const std::string& theName);

   Opcode(
      const Opcode& theOther);

   bool
   addFormat(
      Format   theFormat,
      uint16_t theCode,
      uint16_t theMask);

   const std::string&
   getName() const;

   size_t
   getNumberOfFormats() const;

   Format
   getFormat(
      size_t theIndex) const;

   uint16_t
   getCode(
      size_t theIndex) const;

   uint16_t
   getMask(
      size_t theIndex) const;

private:

   struct FormatEntry
   {
      Format   format;
      uint16_t code;
      uint16_t mask;
   };

   typedef std::vector<FormatEntry> FormatArray;

   std::string nameM;
   FormatArray formatM;
};

// ----------------------------------------------------------------------------

inline
Opcode::Opcode()
{
   // Empty
}

// ----------------------------------------------------------------------------

inline
Opcode::Opcode(
   const std::string& theName)
:  nameM(theName)
{
   // Empty
}

// ----------------------------------------------------------------------------

inline
Opcode::Opcode(
   const Opcode& theOther)
:  nameM(theOther.nameM),
   formatM(theOther.formatM)
{
   // Empty
}

// ----------------------------------------------------------------------------

inline
bool
Opcode::addFormat(
   Format   theFormat,
   uint16_t theCode,
   uint16_t theMask)
{
   for (size_t i = 0; i < formatM.size(); i++)
   {
      if (formatM[i].format == theFormat)
      {
         // Format already exists
         return false;
      }
   } 
   FormatEntry entry;
   entry.format = theFormat;
   entry.code = theCode;
   entry.mask = theMask;
   formatM.push_back(entry);
   return true;
}

// ----------------------------------------------------------------------------

inline
const std::string&
Opcode::getName() const
{
   return nameM;
}

// ----------------------------------------------------------------------------

inline
size_t
Opcode::getNumberOfFormats() const
{
   return formatM.size();
}

// ----------------------------------------------------------------------------

inline
Opcode::Format
Opcode::getFormat(
   size_t theIndex) const
{
   if (theIndex < formatM.size())
   {
      return formatM[theIndex].format;
   }
   return Format::EmptyE;
}

// ----------------------------------------------------------------------------

inline
uint16_t
Opcode::getCode(
   size_t theIndex) const
{
   if (theIndex < formatM.size())
   {
      return formatM[theIndex].code;
   }
   return 0x0000;
}

// ----------------------------------------------------------------------------

inline
uint16_t
Opcode::getMask(
   size_t theIndex) const
{
   if (theIndex < formatM.size())
   {
      return formatM[theIndex].mask;
   }
   return 0x0000;
}

#endif