Start: LDLZ SP 0x10
LD R0 Op1
LD R1 Op2
JSR Add
STO R2 Res
JMP Start
Add: 
ADD R2 R0 R1
RET
Op1: .data 10
Op2: .data 13
Res: