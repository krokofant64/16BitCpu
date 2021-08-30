#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stddef.h>
#include <stdint.h>
#include <string>

#include "OpcodeTable.h"

class Assembler
{
public:

   Assembler();

   bool
   parse(
      const std::string& theAssemblyCode);

   const uint16_t* 
   getCode() const;

   uint16_t
   getCodeLength() const;

   size_t
   getErrorLine() const;

   const std::string&
   getErrorMessage() const;

private:

   static constexpr int InvalidC = -1;

   enum class FixupType
   {
      Offs7E,
      Offs10E
   };

   struct FixupPos
   {
      uint16_t address;
      uint16_t mask;
   };

   struct Label
   {
      std::string           name;
      bool                  isDefined;
      uint16_t              address;
      std::vector<FixupPos> fixupPos;
   };

   typedef std::unordered_map<std::string, Label> LabelMap;

   char
   currentChar() const;

   const Opcode*
   identifierToOpcode(
      const std::string& theIdentifier) const;

   bool
   isEndOfLine();

   void
   nextChar();

   bool
   parseAddress(
      uint16_t  theOffsetMask,
      uint16_t& theRegister,
      uint16_t& theOffset);
      
   bool
   parseArguments(
      const Opcode* theOpcode,
      uint16_t&     theCode);

   bool
   parseBinNumber(
      uint32_t& theNumber);

   bool
   parseDecimalNumber(
      uint32_t& theNumber);

   bool
   parseDirective();
      
   bool
   parseExpression(
      uint32_t& theNumber);

   bool
   parseHexNumber(
      uint32_t& theNumber);

   bool
   parseIdentifier(
      std::string& theIdentifier);

   bool
   parseLine();

   bool
   parseNumber(
      uint32_t& theNumber);

   bool
   parseOctalNumber(
      uint32_t& theNumber);

   bool
   parseOffset(
      uint16_t  theOffsetMask,
      uint16_t& theOffset);

   bool
   parseReg21(
      uint16_t& theCode);

   bool
   parseReg2Offs7(
      uint16_t& theCode);

   bool
   parseReg3(
      uint16_t& theCode);

   bool
   parseReg32(
      uint16_t& theCode);

   bool
   parseReg321(
      uint16_t& theCode);
      
   bool
   parseReg32Offs7(
      uint16_t& theCode);

   bool
   parseReg3Imm8(
      uint16_t& theCode);

   bool
   parseReg3Offs10(
      uint16_t& theCode);

   bool
   parseRegister(
      uint16_t& theRegister);

   bool
   findLabel(
      const std::string& theLabel,
      uint16_t           theMask,
      uint16_t&          theAddress);

   bool
   storeLabel(
      const std::string& theLabel);

   bool
   offsetFromLabel(
      const Label& theLabel,
      uint16_t     theAddress,
      uint16_t     theMask,
      uint16_t&    theOffset);

   uint16_t
   encodeImm8(
      uint16_t theCode,
      uint16_t theImm8) const;

   uint16_t
   encodeOffs7(
      uint16_t theCode,
      uint16_t theOffs7) const;

   uint16_t
   encodeOffs10(
      uint16_t theCode,
      uint16_t theOffs10) const;

   uint16_t
   encodeRegister1(
      uint16_t theCode,
      uint16_t theRegister) const;

   uint16_t
   encodeRegister2(
      uint16_t theCode,
      uint16_t theRegister) const;

   uint16_t
   encodeRegister3(
      uint16_t theCode,
      uint16_t theRegister) const;

   const char*
   getParsePointer() const;

   void
   setParsePointer(
      const char* theParsePointer);

   void
   skipSpaces();

   uint16_t          codeM[65536];
   uint16_t          currentAddressM;
   const char*       currentCharM;
   std::string       errorMessageM;
   LabelMap          labelMapM;
   size_t            lineM;
   OpcodeTable       opcodeTableM;
};

// ----------------------------------------------------------------------------

inline
const uint16_t*
Assembler::getCode() const
{
   return codeM;
}

// ----------------------------------------------------------------------------

inline
uint16_t
Assembler::getCodeLength() const
{
   return currentAddressM;
}

// ----------------------------------------------------------------------------

inline
size_t
Assembler::getErrorLine() const
{
   return lineM;
}

// ----------------------------------------------------------------------------

inline
const std::string&
Assembler::getErrorMessage() const
{
   return errorMessageM;
}

// ----------------------------------------------------------------------------

inline
char
Assembler::currentChar() const
{
   return *currentCharM;
}

// ----------------------------------------------------------------------------

inline
bool
Assembler::isEndOfLine()
{
   if (currentChar() == ';' || 
       currentChar() == 0   ||
       currentChar() == '\n')
   { 
      while (currentChar() != '\n')
      {
         if (currentChar() == 0)
         {
            return true;
         }
         nextChar();
         if (currentChar() == 0)
         {
            return true;
         }
      }
      nextChar();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

inline
void
Assembler::nextChar()
{
   if (*currentCharM != 0)
   {
      if (*currentCharM == '\n')
      {
         lineM++;
      }
      currentCharM++;
   }
}

// ----------------------------------------------------------------------------

inline
const char*
Assembler::getParsePointer() const
{
   return currentCharM;
}

// ----------------------------------------------------------------------------

inline
void
Assembler::setParsePointer(
   const char* theParsePointer)
{
   currentCharM = theParsePointer;
}

// ----------------------------------------------------------------------------

inline
void
Assembler::skipSpaces()
{
   while (currentChar() == ' ' || currentChar() == '\t')
   {
      nextChar();
   }
}

#endif
