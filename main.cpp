#include "Assembler.h"
#include "Cpu.h"
#include "CpuControlPanel.h"
#include "Memory.h"
#include <stdio.h>

static const char* Test0C = "    LDL R0 0x11\n"
                            "    LDH R0 0x47\n"
                            "    STO R0 L4\n"
                            "    HLT\n"
                            "L1: .data 1\n"
                            "L2: .data 2\n"
                            "L3: .data 3\n"
                            "L4: .data 4\n";


static const char* Test1C = "    LDLZ SP 0x1F\n"
                            "    LDL R0 0x64\n"
                            "    LDH R0 0x19\n"
                            "    PSH R0\n"
                            "    POP R1\n"
                            "    HLT\n";

static const char* Test2C = "      LDLZ SP 0x1F\n"
                            "      LDL R0 0x64\n"
                            "      LDH R0 0x19\n"
                            "      JSR Swap\n"
                            "      HLT\n"
                            "Swap: SWP R0 R0\n"
                            "      RET\n"
                            "Str1: .string \"Hello\"";


static const char* TestC = "L1:   LD R0 N1\n"
                           "      LD R1 N2; \n"
                           "      ADD R2 R1 R0\n"
                           "      LDLZ SP 0x20\n"
                           "      PSH R2\n"
                           "      LDH R2 0x47\n"
                           "      LDL R2 0x11\n"
                         //  "      POP R2\n"
                           "      STO R2 Res\n"
                           "      JSR Sum\n"
                           "      HLT\n"
                           "Sum:  ADD R2 R2 R2\n"
                           "      RET\n"  
                           "N1:  .data 0x1234\n"
                           "N2:  .data 0x0001\n"
                           "Res: .data 0\n";

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
                               "         HLT\n"
                               "Op1:     .data 500\n"
                               "Op2:     .data 13\n";

static const char* DivideC =   "Start:   LD R1 Q\n"
                               "         LD R2 M\n"
                               "; R1 / R2 -> R1 remainder R0; temp R3\n"
                               "         LDLZ R0 0; A\n"
                               "         LDLZ R3 16; N\n"
                               "         SHL R1 R1\n"
                               "BitLoop: ROL R0 R0\n"
                               "         CMP R2 R0\n"
                               "         BCC NoSub\n"
                               "         SUB R0 R0 R2\n"
                               "         SEC\n"
                               "NoSub:   ROL R1 R1\n"
                               "         DEC R3 R3\n"
                               "         BZC BitLoop\n"
                               "         HLT\n"
                               "Q:       .data 100\n"
                               "M:       .data 13\n";

// ----------------------------------------------------------------------------

void
generateListing(
   const Assembler& theAssembler)
{
   for (uint16_t i = 0; i < theAssembler.getCodeLength(); i++)
   {
      uint16_t instruction = theAssembler.getCode()[i];
      const char* sourceCode = theAssembler.getSourceCode(i).c_str();
      printf("%04X %s\n", instruction, sourceCode);
   }
}

// ----------------------------------------------------------------------------

void
generateVerilogMemory(
   const Assembler& theAssembler)
{
   for (uint16_t i = 0; i < theAssembler.getCodeLength(); i++)
   {
      uint16_t instruction = theAssembler.getCode()[i];
      const char* sourceCode = theAssembler.getSourceCode(i).c_str();
      printf("mem[%d] =  16'h%04X; // %s\n", i, instruction, sourceCode);
   }
}

// ----------------------------------------------------------------------------

void
usage(
   const char* theProgram)
{
   fprintf(stderr, "Usage: %s <input file>\n", theProgram);
}

// ----------------------------------------------------------------------------

int
main(
   int argc, char* argv[])
{
   if (argc != 2)
   {
      usage(argv[0]);
      return 1;
   }

   Assembler assembler;

   if (assembler.parseFile(argv[1]) == false)
   {
      fprintf(stderr, "Error in line %zu: %s\n", assembler.getErrorLine(),
                                                 assembler.getErrorMessage().c_str());
      return 1;
   }

   generateVerilogMemory(assembler);

   Memory memory(1024 * 64);

   Cpu cpu(&memory);

   CpuControlPanel controlPanel(cpu, memory, assembler);

   memory.writeData(0x0000, assembler.getCode(), assembler.getCodeLength());

   controlPanel.draw();

   while (true)
   {
      printf("clockTick (enter), or quit (q+enter)?");
      printf("\n");
      int c = getchar();
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
