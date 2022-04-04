#include "Memory.h"
#include <string.h>

Memory::Memory(
   uint32_t theSize)
:  addressM(0),
   sizeM(theSize)
{
   memoryMd = new uint16_t [theSize];
}

// ----------------------------------------------------------------------------

Memory::~Memory()
{
   delete memoryMd;
}

// ----------------------------------------------------------------------------

const uint16_t*
Memory::readData(
   uint16_t  theAddress,
   uint32_t& theDataLength) const
{
   if (theAddress < sizeM)
   {
      uint32_t maxDataLength = sizeM - theAddress;
      theDataLength = theDataLength <= maxDataLength ? theDataLength : maxDataLength;
      return &memoryMd[theAddress];
   }
   return nullptr;
}

// ----------------------------------------------------------------------------

bool
Memory::writeBinary(
   uint16_t    theAddress,
   const char* theBinaryValue)
{
   if (addressM < sizeM)
   {
      uint16_t value = 0;
      uint16_t numberOfBits = 0;
      const char* current = theBinaryValue;
      while (*current != 0)
      {
         switch (*current)
         {
            case '0':
            {
               value <<= 1;
               numberOfBits++;
               break;
            }
            case '1':
            {
               value <<= 1;
               value |= 1;
               numberOfBits++;
               break;
            }
            case ' ':
            {
               break;
            }
            default:
            {
               return false;
            }
         }
         current++;
      }
      if (numberOfBits != 16)
      {
         return false;
      }
      memoryMd[theAddress] = value;
      return true;
   }
   return false;   
}

// ----------------------------------------------------------------------------

bool
Memory::writeHex(
   uint16_t theAddress,
   uint16_t theHexValue)
{
   if (addressM < sizeM)
   {
      memoryMd[theAddress] = theHexValue;
      return true;
   }
   return false;   
}

// ----------------------------------------------------------------------------

uint32_t
Memory::writeData(
   uint16_t        theAddress,
   const uint16_t* theData,
   uint16_t        theDataLength)
{
   if (addressM < sizeM)
   {
      uint32_t maxDataLength = sizeM - theAddress;
      uint32_t dataLength = theDataLength < maxDataLength ? theDataLength : maxDataLength;
      for (uint16_t i = 0; i < dataLength; i++)
      {
         memoryMd[addressM + i] = theData[i];
      }
      return dataLength;
   }
   return 0;
}

// ----------------------------------------------------------------------------

void
Memory::setAddress(
   uint16_t theAddress)
{
   addressM = theAddress;
}

// ----------------------------------------------------------------------------

uint16_t
Memory::read()
{
   return addressM < sizeM ? memoryMd[addressM] : 0xBAD1;
}

// ----------------------------------------------------------------------------

void
Memory::write(
   uint16_t theData)
{
   memoryMd[addressM] = theData;
} 


