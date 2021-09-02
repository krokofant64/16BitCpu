#include "CpuControlPanel.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

static const size_t ScreenWidthC = 80;
static const size_t ScreenHeightC = 30;
static const size_t ScreenSizeC = ScreenWidthC * ScreenHeightC;

CpuControlPanel::CpuControlPanel(
   const Cpu&       theCpu,
   const Memory&    theMemory,
   const Assembler& theAssembler)
:  assemblerM(theAssembler),
   cpuM(theCpu),
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
      printf(10, 2+i, "R%zu: %04X", i, cpuM.getRegister(i));
      if (i == Cpu::PcRegC)
      {
         printf(19, 2+i, " PC");
      }
      else
      if ( i == Cpu::SpRegC)
      {
         printf(19, 2+i, " SP");
      }
   }
   
   Cpu::Flags flags = cpuM.getFlags();
   printf(7, 11, "Flags: CF=%d", 
          flags.carry);
   printf(7, 12, "       ZF=%d", 
          flags.zero);
   printf(7, 13, "       NF=%d", 
          flags.negative);
   printf(7, 14, "       OF=%d", 
          flags.overflow);
   printf(7, 15, "       IF=%d", 
          flags.enableInterupt);
   printf(5, 17, "Address: %04X", cpuM.getAddressRegister());
   printf(1, 18, "Instruction: %04X", cpuM.getInstruction());
   std::string state;
   switch (cpuM.getState())
   {
      case Cpu::State::FetchInstrE:
      {
         state = "FetchInstr";
         break;
      }
      case Cpu::State::DecodeInstrE: 
      {
         state = "DecodeInstr            " + assemblerM.getSourceCode(cpuM.getRegister(Cpu::PcRegC) - 1);
         break;
      }
      case Cpu::State::WaitReadMemE:
      {
         state = "WaitReadMem            " + assemblerM.getSourceCode(cpuM.getRegister(Cpu::PcRegC) - 1);
         break;
      }
      case Cpu::State::WaitWriteMemE:
      {
         state = "WaitWriteMem           " + assemblerM.getSourceCode(cpuM.getRegister(Cpu::PcRegC) - 1);
         break;
      }
      case Cpu::State::WaitWriteReturnAddressE:
      {
         state = "WaitWriteReturnAddress " + assemblerM.getSourceCode(cpuM.getRegister(Cpu::PcRegC) - 1);
         break;
      }
   }
   printf(7, 19, "State: %s", state.c_str());
   printf(8, 20, "Tick: %zu", cpuM.getTickCount());

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


