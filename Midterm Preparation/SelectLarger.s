.text
.global _start
.org 0x0000

_start:
	movia sp, 0x7FFFFC
	movia r2, ListZ
	movia r3, ListX
	movia r4, ListY
	movia r5, N
	ldw r5, 0(r5) 
	call SelectLarger
	
_end: 
	break
	
	SelectLarger:
				subi sp, sp, 24
				stw r2, 20(sp)
				stw r3, 16(sp)
				stw r4, 12(sp)
				stw r5, 8(sp)
				stw r6, 4(sp)
				stw r7, 0(sp)
			LOOP:
				ldw r6, 0(r3)
				ldw r7, 0(r4) 
				bgt r6, r7, THEN
				stw r7, 0(r2)
				br END_IF
			TEHN:
				stw r6, 0(r2)
			END_IF:
				addi r2, r2, 4
				addi r3, r3, 4
				addi r4, r4, 4
				subi r5, r5, 1
				bgt r5, r0, LOOP
			ENDLOOP:
				ldw r7, 0(sp)
				ldw r6, 4(sp)
				ldw r5, 8(sp)
				ldw r4, 12(sp)
				ldw r3, 16(sp)
				ldw r2, 20(sp)
				addi sp, sp, 24
				ret
				
.org 0x1000
ListX: .word 9, 13, 0xFF, 167
ListY: .word 24, -5, 0, 192
ListZ: .skip 16
N: .word 4