.equ JTAG_UART_BASE, 0x10001000
.equ DATA_OFFSET, 0
.equ STATUS_OFFSET, 4
.equ WSPACE_MASK, 0xFFFF

.text
.global _start
.org 0
_start:

main:
 movia sp, 0x7FFFFC
 call 	SwitchSetting
 addi 	r2, r2, '0'
 call	PrintChar
 mov 	r3, r2
 
loop:
 call 	SwitchSetting
 mov 	r4, r2
 beq 	r4, r3, end_if
 movi 	r2, '\b'
 call 	PrintChar
 addi	r2, r4, '0'
 call	PrintChar
 mov	r3, r4
 
end_if:
 br		loop
end_loop:


# movia r2, 0x2A # Load the ASCII value of '*' into r2
# call PrintChar # Call the function to print the character
break

PrintChar:
 subi sp, sp, 8
 stw r3, 4(sp)
 stw r4, 0(sp)

 movia r3, JTAG_UART_BASE

pc_loop:
 ldwio r4, STATUS_OFFSET(r3)
 andhi r4, r4, WSPACE_MASK
 beq r4, r0, pc_loop
 stwio r2, DATA_OFFSET(r3)
 ldw r3, 4(sp)
 ldw r4, 0(sp)
 addi sp, sp, 8
 ret
 
 SwitchSetting:
 movia	r2, 0x10000040
 ldwio	r2, 0(r2)
 andi	r2, r2, 0x1
 ret

 .org 0x1000
	
	