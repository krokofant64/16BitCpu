#ifndef MEMORY_H
#define MEMORY_H

#include "MemoryApi.h"

#include <stddef.h>
#include <stdint.h>

class Memory : public MemoryApi
{
public:

   explicit
   Memory(
      uint16_t theSize);

   ~Memory();

   const uint16_t*
   readData(
      uint16_t  theAddress,
      uint16_t& theDataLength) const;

   bool
   writeBinary(
      uint16_t    theAddress,
      const char* theBinaryValue);

   bool
   writeHex(
      uint16_t theAddress,
      uint16_t theHexValue);

   uint16_t
   writeData(
      uint16_t        theAddress,
      const uint16_t* theData,
      uint16_t        theDataLength);

   void
   setAddress(
      uint16_t theAddress) override;

   uint16_t
   read() override;

   void
   write(
      uint16_t theData) override;   

private:

   uint16_t  addressM;
   uint16_t* memoryMd;
   uint16_t  sizeM;
};

#endif
