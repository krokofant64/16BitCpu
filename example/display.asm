Start: LDHZ R2 0x80
       LDHZ R1 0x04
       LDHZ R3 0x01
Loop:  STO R1 [R2 + 5]
       INC R1 R1 
       INC R2 R2
       LDH R2 0x80
       LDH R1 0x04
       JMP Loop
Res:

