#include "Assembler.h"
#include "Cpu.h"
#include "CpuControlPanel.h"
#include "Memory.h"
#include <stdio.h>


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

         controlPanel.draw();

   while (true)
   {
      printf("clockTick (c), or quit (q)? ");
      int c = getchar();
      printf("\n");
      if (c == 'c')
      {
         cpu.clockTick();
         controlPanel.draw();
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
