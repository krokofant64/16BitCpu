Start:   LD R0 Op1
         LD R2 Op2
         ; R0 * R2 -> R1 R2; temp R3
         LDLZ R1 0
         LDLZ R3 16
         SHR R2 R2
BitLoop: BCC NoAdd
         ADD R1 R1 R0
NoAdd:   SHR R1 R1
         ROR R2 R2
         DEC R3 R3
         BZC BitLoop
         HLT
Op1:     .data 500
Op2:     .data 13

