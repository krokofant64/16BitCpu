Start:    LD R1 Divident
          LD R2 Divisor
          ; Divide R1 by R2. Result in R1. Remainder in R0. Uses R3.
          LDLZ R0 0
          LDLZ R3 16
          SHL R1 R1
BitLoop:  ROL R0 R0
          CMP R2 R0
          BCC NoSub
          SUB R0 R0 R2
          SEC 
NoSub:    ROL R1 R1
          DEC R3 R3
          BZC BitLoop
          HLT
Divident: .data 100
Divisor:  .data 13

