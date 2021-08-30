#ifndef MEMORYAPI_H
#define MEMORYAPI_H

#include <stddef.h>
#include <stdint.h>

class MemoryApi
{
public:
   
   virtual
   void
   setAddress(
      uint16_t theAddress) = 0;

   virtual
   uint16_t
   read() = 0;

   virtual
   void
   write(
      uint16_t theData) = 0;
};

#endif
