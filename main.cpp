#include "Assembler.h"
#include "Cpu.h"
#include "CpuControlPanel.h"
#include "Memory.h"
#include <stdio.h>

void
printCpuStatus(const Cpu& theCpu)
{
   printf("Registers:\n");
   for (size_t i = 0; i < Cpu::NumRegsC; i++)
   {
      printf("              R%zu: %04X", i, theCpu.getRegister(i));
      if (i == Cpu::PcRegC)
      {
         printf(" PC");
      }
      else
      if ( i == Cpu::SpRegC)
      {
         printf(" SP");
      }
      printf("\n");
   }
   printf("\n");
   printf("Address Register: %04X\n", theCpu.getAddressRegister());
   printf("Instruction:      %04X\n", theCpu.getInstruction());
   Cpu::Flags flags = theCpu.getFlags();
   printf("Flags:            CF=%d, ZF=%d, NF=%d, OF=%d, IF=%d\n", 
          flags.carry,
          flags.zero,
          flags.negative,
          flags.overflow,
          flags.enableInterupt);
   printf("State:            ");
   switch (theCpu.getState())
   {
      case Cpu::State::FetchInstrE:
      {
         printf("FetchInstr");
         break;
      }
      case Cpu::State::DecodeInstrE: 
      {
         printf("DecodeInstr");
         break;
      }
      case Cpu::State::WaitReadMemE:
      {
         printf("WaitReadMem");
         break;
      }
      case Cpu::State::WaitWriteMemE:
      {
         printf("WaitWriteMem");
         break;
      }
   }
   printf("\n");
}

// ----------------------------------------------------------------------------

void
printMemory(
   const Memory& theMemory,
   uint16_t      theStartAddress,
   uint16_t      theSize)
{
   const uint16_t* memory = theMemory.readData(theStartAddress, theSize);
   if (memory != nullptr)
   {
      for (int i = 0; i < theSize; i++)
      {
         printf("%04X: %04X\n", theStartAddress + i, memory[i]);
      }
   }
}

// ----------------------------------------------------------------------------

int
main(
   int argc, char* argv[])
{
   (void) argc;
   (void) argv;

   Assembler assembler;

   if (assembler.parse("L1: LD R0 [PC +7]\n"
                       "    LD R1 [PC +7]; \n"
                       "    ADD R2 R1 R0\n"
                       "    JMP [PC -4]") == false)
   {
      printf("Error in line %zu: %s\n", assembler.getErrorLine(),
                                        assembler.getErrorMessage().c_str());
      return 1;
   }

   Memory memory(1024);

   Cpu cpu(&memory);

   CpuControlPanel controlPanel(cpu, memory);

   memory.writeData(0x0000, assembler.getCode(), assembler.getCodeLength());
//   memory.writeBinary(0x0000, "110 000 111 0000111");
//   memory.writeBinary(0x0001, "110 001 111 0000111");
//   memory.writeBinary(0x0002, "000 010 000 001 0000");

   memory.writeHex(0x0008, 0x1234);
   memory.writeHex(0x0009, 0x0001);

   printMemory(memory, 0x0000, 9);

   while (true)
   {
      printf("clockTick (c), memory (m), or quit (q)? ");
      int c = getchar();
      printf("\n");
      if (c == 'c')
      {
         cpu.clockTick();
         controlPanel.draw();
      }
      else
      if (c == 'm')
      {
         printMemory(memory, 0x0000, 16);
      }
      else
      if (c == 'q')
      {
         return 0;
      }
      c = getchar();
   }

   return 0;
}
