       LDHZ R4 0x00
Start: LDHZ SP 0x01 
       LDH R3 0xFF
       LDL R3 0xFD
       LD  R2 [R3]
       LDLZ R0 1
       LDLZ R1 4
       JSR DisplayReg
       LDL R3 0xFE
       LD R2 [R3]
       LDLZ R0 1
       LDLZ R1 5
       JSR DisplayReg
       LD R2 R4
       LDLZ R0 1
       LDLZ R1 7
       JSR DisplayReg
       INC R4 R4
       JMP Start

; [in]   R0 = column
; [in]   R1 = row
; [in]   R2 = reg
DisplayReg:   PSH R3
              PSH R4
              PSH R5
              JSR GetCursorPos
              LDLZ R4 0x0F
              LDH R3 R2
              SHR R3 R3
              SHR R3 R3
              SHR R3 R3
              SHR R3 R3
              JSR WriteHexDigit
              LDH R3 R2
              AND R3 R3 R4
              JSR WriteHexDigit
              LDL R3 R2
              SHR R3 R3
              SHR R3 R3
              SHR R3 R3
              SHR R3 R3
              JSR WriteHexDigit
              LDL R3 R2
              AND R3 R3 R4
              JSR WriteHexDigit
              POP R5
              POP R4
              POP R3

; [in]   R0 = column
; [in]   R1 = row
; [out]  R5 = position 
GetCursorPos: PSH R3
              LDHZ R5 0x80
              LD R3  R1 ; Multiply row by 40 
              SHL R3 R3
              SHL R3 R3
              SHL R3 R3
              ADD R5 R5 R3
              SHL R3 R3
              SHL R3 R3
              ADD R5 R5 R3
              ADD R5 R5 R0 ; Add column
              POP R3
              RET

; [in]   R5 = pos
; [in]   R3 = hex digit
WriteHexDigit: PSH R4
               LDLZ R4 10
               CMP R3 R4
               BCC NotDecimal
               LDLZ R4 48 ; '0'
               ADD R3 R3 R4
               JMP StoreDigit
NotDecimal:    LDLZ R4 55 ; 'A' - 10
               ADD R3 R3 R4
StoreDigit:    LDH R3 0x3C; White background red text
               STO R3 [R5] 
               INC R5 R5
               POP R4
               RET
