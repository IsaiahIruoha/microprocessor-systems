.equ JTAG_UART_BASE, 0x10001000
.equ DATA_OFFSET, 0
.equ STATUS_OFFSET, 4

.equ BUTTON_MASK, 0x10000058
.equ BUTTON_EDGE, 0x1000005C
.equ BUTTON1, 0x2
.equ LEDS, 0x10000010

.text
.global _start
.org 0x0000

_start:
    br main

.org 0x0020
    br isr   # Branch to ISR on interrupt

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

# Initialize Interrupts and I/O
Init:
    movia r3, COUNT
    movia r2, 0
    stw r2, 0(r3)          # Initialize COUNT to 0

    movia r3, BUTTON_MASK  # Load the address of BUTTON_MASK into r3
    movia r2, BUTTON1
    stwio r2, 0(r3)        # Use stwio to set button 1 to trigger interrupts

    movia r2, 1
    wrctl ienable, r2      # Enable interrupts for button 1

    movia r2, 1
    wrctl status, r2       # Enable global interrupts
    ret

# Interrupt Service Routine (ISR)
isr:
    subi ea, ea, 4         # Adjust ea for hardware interrupts

    rdctl r4, ipending      # Read ipending register
    movia r5, BUTTON1
    and r4, r4, r5          # Check if button 1 caused the interrupt
    beq r4, r0, isr_exit    # Exit if no interrupt from button 1

    # Toggle the right-most LED
    movia r3, LEDS          # Load the address of LEDS into r3
    ldwio r2, 0(r3)         # Load the current LED value using ldwio
    xori r2, r2, 1          # Toggle the right-most LED (bit 0)
    stwio r2, 0(r3)         # Store the updated LED value back using stwio

    # Clear the button interrupt
    movia r3, BUTTON_EDGE   # Load the address of BUTTON_EDGE into r3
    movia r2, BUTTON1       # Load the bit pattern for button 1
    stwio r2, 0(r3)         # Clear the button interrupt using stwio

isr_exit:
    eret                    # Return from interrupt

# Data Definitions
.org 0x1000
COUNT: .word 0
TEXT: .asciz "ELEC 371 Lab 2 by Isaiah Iruoha and Alex Morra \n  "

.end