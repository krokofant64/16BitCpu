# KF16 - 16-bit CPU
The goal of this project is to design a simple 16-bit CPU, the KF16 (short for KrokoFant16 :smile:).
The CPU has a RISC inspired instruction set and it can address up to 64 K of 16-bit words.

The table below shows the encoding of the instructions and their mnemonics:

![Instruction format of KF16 CPU](https://github.com/krokofant64/16BitCpu/blob/master/doc/InstructionFormat.png "KF 16 CPU instruction format")

# Instructions
## ALU Instructions
### ADD - Add
RDest = RSrc1 + RSrc2

Modified flags: C, Z, N, O

Example: R3 = R2 + R1

ADD R3 R2 R1

### ADC - Add with carry
RDest = RSrc1 + RSrc2 + C

Modified flags: C, Z, N, O

### SUB - Subtract
RDest = RSrc1 - RSrc2

Modified flags: C, Z, N, O

### SBC - Subtract with carry
RDest = RSrc1 - RSrc2 - C

Modified flags: C, Z, N, O

### AND - Logical AND
### OR - Logical OR
### XOR - Logical XOR
### NOT - Logical NOT
## Load from Register Instructions
### LD - Load from register
### LDL - Load low byte from register
### LDH - Load high byte from register
### SWP - Load swapped bytes from register
## Other Arithmetic Instructions
### INC - Increment register
### DEC - Decrement register
### CMP - Compare register


