#include "Cpu.h"

#include <stdio.h>

Cpu::Cpu(
   MemoryApi* theMemoryApi)
:  memoryApiM(theMemoryApi)
{
   reset();
}

// ----------------------------------------------------------------------------

void
Cpu::reset()
{
   for (size_t i = 0; i < NumRegsC; i++)
   {
      regM[i] = 0;
   }
   addressRegM = 0;
   instructionM = 0;
   flagsM.carry = 0;
   flagsM.zero = 0;
   flagsM.negative = 0;
   flagsM.overflow = 0;
   stateM = State::FetchInstrE;
   tickCountM = 0;
}

// ----------------------------------------------------------------------------

void
Cpu::clockTick()
{
   tickCountM++;
   switch (stateM)
   {
      case State::FetchInstrE:
      {
         addressRegM = regM[PcRegC];
         memoryApiM->setAddress(addressRegM);
         regM[PcRegC]++;
         stateM = State::DecodeInstrE;
         break;
      }
      case State::DecodeInstrE:
      {
         instructionM = memoryApiM->read();
         decodeInstruction();
         break;
      }
      case State::WaitReadMemE:
      {
         readMemReady();
         break;
      }
      case State::WaitWriteMemE:
      {
         writeMemReady();
         break;
      }
      case State::WaitWriteReturnAddressE:
      {
         writeReturnAddressReady();
         break;
      }
   }
}

// ----------------------------------------------------------------------------

void
Cpu::decodeInstruction()
{
   switch ((instructionM >> 13) & 0x0007)
   {
      case 0x0000:
      {
         switch ((instructionM >> 2) & 0x0003)
         {
            case 0x0000:
            case 0x0001:
            {
               // ALU instructions (ADD, ADC, SUB, SBC, AND, OR, XOR, NOT)
               decodeAluInstruction();
               break;
            }
            case 0x0002:
            {
               // Halfword instructions (LD, LDL, LDH, SWP)
               decodeHalfwordInstruction();
               break;
            } 
            case 0x0003:
            {
               // Other instructions (INC, DEC, CMP)
               decodeOtherInstruction();
               break;
            }  
         }
         break;
      }
      case 0x0001:
      {
         switch ((instructionM >> 2) & 0x0003)
         {
            case 0x0000:
            case 0x0001:
            {
               // Shift instructions (SHR, ASHL, SHL, ASHR, ROR, ROL)
               decodeShiftInstruction();
               break;
            }
            case 0x0002:
            {
               // Flag instructions (CLC, SEC, CLI, SEI)
               decodeFlagInstruction();
               break;
            }
            case 0x0003:
            {
               // Stack instructions (PSH, POP, RET, RTI)
               decodeStackInstruction();
               break;
            }
         }
         break;
      }
      case 0x0002:
      {
         // Branch instructions (BCS, BCC, BZS, BZC, BNS, BNC, BOS, BOC))
         decodeBranchInstruction();
         break;
      }
      case 0x0003:
      {
         // Load immediate instructions (LDL, LDH, LDLZ, LDHZ)
         decodeImmediateInstruction();
         break;
      }
      case 0x0004:
      {
         // Jump instruction (JMP)
         decodeJmpInstruction();
         break;
      }
      case 0x0005:
      {
         // Jump subroutine instruction (JSR)
         decodeJsrInstruction();
         break;
      }
      case 0x0006:
      {
         // Load from memory instruction (LD mem)
         decodeLoadInstruction();
         break;
      }
      case 0x0007:
      {
         // Store to memory instruction (STO mem)
         decodeStoreInstruction();
         break;
      }
   }
}

// ----------------------------------------------------------------------------

void
Cpu::decodeAluInstruction()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   size_t rSrc1 = (instructionM >> 7) & 0x007; 
   size_t rSrc2 = (instructionM >> 4) & 0x007; 

   uint16_t src1 = regM[rSrc1];
   uint16_t src2 = regM[rSrc2];
   uint16_t result;

   switch (instructionM & 0x0007)
   {
      case 0x0000: // ADD
      {
         uint32_t tmpResult = src1 + src2;
         setCarryAndOverflowFlag(tmpResult, src1, src2);
         result = tmpResult & 0xFFFF;
         break;
      }
      case 0x0001: // ADC
      {
         uint32_t tmpResult = src1 + src2 + flagsM.carry;
         setCarryAndOverflowFlag(tmpResult, src1, src2);
         result = tmpResult & 0xFFFF;
         break;
      }
      case 0x0002: // SUB
      {
         uint32_t tmpResult = src1 - src2;
         setCarryAndOverflowFlag(tmpResult, src1, src2);
         result = tmpResult & 0xFFFF;
         break;
      }
      case 0x0003: // SBC
      {
         uint32_t tmpResult = src1 - src2 - flagsM.carry;
         setCarryAndOverflowFlag(tmpResult, src1, src2);
         result = tmpResult & 0xFFFF;
         break;
      }
      case 0x0004: // AND
      {
         result = (regM[rSrc1] & regM[rSrc2]);
         clearCarryAndOverflowFlag();
         break;
      }
      case 0x0005: // OR
      {
         result = (regM[rSrc1] | regM[rSrc2]);
         clearCarryAndOverflowFlag();
         break;
      }
      case 0x0006: // XOR
      {
         result = (regM[rSrc1] ^ regM[rSrc2]);
         clearCarryAndOverflowFlag();
         break;
      }
      case 0x0007: // NOT
      {
         result = ~regM[rSrc1];
         clearCarryAndOverflowFlag();
         break;
      }
   }
   setZeroAndNegativeFlag(result);
   regM[rDest] = result;
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeBranchInstruction()
{
   bool branch = false;
   switch ((instructionM >> 10) & 0x0007)
   {
      case 0x0000: // BCS
      {
         branch = (flagsM.carry == 1);
         break;
      }
      case 0x0001: // BCC
      {
         branch = (flagsM.carry == 0);
         break;
      }
      case 0x0002: // BZS
      {
         branch = (flagsM.zero == 1);
         break;
      }
      case 0x0003: // BZC
      {
         branch = (flagsM.zero == 0);
         break;
      }
      case 0x0004: // BNS
      {
         branch = (flagsM.negative == 1);
         break;
      }
      case 0x0005: // BNC
      {
         branch = (flagsM.negative == 0);
         break;
      }
      case 0x0006: // BOS
      {
         branch = (flagsM.overflow == 1);
         break;
      }
      case 0x0007: // BOC
      {
         branch = (flagsM.overflow == 0);
         break;
      }
   }
   if (branch == true)
   {
      size_t rAddress = (instructionM >> 7) & 0x007; 
      uint16_t address = regM[rAddress];
      uint16_t offset = instructionM & 0x007F;
      if (offset & 0x0040)
      {
         // Negative offset - copy sign to most significant bits
         offset |= 0xFF80;
      }
      regM[PcRegC] = address + offset;
   }
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeFlagInstruction()
{
   switch (instructionM & 0x0003)
   {
      case 0x0000: // CLC
      {
         flagsM.carry = 0;
         break;
      }
      case 0x0001: // SEC
      {
         flagsM.carry = 1;
         break;
      }
      case 0x0002: // CLI
      {
         flagsM.enableInterupt = 0;
         break;
      }
      case 0x0003: // SEI
      {
         flagsM.enableInterupt = 1;
         break;
      }
   }
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeHalfwordInstruction()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   size_t rSrc = (instructionM >> 7) & 0x007; 
   uint16_t src = regM[rSrc];
   uint16_t result;

   switch (instructionM & 0x0003)
   {
      case 0x0000: // LD
      {
         result = src;
         break;
      }
      case 0x0001: // LDL
      {
         result = src & 0x00FF;
         break;
      }
      case 0x0002: // LDH
      {
         result = (src >> 8) & 0x00FF;
         break;
      }
      case 0x0003: // SWP
      {
         result = ((src >> 8) & 0x00FF) | ((src << 8) & 0xFF00);
         break;
      }
   }
   setZeroAndNegativeFlag(result);
   regM[rDest] = result;
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeImmediateInstruction()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   uint16_t dest = regM[rDest];
   uint16_t imm = (instructionM & 0x00FF);
   uint16_t result;

   clearCarryAndOverflowFlag();
   switch ((instructionM >> 8) & 0x0003)
   {
      case 0x0000: // LDL
      {
         result = (dest & 0xFF00) | imm;
         break;
      }
      case 0x0001: // LDH
      {
         result = ((imm << 8) & 0xFF00) | (dest & 0x00FF);
         break;
      }
      case 0x0002: // LDLZ
      {
         result = imm;
         break;
      }
      case 0x0003: // LDHZ
      {
         result = (imm << 8) &  0xFF00; 
         break;
      }
   }
   setZeroAndNegativeFlag(result);
   regM[rDest] = result;
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeJmpInstruction()
{
   size_t rAddress = (instructionM >> 10) & 0x007; 
   uint16_t address = regM[rAddress];
   uint16_t offset = instructionM & 0x03FF;
   if (offset & 0x0200)
   {
      // Negative offset - copy sign to most significant bits
      offset |= 0xFC00;
   }
   regM[PcRegC] = address + offset;
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeJsrInstruction()
{
   addressRegM = regM[SpRegC];
   memoryApiM->setAddress(addressRegM);
   regM[SpRegC] -= 1;

   stateM = State::WaitWriteReturnAddressE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeLoadInstruction()
{
   uint16_t rAddress = (instructionM >> 7) & 0x007; 
   uint16_t address = regM[rAddress];
   uint16_t offset = instructionM & 0x007F;
   if (offset & 0x0040)
   {
      // Negative offset - copy sign to most significant bits
      offset |= 0xFF80;
   }
   addressRegM = address + offset;
   memoryApiM->setAddress(addressRegM);
   stateM = State::WaitReadMemE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeOtherInstruction()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   size_t rSrc1 = (instructionM >> 7) & 0x007; 
   size_t rSrc2 = (instructionM >> 4) & 0x007; 

   uint16_t src1 = regM[rSrc1];
   uint16_t src2 = regM[rSrc2];
   uint16_t result;
   switch (instructionM & 0x0003)
   {
      case 0x0000: // INC
      {
         result = src1 + 1;
         setZeroAndNegativeFlag(result);
         regM[rDest] = result;
         break;
      }
      case 0x0001: // spare
      {
         break;
      }
      case 0x0002: // CMP
      {
         uint32_t tmpResult = src1 - src2;
         setCarryAndOverflowFlag(tmpResult, src1, src2);
         result = tmpResult & 0xFFFF;
         setZeroAndNegativeFlag(result);
         break;
      }
      case 0x0003: // DEC
      {
         result = src1 - 1;
         setZeroAndNegativeFlag(result);
         regM[rDest] = result;
         break;
      }
   }
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeShiftInstruction()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   size_t rSrc = (instructionM >> 7) & 0x007; 

   uint16_t src = regM[rSrc];
   uint16_t result;
   switch (instructionM & 0x0007)
   {
      case 0x0000: // SHR
      {
         result = (src >> 1) & 0x7FFF;
         flagsM.carry = (src & 0x0001 ? 1 : 0);
         break;
      }
      case 0x0001: // ASHL & SHL
      {
         result = (src << 1) & 0xFFFE;
         flagsM.carry = (src & 0x8000 ? 1 : 0);
         break;
      }
      case 0x0002: // ASHR
      {
         if (src && 0x8000)
         {
            // Negative - fill with 1
            result = (src >> 1) | 0x8000; 
         }
         else
         {
            // Positive - fill with 0
            result = (src >> 1) & 0x7FFF; 
         }
         flagsM.carry = (src & 0x0001 ? 1 : 0);
         break;
      }
      case 0x0003: // ROR
      {
         int nextCarry = (src & 0x0001 ? 1 : 0); 
         if (flagsM.carry == 0)
         {
            result = (src >> 1) & 0x7FFF;
         }
         else
         {
            result = (src >> 1) | 0x8000;
         }
         flagsM.carry = nextCarry;
         break;
      }
      case 0x0004: // ROL
      {
         int nextCarry = (src & 0x8000 ? 1 : 0); 
         if (flagsM.carry == 0)
         {
            result = (src << 1) & 0xFFFE;
         }
         else
         {
            result = (src << 1) | 0x0001;
         }
         flagsM.carry = nextCarry;
         break;
      }
      case 0x0005: // Spare
      case 0x0006: // Spare
      case 0x0007: // Spare
      {
         break;
      }
   }
   flagsM.overflow = 0;
   setZeroAndNegativeFlag(result);
   regM[rDest] = result;
   stateM = State::FetchInstrE;
}

// ----------------------------------------------------------------------------

void
Cpu::decodeStackInstruction()
{
   size_t reg = (instructionM >> 10) & 0x007; 

   switch (instructionM & 0x0003)
   {
      case 0x0000: // PSH
      {
         addressRegM = regM[SpRegC];
         memoryApiM->setAddress(addressRegM);
         regM[SpRegC] -= 1;
         stateM = State::WaitWriteMemE;
         break;
      }
      case 0x0001: // POP; RET
      {
         regM[SpRegC] += 1;
         addressRegM = regM[SpRegC];
         memoryApiM->setAddress(addressRegM);
         stateM = State::WaitReadMemE;
         break;
      }
      case 0x0002: // RTI
      {
         break;
      }
      case 0x0003: // Spare
      {
         break;
      }
   }
}

// ----------------------------------------------------------------------------

void
Cpu::decodeStoreInstruction()
{
   size_t rAddress = (instructionM >> 7) & 0x007; 
   uint16_t address = regM[rAddress];
   uint16_t offset = instructionM & 0x007F;
   if (offset & 0x0040)
   {
      // Negative offset - copy sign to most significant bits
      offset |= 0xFF80;
   }
   addressRegM = address + offset;
   memoryApiM->setAddress(addressRegM);
   stateM = State::WaitWriteMemE;
}

// ----------------------------------------------------------------------------

void
Cpu::readMemReady()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   regM[rDest] = memoryApiM->read();
   setZeroAndNegativeFlag(regM[rDest]);
   clearCarryAndOverflowFlag();
   
   stateM = State::FetchInstrE;   
}

// ----------------------------------------------------------------------------

void
Cpu::writeMemReady()
{
   size_t rDest = (instructionM >> 10) & 0x007; 
   memoryApiM->write(regM[rDest]);
   stateM = State::FetchInstrE;   
}

// ----------------------------------------------------------------------------

void
Cpu::writeReturnAddressReady()
{
   memoryApiM->write(regM[PcRegC]);
   size_t rAddress = (instructionM >> 10) & 0x007; 
   uint16_t address = regM[rAddress];
   uint16_t offset = instructionM & 0x03FF;
   if (offset & 0x0200)
   {
      // Negative offset - copy sign to most significant bits
      offset |= 0xFC00;
   }
   regM[PcRegC] = address + offset;
   stateM = State::FetchInstrE;   
}

// ----------------------------------------------------------------------------

void
Cpu::setZeroAndNegativeFlag(
   uint16_t theResult)
{
   flagsM.zero = (theResult == 0 ? 1 : 0);
   flagsM.negative = (theResult & 0x8000 ? 1 : 0);
}

// ----------------------------------------------------------------------------

void
Cpu::setCarryAndOverflowFlag(
   uint32_t theTempResult,
   uint16_t theSrc1,
   uint16_t theSrc2)
{
   uint16_t result = theTempResult & 0xFFFF;
   uint16_t resultNegative = result & 0x8000;
   uint16_t src1Negative = theSrc1 & 0x8000;
   uint16_t src2Negative = theSrc2 & 0x8000;
   flagsM.carry = (theTempResult & 0x10000 ? 1 : 0);
   flagsM.overflow = (  src1Negative &    src2Negative  & (~resultNegative)) ||
                     ((~src1Negative) & ~(src2Negative) &   resultNegative);
}

// ----------------------------------------------------------------------------

void
Cpu::clearCarryAndOverflowFlag()
{
   flagsM.carry = 0;
   flagsM.overflow = 0;
}



