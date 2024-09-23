	.set	noat
	.text
	.global	_start
	.org 0
_start:
	addi	r1, r0, 0xABC
	stw		r1, 0x2A00(r0)
	ldw		r1, 0x2A00(r0)
	br		_start