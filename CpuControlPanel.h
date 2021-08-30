#ifndef CPUCONTROLPANEL_H
#define CPUCONTROLPANEL_H

#include "Cpu.h"
#include "Memory.h"

class CpuControlPanel
{
public:

   CpuControlPanel(
      const Cpu&    theCpu,
      const Memory& theMemory);

   ~CpuControlPanel();

   void
   draw() const;

private:

   void
   printf(
      size_t      theColumn, 
      size_t      theRow, 
      const char* theFormat, 
      ...) const;

   const Cpu&    cpuM;
   const Memory& memoryM;
   char*         screenMd;
};

#endif
