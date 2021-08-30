#include "CpuControlPanel.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

static const size_t ScreenWidthC = 80;
static const size_t ScreenHeightC = 30;
static const size_t ScreenSizeC = ScreenWidthC * ScreenHeightC;

CpuControlPanel::CpuControlPanel(
   const Cpu&    theCpu,
   const Memory& theMemory)
:  cpuM(theCpu),
   memoryM(theMemory)
{
   screenMd = new char [ScreenWidthC * ScreenHeightC];
}

// ----------------------------------------------------------------------------

CpuControlPanel::~CpuControlPanel()
{
   delete screenMd;
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::draw() const
{
   // Clear screen
   memset(screenMd, ' ', ScreenWidthC * ScreenHeightC);

   // Print registers
   printf(1, 1, "Registers:");
   for (size_t i = 0; i < Cpu::NumRegsC; i++)
   {
      printf(1, 2+i, "R%zu: %04X", i, cpuM.getRegister(i));
      if (i == Cpu::PcRegC)
      {
         printf(9, 2+i, " PC");
      }
      else
      if ( i == Cpu::SpRegC)
      {
         printf(9, 2+i, " SP");
      }
   }
   
   printf(1, 11, "Address:     %04X", cpuM.getAddressRegister());
   printf(1, 12, "Instruction: %04X", cpuM.getInstruction());
   Cpu::Flags flags = cpuM.getFlags();
   printf(1, 13, "Flags:       CF=%d", 
          flags.carry);
   printf(1, 14, "             ZF=%d", 
          flags.zero);
   printf(1, 15, "             NF=%d", 
          flags.negative);
   printf(1, 16, "             OF=%d", 
          flags.overflow);
   printf(1, 17, "             IF=%d", 
          flags.enableInterupt);
   const char* state;
   switch (cpuM.getState())
   {
      case Cpu::State::FetchInstrE:
      {
         state = "FetchInstr";
         break;
      }
      case Cpu::State::DecodeInstrE: 
      {
         state = "DecodeInstr";
         break;
      }
      case Cpu::State::WaitReadMemE:
      {
         state = "WaitReadMem";
         break;
      }
      case Cpu::State::WaitWriteMemE:
      {
         state = "WaitWriteMem";
         break;
      }
   }
   printf(1, 19, "State:       %s", state);

   // Print memory
   printf(30, 1, "Memory:");
   for (int memcol = 0; memcol < 4; memcol++)
   {
      uint16_t memorySize = 16;
      const uint16_t* memory = memoryM.readData(16 * memcol, memorySize);
      if (memory != nullptr)
      {
         for (int i = 0; i < memorySize; i++)
         {
            printf(30 + 13 * memcol, 2 + i, "%04X: %04X", memcol * 16 + i, memory[i]);
         }
      }
   }

   for (size_t row = 0; row < ScreenHeightC; row++)
   {
      std::string line(&screenMd[row * ScreenWidthC], ScreenWidthC);
      ::printf("%s\n", line.c_str());
   }
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::printf(
   size_t      theColumn, 
   size_t      theRow, 
   const char* theFormat, 
   ...) const
{
  if (theRow >= ScreenHeightC || theColumn >= ScreenWidthC)
  {
      return;
  }
  char buffer[ScreenWidthC + 1];
  size_t offset = theRow * ScreenWidthC + theColumn;
  va_list args;
  va_start(args, theFormat);
  vsnprintf(buffer, ScreenWidthC + 1 - theColumn, theFormat, args);
  va_end(args);   
  memcpy(&screenMd[offset], buffer, strlen(buffer));
}


