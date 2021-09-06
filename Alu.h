#ifndef ALU_H
#define ALU_H

class Alu
{
public:

   enum class AluOperation
   { 
      AddE,          // Result = Op1 + Op2 // ADD : Op1 = RSrc1; Op2 = RSrc2
                     //                    // B** : Op1 = RBase; Op2 = Offs7
                     //                    // INC : Op1 = RSrc;  Op2 = 0x0001
                     //                    // JMP : Op1 = RBase; Op2 = Offs10
                     //                    // JSR : Op1 = RBase; Op2 = Offs10
                     //                    // LD  : Op1 = RBase; Op2 = Offs7
                     //                    // STO : Op1 = RBase; Op2 = Offs7
                     //                    // POP : Op1 = SP;    Op2 = 0x0001
                     //                    // RET : Op1 = SP;    Op2 = 0x0001
                     //                    // RTI : Op1 = SP;    Op2 = 0x0001
      AdcE,
      SubE,          // Result = Op1 - Op2 // SUB : Op1 = RSrc1; Op2 = RSrc2
                     //                    // DEC : Op1 = RSrc;  Op2 = 0x0001
                     //                    // CMP : Op1 = RSrc1; Op2 = RSrc2
                     //                    // PSH : Op1 = SP;    Op2 = 0x0001
                     //                    // JSR : Op1 = RBase; Op2 = Offs10
      SbcE,
      AndE,
      OrE,
      XorE,
      NotE
   };

   enum class ShiftOperation
   {
      ShrE,
      ShlE,
      AShrE,
      RorE,
      RolE,
   };

   enum class LoadOperation
   {
     CopyE,          // Result.low = Op2.low;  Result.high = Op2.high; // LD        : Op2 = RSrc
     SwapE,          // Result.low = Op2.high; Result.high = Op2.low;  // SWP       : Op2 = RSrc
     Op2LowOp1LowE,  // Result.low = Op2.low;  Result.high = Op1.low   // LDL       : Op2 = RSrc;  Op1 = 0x0000
                     // Result.low = Op2.low;  Result.high = Op1.low   // LDLZ imm8 : Op2 = imm8;  Op1 = 0x0000
     Op2HighOp1LowE, // Result.low = Op2.high; Result.high = Op1.low   // LDH       : Op2 = RDest; Op1 = 0x0000
     Op2LowOp1HighE, // Result.low = Op2.low;  Result.high = Op1.high  // LDL imm8  : Op2 = imm8;  Op1 = RDest
     Op1LowOp2LowE   // Result.low = Op1.low;  Result.high = Op2.low   // LDH imm8  : Op2 = imm8;  Op1 = RDest
                     // Result.low = Op1.low;  Result.high = Op2.low   // LDHZ imm8 : Op2 = imm8;  Op1 = 0x0000
   };

   

private:

};

#endif
