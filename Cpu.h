#ifndef CPU_H
#define CPU_H

#include "MemoryApi.h"
#include <stdint.h>

class Cpu
{
public:

   static constexpr int NumRegsC = 8;
   static constexpr int PcRegC = NumRegsC - 1;
   static constexpr int SpRegC = NumRegsC - 2;

   typedef struct
   {
      uint16_t carry : 1;
      uint16_t zero : 1;
      uint16_t negative : 1;
      uint16_t overflow : 1;
      uint16_t enableInterupt : 1;
   } Flags;

   enum class State
   { 
      FetchInstrE, 
      DecodeInstrE, 
      WaitReadMemE,
      WaitWriteMemE
   };

   explicit 
   Cpu(
      MemoryApi* theMemoryApi);

   void
   reset();

   void
   clockTick();

   // Only for test
   void
   decodeInstruction();

   uint16_t
   getAddressRegister() const;

   const Flags&
   getFlags() const;

   uint16_t
   getInstruction() const;

   uint16_t
   getRegister(
      size_t theIndex) const;

   State
   getState() const;

   void
   setAddressRegister(
      uint16_t theValue);

   void
   setFlags(
      const Flags& theFlags);

   void 
   setInstruction(
      uint16_t theValue);

   void
   setRegister(
      size_t   theIndex,
      uint16_t theValue);

private:

   void
   decodeAluInstruction();

   void 
   decodeBranchInstruction();

   void
   decodeFlagInstruction();

   void
   decodeHalfwordInstruction();

   void
   decodeImmediateInstruction();

   void
   decodeJmpInstruction();

   void
   decodeLoadInstruction();

   void
   decodeOtherInstruction();

   void
   decodeShiftInstruction();

   void
   decodeStoreInstruction();

   void
   readMemReady();

   void
   writeMemReady();

   void
   setZeroAndNegativeFlag(
      uint16_t theResult);

   void
   setCarryAndOverflowFlag(
      uint32_t theTempResult,
      uint16_t theSrc1,
      uint16_t theSrc2);

   void
   clearCarryAndOverflowFlag();

   MemoryApi* memoryApiM;
   uint16_t   regM[NumRegsC];
   uint16_t   addressRegM;
   Flags      flagsM;   
   uint16_t   instructionM;
   State      stateM;
};

// ----------------------------------------------------------------------------

inline
uint16_t
Cpu::getAddressRegister() const
{
   return addressRegM;
}

// ----------------------------------------------------------------------------

inline
const Cpu::Flags&
Cpu::getFlags() const
{
   return flagsM;
}

// ----------------------------------------------------------------------------

inline
uint16_t
Cpu::getInstruction() const
{
   return instructionM;
}

// ----------------------------------------------------------------------------

inline
uint16_t
Cpu::getRegister(
   size_t theIndex) const
{
   return theIndex < NumRegsC ? regM[theIndex] : 0xBAD1;
}

// ----------------------------------------------------------------------------

inline
Cpu::State
Cpu::getState() const
{
   return stateM;
}

// ----------------------------------------------------------------------------

inline
void
Cpu::setAddressRegister(
   uint16_t theValue)
{
   addressRegM = theValue;
}

// ----------------------------------------------------------------------------

inline
void
Cpu::setFlags(
   const Cpu::Flags& theFlags)
{
   flagsM = theFlags;
}

// ----------------------------------------------------------------------------

inline
void 
Cpu::setInstruction(
   uint16_t theValue)
{
   instructionM = theValue;
}

// ----------------------------------------------------------------------------

inline
void
Cpu::setRegister(
   size_t   theIndex,
   uint16_t theValue)
{
   if (theIndex < NumRegsC)
   { 
      regM[theIndex] = theValue;
   }
}

#endif
