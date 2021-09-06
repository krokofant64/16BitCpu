# KF16 - 16-bit CPU
The goal of this project is to design a simple 16-bit CPU, the KF16 (short for KrokoFant16 :smile:).

The CPU has a RISC inspired instruction set and it can address up to 64 K of 16-bit words.

The table below shows the encoding of the instructions and their mnemonics:

![Instruction format of KF16 CPU](https://github.com/krokofant64/16BitCpu/blob/master/doc/InstructionFormat.png "KF 16 CPU instruction format")

# Instructions
## ALU Instructions
### ADD - Add
Operation
```
R_dest = R_src1 + R_src2
```
Flags
```
N Z C O I
+ + + + -
```
Example:
```
ADD R3 R2 R1
```
### ADC - Add with carry
Operation
```
R_dest = R_src1 + R_src2 + Carry
```
Flags
```
N Z C O I
+ + + + -
```
Example:
```
ADC R5 R4 R1
```
### SUB - Subtract
Operation
```
R_dest = R_src1 - R_src2
```
Flags
```
N Z C O I
+ + + + -
```
Example:
```
SUB R3 R2 R1
```
### SBC - Subtract with carry
Operation
```
R_dest = R_src1 - R_src2 - Carry
```
Flags
```
N Z C O I
+ + + + -
```
Example:
```
SBC R4 R3 R2
```
### AND - Logical AND
Operation
```
R_dest = R_src1 and R_src2
```
Flags
```
N Z C O I
+ + - - -
```
Example:
```
AND R4 R3 R2
```
### OR - Logical OR
Operation
```
R_dest = R_src1 or R_src2
```
Flags
```
N Z C O I
+ + - - -
```
Example:
```
OR R3 R2 R1
```
### XOR - Logical XOR
Operation
```
R_dest = R_src1 xor R_src2
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
XOR R3 R2 R1
```
### NOT - Logical NOT
Operation
```
R_dest = not R_src
```
Flags
```
N Z C O I
+ + - - -
```
Example: R3 = not R2
```
NOT R3 R2
```
## Load from Register Instructions
### LD - Load from register
Operation
```
R_dest = R_src
```
Flags
```
N Z C O I
+ + - - -
```
Example:
```
LD R3 R1
```
### LDL - Load low byte from register
Operation
```
R_dest.low_byte = R_src.low_byte

R_dest.high_byte = 0
```
Flags
```
N Z C O I
+ + - - -
```
Example: R3 = R2.low_byte
```
LDL R3 R2
```
### LDH - Load high byte from register
Operation
```
R_dest.low_byte = R_src.high_byte
R_dest.high_byte = 0
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
LDH R3 R2
```
### SWP - Load swapped bytes from register
Operation
```
R_dest.hight_byte = R_src.low_byte
R_dest.low_byte = R_src.high_byte
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
SWP R3 R2
```
## Other Arithmetic Instructions
### INC - Increment register
Operation
```
R_dest = R_src + 1
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
INC R3 R2
```
### DEC - Decrement register
Operation
```
R_dest = R_src - 1
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
DEC R3 R2
```
### CMP - Compare register
Operation
```
R_src1 - R_src2
```
Flags
```
N Z C O I
+ + + + -
```
Example: 
```
CMP R3 R2
```
## Shift Instructions
### SHR - Logic shift right
### SHL, ASHL - Logic and arithmetic shift left
### ASHR - Arithmetic shift right
### ROR - Rotate right
### ROL - Rotate left
## Flag Manipulation Instructions
### CLC - Clear carry flag
Operation
```
C = 0
```
Flags
```
N Z C O I
- - 0 - -
```
Example: 
```
CLC
```
### SEC - Set carry flag
Operation
```
C = 1
```
Flags
```
N Z C O I
- - 1 - -
```
Example: 
```
SEC
```
### CLI - Clear interrupt enable flag
Operation
```
I = 0 
```
Flags
```
N Z C O I
- - - 0 -
```
Example: 
```
CLI
```
### SEI - Set interrupt enable flag
Operation
```
I = 1
```
Flags
```
N Z C O I
- - - 1 -
```
Example: 
```
SEI
```
## Stack Instructions
### PSH - Push to stack
### POP - Pop from stack
### RET - Return from subroutine
### RTI - Return from interrupt
## Conditional Branch Instructions
### BCS - Branch if carry flag is set
Operation
```
if C == 1 then
   PC = R_base + offset
endif
```
Flags
```
N Z C O I
- - - - -
```
Example: 
```
BCS PC +5
```
### BCC - Branch if carry flag is clear
Operation
```
if C == 0 then
   PC = R_base + offset
endif
```
Flags
```
N Z C O I
- - - - -
```
Example: 
```
BCC R2 +17
```
### BZS - Branch if zero flag is set
### BZC - Branch if zero flag is clear
### BNS - Branch if negative flag is set
### BNC - Branch if negative flag is clear
### BOS - Branch if overflow flag is set
### BOC - Branch if overflow flag is clear
## Load Immediate Value Instructions
### LDL - Load low byte from immediate value
### LDH - Load high byte from immediate value
### LDLZ - Load loaw byte from immediate value and clear high byte
### LDHZ - Load high byte from immediate value and clear low byte
## Jump Instruction
### JMP - Jump
## Jump to Subroutine Instruction
### JSR - Jump to subroutine
## Load from Memory Instruction
### LD - Load from memory
Operation
```
R_dest = mem[R_base + offset]
```
Flags
```
N Z C O I
+ + - - -
```
Example: 
```
LD R4 [SP -2]
```
## Store in Memory Instruction
### STO - Store in memory
Operation
```
mem[R_base + offset] = R_src
```
Flags
```
N Z C O I
- - - - -
```
Example: 
```
STO R3 [R2 +18]
```
