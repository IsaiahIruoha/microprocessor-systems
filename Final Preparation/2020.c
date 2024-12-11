#include "nios2_control.h"

// Define memory-mapped I/O addresses
#define TIMER_STATUS ((volatile unsigned int *) 0x00005000)
#define TIMER_CONTROL ((volatile unsigned int *) 0x00005004)
#define TIMER_START_LO ((volatile unsigned int *) 0x00005008)
#define TIMER_START_HI ((volatile unsigned int *) 0x0000500C)
#define INPUT_PORT ((volatile unsigned int *) 0x00006800)
#define OUTPUT_PORT ((volatile unsigned int *) 0x00006000)
#define JTAG_UART_DATA ((volatile unsigned int *) 0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x00006A04)

// Global variables
unsigned int low_count, high_count, interrupt_count, second_flag;

// Initialization function
void Init(void) {
    // Initialize global variables
    low_count = high_count = interrupt_count = second_flag = 0;

    // Configure timer
    *TIMER_STATUS = 0x0;
    *TIMER_CONTROL = 0x7;
    *TIMER_START_LO = (12500000 & 0xFFFF); // Set low 16 bits
    *TIMER_START_HI = (12500000 >> 16);    // Set high 16 bits

    // Initialize output port
    *OUTPUT_PORT = 0xC0;

    // Enable interrupts
    NIOS2_WRITE_IENABLE(0x1); // Enable timer interrupt
    NIOS2_WRITE_STATUS(0x1); // Enable global interrupts
}

// Function to print a character using JTAG UART
void PrintChar(unsigned int ch) {
    unsigned int st;
    do {
        st = *JTAG_UART_STATUS;
        st = (st & 0xFFFF0000); // Check if UART is ready
    } while (st == 0);
    *JTAG_UART_DATA = ch; // Write character to UART
}

// Timer interrupt handler
void HandleTimer(void) {
    *TIMER_STATUS = 0x0; // Clear timer interrupt
    unsigned int led_index = interrupt_count % 4; // Determine LED index
    interrupt_count += 1; // Increment interrupt counter

    // Update output port
    *OUTPUT_PORT = (0xc0 >> (led_index*2));
    if (led_index == 3) {
        second_flag = 1; // Set second flag every 4 interrupts
    }
}

// Main interrupt handler
void interrupt_handler(void) {
    unsigned int ipending = NIOS2_READ_IPENDING(); // Check pending interrupts
    if (ipending & 0x1) {
        HandleTimer(); // Handle timer interrupt
    }
}

// Main function
int main(void) {
    volatile unsigned int input_data; // Variable to hold input data

    Init(); // Initialize system

    while (1) {
        if (second_flag) {
            input_data = *INPUT_PORT; // Read input data

            // Check input data and update counts
            if (input_data < 128) {
                low_count += 1;
                PrintChar('L'); // Print 'L' for low data
            } else {
                high_count += 1;
                PrintChar('H'); // Print 'H' for high data
            }

            second_flag = 0; // Reset second flag
        }
    }

    return 0;
}
