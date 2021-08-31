#include "Assembler.h"
#include "Cpu.h"
#include "CpuControlPanel.h"
#include "Memory.h"
#include <stdio.h>

static const char* TestC = "L1: LD R0 N1\n"
                           "    LD R1 N2; \n"
                           "    ADD R2 R1 R0\n"
                           "    JMP L1\n"
                           "N1: .data 0x1234\n"
                           "N2: .data 0x0001\n";

static const char* MultiplyC = "Start:   LD R0 Op1\n"
                               "         LD R2 Op2\n"
                               "; R0 * R2 -> R1 R2; temp R3\n"
                               "         LDLZ R1 0\n"
                               "         LDLZ R3 16\n"
                               "         SHR R2 R2\n"
                               "BitLoop: BCC NoAdd\n"
                               "         ADD R1 R1 R0\n"
                               "NoAdd:   SHR R1 R1\n"
                               "         ROR R2 R2\n"
                               "         DEC R3 R3\n"
                               "         BZC BitLoop\n"
                               "         JMP Start\n"
                               "Op1:     .data 500\n"
                               "Op2:     .data 13\n";

int
main(
   int argc, char* argv[])
{
   (void) argc;
   (void) argv;

   Assembler assembler;

   if (assembler.parse(MultiplyC) == false)
   {
      printf("Error in line %zu: %s\n", assembler.getErrorLine(),
                                        assembler.getErrorMessage().c_str());
      return 1;
   }

   Memory memory(1024);

   Cpu cpu(&memory);

   CpuControlPanel controlPanel(cpu, memory, assembler);

   memory.writeData(0x0000, assembler.getCode(), assembler.getCodeLength());
//   memory.writeBinary(0x0000, "110 000 111 0000111");
//   memory.writeBinary(0x0001, "110 001 111 0000111");
//   memory.writeBinary(0x0002, "000 010 000 001 0000");

//   memory.writeHex(0x0008, 0x1234);
//   memory.writeHex(0x0009, 0x0001);

         controlPanel.draw();

   while (true)
   {
      printf("clockTick (enter), or quit (q+enter)? ");
      int c = getchar();
      printf("\n");
      if (c == '\n')
      {
         cpu.clockTick();
         controlPanel.draw();
      }
      else
      if (c == 'q')
      {
         return 0;
      }
   }

   return 0;
}
