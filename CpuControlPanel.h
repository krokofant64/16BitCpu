#ifndef CPUCONTROLPANEL_H
#define CPUCONTROLPANEL_H

#include "Assembler.h"
#include "Cpu.h"
#include "Memory.h"

class CpuControlPanel
{
public:

   CpuControlPanel(
      const Cpu&       theCpu,
      const Memory&    theMemory,
      const Assembler& theAssembler);

   ~CpuControlPanel();

   void
   draw() const;

private:

   void
   drawChar(
      uint16_t theChar) const;

   void
   showCode() const;

   void
   showMemory() const;

   void
   showScreen() const;

   void
   showSourceCode() const;

   void
   printf(
      size_t      theColumn, 
      size_t      theRow, 
      const char* theFormat, 
      ...) const;

   const Assembler& assemblerM;
   const Cpu&       cpuM;
   const Memory&    memoryM;
};

#endif
