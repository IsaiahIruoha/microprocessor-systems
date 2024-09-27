#-----------------------------------------------------------------------------
# This template source file for ELEC 371 Lab 2 experimentation with interrupts
# also serves as the template for all assembly-language-level coding for
# Nios II interrupt-based programs in this course. DO NOT USE the approach
# shown in the vendor documentation for the DE0 Basic (or Media) Computer.
# The approach illustrated in this template file is far simpler for learning.
#
# Dr. N. Manjikian, Dept. of Elec. and Comp. Eng., Queen's University
#-----------------------------------------------------------------------------

        .text

        .global _start

#-----------------------------------------------------------------------------
# Define symbols for memory-mapped I/O register addresses and use them in code
#-----------------------------------------------------------------------------

#PrintChar and  PrintString references

        .equ JTAG_UART_BASE, 0x10001000
        .equ DATA_OFFSET, 0
        .equ STATUS_OFFSET, 4

# mask/edge registers for pushbutton parallel port

        .equ BUTTON_MASK, 0x10000058
        .equ BUTTON_EDGE, 0x1000005C

# pattern corresponding to the bit assigned to button1 in the registers above

        .equ BUTTON1, 0x2

# data register for LED parallel port
        .equ LEDS, 0x10000010
		
# data register for HEX displays
		.equ HEX_DISPLAYS, 0x10000020
		
# timer symbols
		.equ TIMER_STATUS, 0x10002000
		.equ TIMER_CONTROL, 0x10002004
		.equ TIMER_START_LO, 0x10002008
		.equ TIMER_START_HI, 0x1000200C

        .org 0x0000 # this is the _reset_ address 

_start:
        br main # branch to actual start of main() routine 

.org 0x0020
        br isr   # Branch to ISR on interrupt

#-----------------------------------------------------------------------------
# The actual program code (incl. service routine) can be placed immediately
# after the second branch above, or another .org directive could be used
# to place the program code at a desired address (e.g., 0x0080). It does not
# matter because the _start symbol defines where execution begins, and the
# branch at that location simply forces execution to continue where desired.
#-----------------------------------------------------------------------------

main:
        movia sp, 0x7FFFFC    # Initialize stack pointer

        call Init             # Initialize interrupts and I/O

        movia r2, TEXT
        call PrintString       # Print the string
        br main_loop           # Jump to main loop

main_loop:

        movia r3, COUNT        # Load the address of COUNT into r3
        ldw r2, 0(r3)          # Load current value of COUNT
        addi r2, r2, 1         # Increment COUNT
        stw r2, 0(r3)          # Store incremented value back to COUNT

        br main_loop           # Infinite loop

PrintChar:
        subi sp, sp, 8
        stw r4, 0(sp)
        stw r3, 4(sp)
        movia r4, JTAG_UART_BASE

pc_loop:
        ldwio r3, 4(r4)         # Load status register using ldwio
        andhi r3, r3, 0xFFFF
        beq r3, r0, pc_loop     # Wait until UART is ready
        stwio r2, 0(r4)         # Output the character using stwio

        ldw r3, 4(sp)
        ldw r4, 0(sp)
        addi sp, sp, 8
        ret

PrintString:
        subi sp, sp, 12
        stw r3, 8(sp)
        stw r2, 4(sp)
        stw ra, 0(sp)
        mov r3, r2

ps_loop:
        ldbu r2, 0(r3)           # Load next character in the string
        beq r2, r0, ps_endloop   # End of string (null-terminated)
        call PrintChar           # Print character
        addi r3, r3, 1
        br ps_loop

ps_endloop:
        ldw r3, 8(sp)
        ldw r2, 4(sp)
        ldw ra, 0(sp)
        addi sp, sp, 12
        ret
		
UpdateHexDisplay:
		subi sp, sp, 8
		stw	 r2, 4(sp)
		stw	 r3, 0(sp)
		
		movia	r2, HEX_DISPLAYS

uhd_if:
		movi	r3, 0x7F
		bne 	r2, r3, uhd_else
		slli	r3, r3, 24
		br		uhd_end_if

uhd_else:
		srli	r3, r3, 8

uhd_end_if:
		stwio	r3, 0(r2)
		
		ldw		r3, 0(sp)
		ldw		r2, 4(sp)
		addi	sp, sp, 8
		ret	
	
#-----------------------------------------------------------------------------
# This subroutine should encompass preparation of I/O registers as well as
# special processor registers for recognition and processing of interrupt
# requests. Initialization of data variables in memory can also be done here.
#-----------------------------------------------------------------------------

# Initialize Interrupts and I/O

Init:
		subi	sp, sp, 8 # make it modular -- save/restore registers
		stw		r2, 4(sp)
		stw		r3, 0(sp)
		
		movia 	r2, TIMER_START_LO 	# set start_lo
		movia	r3, 0x017D7840
		srli	r3, r3, 1
		stwio	r3, 0(r2)		
		
		movia 	r2, TIMER_START_HI 	# set start_hi
		srli	r3, r3, 16
		stwio	r3, 0(r2)	
		
		movia	r2, TIMER_CONTROL
		movi	r3, 7 			# 7 = 0111_2; not stop, start, continuous, interrupt enable
		stwio	r3, 0(r2)
		
		movia	r2, TIMER_STATUS	# set timer status to 0
		stwio	r0, 0(r2)
		
		movia	r2, HEX_DISPLAYS 	# Inititally turn on left most 8
		movi	r3, 0x7F
		slli	r3, r3, 24
		stwio	r3, 0(r2)
	
        movia r3, COUNT
        movia r2, 0
        stw r2, 0(r3)          # Initialize COUNT to 0

        movia r3, BUTTON_MASK  # Load the address of BUTTON_MASK into r3
        movia r2, BUTTON1
        stwio r2, 0(r3)        # Use stwio to set button 1 to trigger interrupts

        movia r2, 0x3
        wrctl ienable, r2      # Enable interrupts for button 1

        movia r2, 1
        wrctl status, r2       # Enable global interrupts
		
		ldw		r2, 4(sp)
		ldw		r3, 0(sp)
		addi	sp, sp, 8
        ret

#-----------------------------------------------------------------------------
# The code for the interrupt service routine is below. Note that the branch
# instruction at 0x0020 is executed first upon recognition of interrupts,
# and that branch brings the flow of execution to the code below. Therefore,
# the actual code for this routine can be anywhere in memory for convenience.
# This template involves only hardware-generated interrupts. Therefore, the
# return-address adjustment on the ea register is performed unconditionally.
# Programs with software-generated interrupts must check for hardware sources
# to conditionally adjust the ea register (no adjustment for s/w interrupts).
#-----------------------------------------------------------------------------

# Interrupt Service Routine (ISR)
isr:
	# Save registers (except ea which we will adjust)
	subi	sp, sp, 20
	stw		ra, 16(sp)
	stw		r5, 12(sp)	
	stw		r4, 8(sp)
	stw		r3, 4(sp)	
	stw		r2, 0(sp)
        subi ea, ea, 4         # Adjust ea for hardware interrupts

        rdctl r4, ipending      # Read ipending register

        movia r5, BUTTON1
        and r4, r4, r5          # Check if button 1 caused the interrupt
        beq r4, r0, TESTTMR    # Exit if no interrupt from button 1
		
		button_code:
        # Toggle the right-most LED
        movia r3, LEDS          # Load the address of LEDS into r3
        ldwio r2, 0(r3)         # Load the current LED value using ldwio
        xori r2, r2, 1          # Toggle the right-most LED (bit 0)
        stwio r2, 0(r3)         # Store the updated LED value back using stwio

        # Clear the button interrupt
        movia r3, BUTTON_EDGE   # Load the address of BUTTON_EDGE into r3
        movia r2, BUTTON1       # Load the bit pattern for button 1
        stwio r2, 0(r3)         # Clear the button interrupt using stwio
		
TESTTMR:
		rdctl	r4, ipending
		movia r5, 0x2
		and r4, r4, r5
		beq	r4, r0, isr_exit
		
		# Clear the timer interrupt
        movia r3, TIMER_STATUS  
        stwio r0, 0(r3)         
		
		call	UpdateHexDisplay


isr_exit:	
	ldw		ra, 16(sp)
	ldw		r5, 12(sp)	
	ldw		r4, 8(sp)
	ldw		r3, 4(sp)	
	ldw		r2, 0(sp)
	addi	sp, sp, 20
    	eret                    # Return from interrupt

#-----------------------------------------------------------------------------
# Definitions for program data, incl. anything shared between main/isr code
#-----------------------------------------------------------------------------

# Data Definitions
	.org 0x1000

COUNT:  .word 0
TEXT:   .asciz "ELEC 371 Lab 2 by Isaiah Iruoha and Alex Morra \n  "

	.end
	
