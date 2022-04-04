#include "CpuControlPanel.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
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
   // Empty
}

// ----------------------------------------------------------------------------

CpuControlPanel::~CpuControlPanel()
{
   // Empty
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::drawChar(uint16_t theChar) const
{
   char character = static_cast<char>(theChar & 0x00FF);
   uint8_t foreground = static_cast<uint8_t>((theChar >> 8) & 0x0007);
   uint8_t background = static_cast<uint8_t>((theChar >> 11) & 0x0007);

   switch (foreground)
   {
      case 0: // black
      {
         ::printf("\033[30m");
         break;
      }
      case 1: // blue
      {
         ::printf("\033[34m");
         break;
      }
      case 2: // green
      {
         ::printf("\033[32m");
         break;
      }

      case 3: // cyan
      {
         ::printf("\033[36m");
         break;
      }
      case 4: // red
      {
         ::printf("\033[31m");
         break;
      }
      case 5: // magenta
      {
         ::printf("\033[35m");
         break;
      }
      case 6: // yellow
      {
         ::printf("\033[33m");
         break;
      }
      case 7: // white
      {
         ::printf("\033[37m");
         break;
      }
   } 
   switch (background)
   {
      case 0: // black
      {
         ::printf("\033[40m");
         break;
      }
      case 1: // blue
      {
         ::printf("\033[44m");
         break;
      }
      case 2: // green
      {
         ::printf("\033[42m");
         break;
      }
      case 3: // cyan
      {
         ::printf("\033[46m");
         break;
      }
      case 4: // red
      {
         ::printf("\033[41m");
         break;
      }
      case 5: // magenta
      {
         ::printf("\033[45m");
         break;
      }
      case 6: // yellow
      {
         ::printf("\033[43m");
         break;
      }
      case 7: // white
      {
         ::printf("\033[47m");
         break;
      }
   }
   ::printf("%c\033[39m\033[49m", character);
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::draw() const
{
   // Clear screen
   ::printf("\033[2J");

   // Print registers
   printf(1, 1, "Registers:");
   for (size_t i = 0; i < Cpu::NumRegsC; i++)
   {
      printf(10, 2 + i, "R%zu: %04X", i, cpuM.getRegister(i));
      if (i == Cpu::PcRegC)
      {
         printf(19, 2 + i, "PC");
      }
      else
      if (i == Cpu::SpRegC)
      {
         printf(19, 2 + i, "SP");
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

   showCode();

   showMemory();

   showScreen();

   printf(1, 29, "");
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::printf(
   size_t      theColumn, 
   size_t      theRow, 
   const char* theFormat, 
   ...) const
{
  char buffer[1024];
  snprintf(buffer, sizeof(buffer), "\033[%zu;%zuH", theRow, theColumn);
  strcat(buffer, theFormat);

  va_list args;
  va_start(args, theFormat);
  vprintf(buffer, args);
  va_end(args);   
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::showCode() const
{
   printf(5, 19, "Address: %04X", cpuM.getAddressRegister());
   printf(1, 20, "Instruction: %04X", cpuM.getInstruction());
   printf(8, 22, "Tick: %zu", cpuM.getTickCount());

   switch (cpuM.getState())
   {
      case Cpu::State::FetchInstrE:
      {
         printf(7, 21, "State: %s", "FetchInstr");
         break;
      }
      case Cpu::State::DecodeInstrE:
      {
         printf(7, 21, "State: %s", "DecodeInstr");
         break;
      }
      case Cpu::State::WaitReadMemE:
      {
         printf(7, 21, "State: %s", "WaitReadMem");
         break;
      }
      case Cpu::State::WaitWriteMemE:
      {
         printf(7, 21, "State: %s", "WaitWriteMem");
         break;
      }
      case Cpu::State::WaitWriteReturnAddressE:
      {
         printf(7, 21, "State: %s", "WaitWriteReturnAddress");
         break;
      }
   }
   showSourceCode();
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::showMemory() const
{
   // Print memory
   printf(24, 1, "Memory:");
   for (int memcol = 0; memcol < 6; memcol++)
   {
      printf(30 + 5 * memcol, 2, "%04X", memcol * 16);
      uint32_t memorySize = 16;
      const uint16_t* memory = memoryM.readData(16 * memcol, memorySize);
      if (memory != nullptr)
      {
         for (int i = 0; i < memorySize; i++)
         {
            if (memcol == 0)
            {
               printf(24, 3 + i, "%04x:", i);
            }
            printf(30 + 5 * memcol, 3 + i, "%04X", memory[i]);
         }
      }
   }
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::showScreen() const
{
   printf(64, 1, "Screen:");
   for (int row = 0; row < 30; row++)
   {
      uint32_t memorySize = 40;
      const uint16_t* memory = memoryM.readData(0x8000 + 40 * row, memorySize);
      for (int col = 0; col < 40; col++)
      {
         printf(64 + col, 2 + row, "");
         drawChar(memory[col]);
      }
   }
}

// ----------------------------------------------------------------------------

void
CpuControlPanel::showSourceCode() const
{
   printf(1, 23, "Source Code:");
   int offset = (cpuM.getState() == Cpu::State::FetchInstrE) ? 0 : -1;
   int currentAddress = cpuM.getRegister(Cpu::PcRegC) - offset;
   int row = 23;
   for (int i = currentAddress - 2; i <= currentAddress + 2; i++, row++)
   { 
      if (i != currentAddress)
      {
         printf(13, row, "  %s", assemblerM.getSourceCode(i).c_str());
      }
      else
      {
         printf(13, row, "> %s", assemblerM.getSourceCode(i).c_str());
      }
   }

}