#include "Assembler.h"
#include "Cpu.h"
#include <ctype.h>

Assembler::Assembler()
:  currentAddressM(0),
   currentCharM(nullptr),
   lineM(0)
{
   opcodeTableM.addOpcode("ADD",  Opcode::Format::Reg321E,     0x0000, 0xE00F);
   opcodeTableM.addOpcode("ADC",  Opcode::Format::Reg321E,     0x0001, 0xE00F);
   opcodeTableM.addOpcode("SUB",  Opcode::Format::Reg321E,     0x0002, 0xE00F);
   opcodeTableM.addOpcode("SBC",  Opcode::Format::Reg321E,     0x0003, 0xE00F);
   opcodeTableM.addOpcode("AND",  Opcode::Format::Reg321E,     0x0004, 0xE00F);
   opcodeTableM.addOpcode("OR",   Opcode::Format::Reg321E,     0x0005, 0xE00F);
   opcodeTableM.addOpcode("XOR",  Opcode::Format::Reg321E,     0x0006, 0xE00F);
   opcodeTableM.addOpcode("NOT",  Opcode::Format::Reg32E,      0x0007, 0xE00F);
   opcodeTableM.addOpcode("LD",   Opcode::Format::Reg32E,      0x0008, 0xE00F);
   opcodeTableM.addOpcode("LDL",  Opcode::Format::Reg32E,      0x0009, 0xE00F);
   opcodeTableM.addOpcode("LDH",  Opcode::Format::Reg32E,      0x000A, 0xE00F);
   opcodeTableM.addOpcode("SWP",  Opcode::Format::Reg32E,      0x000B, 0xE00F);
   opcodeTableM.addOpcode("INC",  Opcode::Format::Reg32E,      0x000C, 0xE00F);
   opcodeTableM.addOpcode("CMP",  Opcode::Format::Reg21E,      0x000E, 0xE00F);
   opcodeTableM.addOpcode("DEC",  Opcode::Format::Reg32E,      0x000F, 0xE00F);
   opcodeTableM.addOpcode("SHR",  Opcode::Format::Reg32E,      0x2000, 0xE00F);
   opcodeTableM.addOpcode("SHL",  Opcode::Format::Reg32E,      0x2001, 0xE00F);
   opcodeTableM.addOpcode("ASHL", Opcode::Format::Reg32E,      0x2001, 0xE00F); // == SHL
   opcodeTableM.addOpcode("ASHR", Opcode::Format::Reg32E,      0x2002, 0xE00F);
   opcodeTableM.addOpcode("ROR",  Opcode::Format::Reg32E,      0x2003, 0xE00F);
   opcodeTableM.addOpcode("ROL",  Opcode::Format::Reg32E,      0x2004, 0xE00F);
   opcodeTableM.addOpcode("CLC",  Opcode::Format::NoArgE,      0x2008, 0xE00F);
   opcodeTableM.addOpcode("SEC",  Opcode::Format::NoArgE,      0x2009, 0xE00F);
   opcodeTableM.addOpcode("CLI",  Opcode::Format::NoArgE,      0x200A, 0xE00F);
   opcodeTableM.addOpcode("SEC",  Opcode::Format::NoArgE,      0x200B, 0xE00F);
   opcodeTableM.addOpcode("PSH",  Opcode::Format::Reg3E,       0x200C, 0xE00F);
   opcodeTableM.addOpcode("POP",  Opcode::Format::Reg3E,       0x200D, 0xE00F); // R3 != PC
   opcodeTableM.addOpcode("RET",  Opcode::Format::NoArgE,      0x3C0D, 0xFFFF); // R3 == PC
   opcodeTableM.addOpcode("RTI",  Opcode::Format::Reg3E,       0x200E, 0xE00F); 
   opcodeTableM.addOpcode("BCS",  Opcode::Format::Reg2Offs7E,  0x4000, 0xFC00);
   opcodeTableM.addOpcode("BCC",  Opcode::Format::Reg2Offs7E,  0x4400, 0xFC00);
   opcodeTableM.addOpcode("BZS",  Opcode::Format::Reg2Offs7E,  0x4800, 0xFC00);
   opcodeTableM.addOpcode("BZC",  Opcode::Format::Reg2Offs7E,  0x4C00, 0xFC00);
   opcodeTableM.addOpcode("BNS",  Opcode::Format::Reg2Offs7E,  0x5000, 0xFC00);
   opcodeTableM.addOpcode("BNC",  Opcode::Format::Reg2Offs7E,  0x5400, 0xFC00);
   opcodeTableM.addOpcode("BOS",  Opcode::Format::Reg2Offs7E,  0x5800, 0xFC00);
   opcodeTableM.addOpcode("BOC",  Opcode::Format::Reg2Offs7E,  0x5C00, 0xFC00);
   opcodeTableM.addOpcode("LDL",  Opcode::Format::Reg3Imm8E,   0x6000, 0xE300);
   opcodeTableM.addOpcode("LDH",  Opcode::Format::Reg3Imm8E,   0x6100, 0xE300);
   opcodeTableM.addOpcode("LDLZ", Opcode::Format::Reg3Imm8E,   0x6200, 0xE300);
   opcodeTableM.addOpcode("LDHZ", Opcode::Format::Reg3Imm8E,   0x6300, 0xE300);
   opcodeTableM.addOpcode("LDHZ", Opcode::Format::Reg3Imm8E,   0x6300, 0xE300);
   opcodeTableM.addOpcode("JMP",  Opcode::Format::Reg3Offs10E, 0x8000, 0xE000);
   opcodeTableM.addOpcode("JSR",  Opcode::Format::Reg3Offs10E, 0xA000, 0xE000);
   opcodeTableM.addOpcode("LD",   Opcode::Format::Reg32Offs7E, 0xC000, 0xE000);
   opcodeTableM.addOpcode("STO",  Opcode::Format::Reg32Offs7E, 0xE000, 0xE000);
   opcodeTableM.addOpcode("HLT",  Opcode::Format::NoArgE,      0x9FFF, 0xFFFF);
}

// ----------------------------------------------------------------------------

bool
Assembler::parseFile(
   const std::string& theFileName)
{
   FILE* fd = fopen(theFileName.c_str(), "rb");
   if (fd == nullptr)
   {
      errorMessageM = "Can't open input fil \"" + theFileName + "\".";
      return false;
   }
   
   std::string assemblyCode;
   char buffer[1024];
   while (true)
   {
      size_t numberOfBytesRead = fread(buffer, 1, sizeof(buffer), fd);
      if (numberOfBytesRead == sizeof(buffer))
      {
         assemblyCode.append(buffer, numberOfBytesRead);
      }
      else
      {
         if (numberOfBytesRead > 0)
         {
            assemblyCode.append(buffer, numberOfBytesRead);
         }
         break;
      }
   }
   
   fclose(fd);
   return parse(assemblyCode);
}


// ----------------------------------------------------------------------------

bool
Assembler::parse(
   const std::string& theAssemblyCode)
{
   currentAddressM = 0;
   currentCharM = theAssemblyCode.c_str();
   errorMessageM.clear();
   lineM = 1;
   
   while (currentChar() != 0)
   {
      if (parseLine() == false)
      {
         return false;
      }
   }
   return true;
}

// ----------------------------------------------------------------------------

const Opcode*
Assembler::identifierToOpcode(
   const std::string& theIdentifier) const
{
   return opcodeTableM.getOpcode(theIdentifier);
}

// ----------------------------------------------------------------------------

bool
Assembler::parseAddress(
   uint16_t  theOffsetMask,
   uint16_t& theRegister,
   uint16_t& theOffset)
{
   bool registerPresent = true;
   bool offsetPresent = true;
   if (currentChar() != '[')
   {
      // Parse label
      std::string identifier;
      if (parseIdentifier(identifier) == true)
      {
         if (findLabel(identifier, theOffsetMask, theOffset) == true)
         {
            theRegister = 7;
            return true;
         }
      }
      errorMessageM = "Address ('[' R_base(opt) +-offset(opt) ']' or Label) expected.";
      return false;
   }

   // Parse '[' R_base(opt) Offset(opt) ']'
   nextChar(); // Eat '['
   skipSpaces();

   if (parseRegister(theRegister) == false)
   {
      theRegister = Cpu::PcRegC;
      registerPresent = false;
   }
   skipSpaces();
   if (parseOffset(theOffsetMask, theOffset) == false)
   {
      theOffset = 0;
      offsetPresent = false;
   }
   skipSpaces();
   if (currentChar() != ']')
   {
      errorMessageM = "']' expected.";
      return false;
   }
   nextChar(); // Eat ']'
   if (registerPresent || offsetPresent)
   {
      return true;
   }
   else
   {
      errorMessageM = "Address ('[' R_base(opt) +-offset(opt) ']' or Label) expected.";
      return false;
   }
}

// ----------------------------------------------------------------------------

bool
Assembler::parseArguments(
   const Opcode* theOpcode,
   uint16_t&     theCode)
{
   for (size_t i = 0; i < theOpcode->getNumberOfFormats(); i++)
   {
      const char* parsePointer = getParsePointer();
      Opcode::Format format = theOpcode->getFormat(i);
      theCode = theOpcode->getCode(i);
      switch (format)
      {
         case Opcode::Format::NoArgE:      // E.g. CLI
         {
            if (isEndOfLine() == true)
            {
               theCode = theOpcode->getCode(i);
               return true;
            }
            errorMessageM = "\"" + theOpcode->getName() + "\" must not have arguments.";
            break;
         }
         case Opcode::Format::Reg3E:       // E.g. PSH R0
         {
            if (parseReg3(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg3Imm8E:   // E.g. LDL R4 0xF2
         {
            if (parseReg3Imm8(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg2Offs7E:  // E.g. BCS R2 -56 or BCS R3 or BCS +56
         {
            if (parseReg2Offs7(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg3Offs10E: // E.g. JMP R5 +10 or JMP R4 or JMP +100
         {
            if (parseReg3Offs10(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg21E:      // E.g. CMP R2 R3
         {
            if (parseReg21(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg32E:      // E.g. SHR R3 R2
         {
            if (parseReg32(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg321E:     // E.g. ADD R3 R2 R1
         {
            if (parseReg321(theCode) == true)
            {
               return true;
            }
            break;
         }
         case Opcode::Format::Reg32Offs7E: // E.g. LD R3 [R2 +60] or STO R3 [R2] or LD R3 [+60]
         {
            if (parseReg32Offs7(theCode) == true)
            {
               return true;
            }
            break;
         }
         default:
         {
            break;
         }
      }
      setParsePointer(parsePointer);
   }
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseBinNumber(
   uint32_t& theNumber)
{
   if (currentChar() == '0' || currentChar() == '1' || currentChar() == '_')
   {
      while (true)
      {
         switch (currentChar())
         {
            case '0':
            {
               theNumber <<= 1;
               break;
            }
            case '1':
            {
               theNumber <<= 1;
               theNumber |= 1;
               break;
            }
            case '_':
            {
               break;
            }
            default:
            {
               return true;
            }
            nextChar(); // Eat digit
         }
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseDecimalNumber(
   uint32_t& theNumber)
{
   while (isdigit(currentChar()))
   {
      theNumber = theNumber * 10 + currentChar() - '0';
      nextChar(); // Eat digit
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseDirective()
{
   std::string directive;
   if (parseIdentifier(directive) == false)
   {
      return false;
   }
   skipSpaces();
   if (directive == "data")
   {
      do
      {
         uint32_t number;
         if (parseExpression(number) == false)
         {
            return "Data, comment, or end - of - line expected.";
         }
         if (number > 0xFFFF)
         {
            errorMessageM = "Number is larger than machineword.";
            return false;
         }
         skipSpaces();
         codeM[currentAddressM++] = number & 0xFFFF;
      } while (currentChar() == ',');
      if (isEndOfLine() == false)
      {
         errorMessageM = "Data, comment, or end-of-line expected.";
         return false;
      }
   }
   else if (directive == "string")
   {
      std::string string;
      if (parseString(string) == false)
      {
         return false;
      }
      size_t numberOfWords = string.size() / 2;
      for (size_t i = 0; i < numberOfWords; i++)
      {
         uint16_t word = (string[i * 2] << 8) & 0xFF00;
         word |= (string[i * 2 + 1] & 0x00FF);
         codeM[currentAddressM++] = word;
      }
      if ((string.size() & 0x0001) != 0)
      {
         // Odd length;  
         uint16_t word = (string[string.size() -1] << 8) & 0xFF00;
         codeM[currentAddressM++] = word;
      }
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseExpression(
   uint32_t& theNumber)
{
   return parseNumber(theNumber);
}

// ----------------------------------------------------------------------------

bool
Assembler::parseHexNumber(
   uint32_t& theNumber)
{
   while (isxdigit(currentChar()))
   {
      theNumber = theNumber * 16;
      if (isdigit(currentChar()))
      {
         theNumber += currentChar() - '0';
      }
      else
      if (currentChar() >= 'a' && currentChar() <= 'f')
      {
         theNumber += 10 + currentChar() - 'a';
      }
      else
      if (currentChar() >= 'A' && currentChar() <= 'F')
      {
         theNumber += 10 + currentChar() - 'A';
      }
      nextChar(); // Eat hex digit
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseIdentifier(
   std::string& theIdentifier)
{
   if (isalpha(currentChar()))
   {
      theIdentifier = currentChar();
      nextChar(); // Eat first character   
      while (isalnum(currentChar()) || currentChar() == '_')
      {
         theIdentifier += currentChar();
         nextChar(); // Eat character
      }
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseLine()
{
   const char* startOfLine = currentCharM;
   skipSpaces();
   if (isEndOfLine())
   {
      // Comment or empty line
      return true;
   }

   if (currentChar() == '.')
   {
      nextChar(); // Eat '.'
      return parseDirective();
   }

   // Parse label or opcode
   std::string identifier;
   if (parseIdentifier(identifier) == false)
   {
      errorMessageM = "Opcode, label, comment or empty line expected.";
      return false;
   }

   const Opcode* opcode = opcodeTableM.getOpcode(identifier);

   if (opcode == nullptr)
   {
      // No opcode, check if this is a label
      skipSpaces();
      if (currentChar() != ':')
      {
         // Error - this is no label
         errorMessageM = "Opcode, label, comment or empty line expected.";
         return false;
      }

      // Previous identifier was a label
      if (storeLabel(identifier) == false)
      {
         return false;
      }

      nextChar(); // Eat ':'
      skipSpaces();

      if (isEndOfLine())
      {
         // Comment or empty line
         return true;
      }

      if (currentChar() == '.')
      {
         nextChar(); // Eat '.'
         return parseDirective();
      }

      // Parse opcode
      if (parseIdentifier(identifier) == false)
      {
         errorMessageM = "Opcode, comment or empty line expected.";
         return false;
      }

      opcode = opcodeTableM.getOpcode(identifier);
      if (opcode == nullptr)
      {
         errorMessageM = "Opcode, comment or empty line expected.";
         return false;
      }
   }

   skipSpaces();

   uint16_t code;
   if (parseArguments(opcode, code) == false)
   {
      return false;
   }
   const char* endOfLine = currentCharM - 1;
   sourceCodeM[currentAddressM] = std::string(startOfLine, endOfLine);
   printf("%04X: %04X \"%s\"\n", currentAddressM, code, sourceCodeM[currentAddressM].c_str());
   codeM[currentAddressM++] = code;
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseNumber(
   uint32_t& theNumber)
{
   if (isdigit(currentChar()))
   {
      theNumber = currentChar() - '0';
      nextChar();
      if (theNumber == 0)
      {
         if (currentChar() == 'x')
         {
            nextChar();
            return parseHexNumber(theNumber);
         }
         else
         if (currentChar() == 'b')
         {
            nextChar();
           return  parseBinNumber(theNumber);
         }
         else
         {
            return parseOctalNumber(theNumber);
         }
      }
      else
      {
         return parseDecimalNumber(theNumber);
      }
      
   }
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseOctalNumber(
   uint32_t& theNumber)
{
   while (currentChar() >= '0' && currentChar() <= '7')
   {
      theNumber = theNumber * 8 + currentChar() - '0';
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseOffset(
   uint16_t  theOffsetMask,
   uint16_t& theOffset)
{
   bool isNegative;
   if (currentChar() == '+')
   {
      isNegative = false;
   }
   else
   if (currentChar() == '-')
   {
      isNegative = true;
   }
   else
   {
      return false;
   }
   nextChar();
   skipSpaces();

   uint32_t number;
   if (parseExpression(number) == false)
   {
      return false;
   }
   if (isNegative)
   {
      // negative offset
      number = (~number + 1); // two's complement
      if ((number & ~theOffsetMask) != ~theOffsetMask)
      {
         return false;
      }
   }
   else
   {
      // positve offset
      if ((number & (~theOffsetMask)) != 0)
      {
         return false;
      }
   }
   theOffset = number & theOffsetMask;
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg21(
   uint16_t& theCode)
{
   uint16_t r2;
   if (parseRegister(r2) == true)
   {
      skipSpaces();
      uint16_t r1;
      if (parseRegister(r1) == true)
      {
         skipSpaces();
         if (isEndOfLine() == true)
         {
            theCode = encodeRegister2(theCode, r2);
            theCode = encodeRegister1(theCode, r1);
            return true;
         }
      }
   }
   errorMessageM = "Arguments R_lhs R_rhs expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg2Offs7(
   uint16_t& theCode)
{
   uint16_t r2;
   uint16_t offset;
   skipSpaces();
   if (parseAddress(0x007F, r2, offset) == true)
   {
      skipSpaces();
      if (isEndOfLine() == true)
      {
         theCode = encodeRegister2(theCode, r2);
         theCode = encodeOffs7(theCode, offset);
         return true;
      }
   }
   errorMessageM = "Arguments R_base +-offset expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg3(
   uint16_t& theCode)
{
   uint16_t r3;
   if (parseRegister(r3) == true)
   {
      skipSpaces();
      if (isEndOfLine() == true)
      {
         theCode = encodeRegister3(theCode, r3);
         return true;
      }
   }
   errorMessageM = " R_dest expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg32(
   uint16_t& theCode)
{
   uint16_t r3;
   if (parseRegister(r3) == true)
   {
      skipSpaces();
      uint16_t r2;
      if (parseRegister(r2) == true)
      {
         skipSpaces();
         if (isEndOfLine() == true)
         {
            theCode = encodeRegister3(theCode, r3);
            theCode = encodeRegister2(theCode, r2);
            return true;
         }
      }
   }
   errorMessageM = "Arguments R_dest R_src expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg321(
   uint16_t& theCode)
{
   uint16_t r3;
   if (parseRegister(r3) == true)
   {
      skipSpaces();
      uint16_t r2;
      if (parseRegister(r2) == true)
      {
         skipSpaces();
         uint16_t r1;
         if (parseRegister(r1) == true)
         {
            skipSpaces();
            if (isEndOfLine() == true)
            {
               theCode = encodeRegister3(theCode, r3);
               theCode = encodeRegister2(theCode, r2);
               theCode = encodeRegister1(theCode, r1);
               return true;
            }
         }
      }
   }
   errorMessageM = "Arguments R_dest R_src1 R_src2 expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg32Offs7(
   uint16_t& theCode)
{
   uint16_t r3;
   uint16_t r2;
   uint16_t offset;
   if (parseRegister(r3) == true)
   {
      skipSpaces();
      if (parseAddress(0x007F, r2, offset) == true)
      {
         skipSpaces();
         if (isEndOfLine() == true)
         {
            theCode = encodeRegister3(theCode, r3);
            theCode = encodeRegister2(theCode, r2);
            theCode = encodeOffs7(theCode, offset);
            return true;
         }
      }
   }
   errorMessageM = "Arguments R_dest [R_src +-offset] expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg3Imm8(
   uint16_t& theCode)
{
   uint16_t r3;
   if (parseRegister(r3) == true)
   {
      skipSpaces();
      uint32_t number;
      if (parseExpression(number) == true)
      {
         if (number < 256)
         {
            uint16_t imm8 = number;
            if (isEndOfLine() == true)
            {
               theCode = encodeRegister3(theCode, r3);
               theCode = encodeImm8(theCode, imm8);
               return true;
            }
         }
      }
   }
   errorMessageM = " R_dest imm8 expected.";
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseReg3Offs10(
   uint16_t& theCode)
{
   uint16_t r3;
   uint16_t offset;
   skipSpaces();
   if (parseAddress(0x03FF, r3, offset) == true)
   {
      skipSpaces();
      if (isEndOfLine() == true)
      {
         theCode = encodeRegister3(theCode, r3);
         theCode = encodeOffs10(theCode, offset);
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseRegister(
   uint16_t& theRegister)
{
   const char* parsePointer = getParsePointer();
   if (currentChar() == 'R')
   {
      nextChar(); // Eat 'R'
      if (currentChar() >= '0' && currentChar() <= '7')
      {
         theRegister = currentChar() - '0';
         nextChar(); // Eat digit
         return true;
      }
   }
   else
   if (currentChar() == 'S')
   {
      nextChar(); // Eat 'S'
      if (currentChar() == 'P')
      {
         theRegister = 6;
         nextChar(); // Eat 'P'
         return true;
      }
   }
   else
   if (currentChar() == 'P')
   {
      nextChar(); // Eat 'P'
      if (currentChar() == 'C')
      {
         theRegister = 7;
         nextChar(); // Eat 'C'
         return true;
      }
   }
   setParsePointer(parsePointer);
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::parseString(
   std::string& theString)
{
   theString.clear();
   skipSpaces();
   if (currentChar() != '"')
   {
      errorMessageM = "'\"' expected.";
   }
   nextChar(); // Eat '"'
   while (isprint(currentChar()))
   {
      switch (currentChar())
      {
         case '"':
         {
            nextChar(); // Eat '"'
            return true;
         } 
         case '\\':
         {
            // TODO: Handle escape sequences
            nextChar(); // Eat '\'
            break;
         }
         default:
         {
            theString += currentChar();
            nextChar();
            break;
         }
      }
   }
   errorMessageM = "Invalid character in string."; 
   return false;
}

// ----------------------------------------------------------------------------

bool
Assembler::findLabel(
   const std::string& theLabel,
   uint16_t           theMask,
   uint16_t&          theOffset)
{
   LabelMap::iterator iter = labelMapM.find(theLabel);
   if (iter == labelMapM.end())
   {
      // Label not found - new entry
      Label label;
      label.name = theLabel;
      label.isDefined = false;
      label.address = 0x0000;

      FixupPos fixupPos;
      fixupPos.mask = theMask;
      fixupPos.address = currentAddressM;

      label.fixupPos.push_back(fixupPos);
      labelMapM.insert(LabelMap::value_type(theLabel, label));
   }
   else
   {
      // Label found
      Label& label = (*iter).second;
      if (label.isDefined == false)
      {
         // Label found, but not yet defined - add entry to fixup table
         FixupPos fixupPos;
         fixupPos.mask = theMask;
         fixupPos.address = currentAddressM;

         label.fixupPos.push_back(fixupPos);
      }
      else
      {
         // Label found and defined
         if (offsetFromLabel(label, currentAddressM, theMask, theOffset) == false)
         {
            return false;
         }
      }
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::storeLabel(
   const std::string& theLabel)
{
   LabelMap::iterator iter = labelMapM.find(theLabel);
   if (iter == labelMapM.end())
   {
      // New label
      Label label;
      label.name = theLabel;
      label.isDefined = true;
      label.address = currentAddressM;
      labelMapM.insert(LabelMap::value_type(theLabel, label));
   }
   else
   {
      // Existing, but not yet defined label
      Label& label = (*iter).second;
      if (label.isDefined == true)
      {
         // Duplicat label
         errorMessageM = "Label \"" + theLabel + "\" is already defined.";
         return false;
      }

      // Define the label
      label.isDefined = true;
      label.address = currentAddressM;

      // 
      for (size_t i = 0; i < label.fixupPos.size(); i++)
      {
         uint16_t address = label.fixupPos[i].address;
         uint16_t mask = label.fixupPos[i].mask;
         uint16_t offset;
         if (offsetFromLabel(label, address, mask, offset) == false)
         {
            return false;
         }
         codeM[address] = (codeM[address] & ~mask) | (offset & mask);
      }
      label.fixupPos.clear();
   }
   return true;
}

// ----------------------------------------------------------------------------

bool
Assembler::offsetFromLabel(
   const Label& theLabel,
   uint16_t     theAddress,
   uint16_t     theMask,
   uint16_t&    theOffset)
{
   uint16_t offset = theLabel.address - (theAddress + 1);
   uint16_t notMask = ~theMask;
   uint16_t extra = offset & notMask;
   if (offset & 0x8000)
   {
      // Negative offset
      if (extra != notMask)
      {
         errorMessageM = "Label \"" + theLabel.name + 
                         "\" is too far from caller at address " + 
                         std::to_string(theLabel.address) + ".";
         return false;
      }
   }
   else
   {
      // Positive offset
      if (extra != 0)
      {
         errorMessageM = "Label \"" + theLabel.name + 
                         "\" is too far from caller at address " + 
                         std::to_string(theLabel.address) + ".";
         return false;
      }
   }
   theOffset = offset & theMask;
   return true;
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeImm8(
   uint16_t theCode,
   uint16_t theImm8) const
{
   return (theCode & ~0x00FF) | (theImm8 & 0x00FF);
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeOffs7(
   uint16_t theCode,
   uint16_t theOffs7) const
{
   return (theCode & ~0x007F) | (theOffs7 & 0x007F);
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeOffs10(
   uint16_t theCode,
   uint16_t theOffs10) const
{
   return (theCode & ~0x03FF) | (theOffs10 & 0x03FF);
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeRegister1(
   uint16_t theCode,
   uint16_t theRegister) const
{
   return (theCode & ~0x0070) | ((theRegister << 4) & 0x0070);
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeRegister2(
   uint16_t theCode,
   uint16_t theRegister) const
{
   return (theCode & ~0x0380) | ((theRegister << 7) & 0x0380);
}

// ----------------------------------------------------------------------------

uint16_t
Assembler::encodeRegister3(
   uint16_t theCode,
   uint16_t theRegister) const
{
   return (theCode & ~0x1C00) | ((theRegister << 10) & 0x1C00);
}

