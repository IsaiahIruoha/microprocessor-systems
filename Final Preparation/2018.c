#include "nios2_control.h"

// Define memory-mapped I/O addresses
#define TIMER0_STATUS ((volatile unsigned int *) 0x00005000)
#define TIMER0_CONTROL ((volatile unsigned int *) 0x00005004)
#define TIMER0_START_LO ((volatile unsigned int *) 0x00005008)
#define TIMER0_START_HI ((volatile unsigned int *) 0x0000500C)
#define TIMER1_STATUS ((volatile unsigned int *) 0x00006000)
#define TIMER1_CONTROL ((volatile unsigned int *) 0x00006004)
#define TIMER1_START_LO ((volatile unsigned int *) 0x00006008)
#define TIMER1_START_HI ((volatile unsigned int *) 0x0000600C)
#define INPUT_DATA ((volatile unsigned int *) 0x00006700)
#define OUTPUT_DATA ((volatile unsigned int *) 0x00006800)
#define JTAG_UART_DATA ((volatile unsigned int *) 0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x00006A04)

// Global variable
unsigned int update_flag = 0;

// Initialization function
void Init(void) {
    *TIMER0_STATUS = 0;
    *TIMER0_START_HI = (50000000 >> 16); // Set high 16 bits of start value
    *TIMER0_START_LO = (50000000 & 0xFFFF); // Set low 16 bits of start value
    *TIMER0_CONTROL = 0x7;

    *TIMER1_STATUS = 0;
    *TIMER1_START_HI = (12500000 >> 16);
    *TIMER1_START_LO = (12500000 & 0xFFFF);
    *TIMER1_CONTROL = 0x7;

    *OUTPUT_DATA = 0;

    NIOS2_WRITE_IENABLE(0x03); // Enable Timer0 and Timer1 interrupts
    NIOS2_WRITE_STATUS(0x1); // Enable global interrupts
}

// Print a character using JTAG UART
void PrintChar(unsigned int ch) {
    unsigned int st;
    do {
        st = *JTAG_UART_STATUS;
        st = (st & 0xFFFF0000); // Check if JTAG UART is ready
    } while (st == 0);
    *JTAG_UART_DATA = ch; // Write character to UART
}

// Handle Timer0 interrupt
void HandleTimer0(void) {
    *TIMER0_STATUS = 0; // Clear Timer0 interrupt
    unsigned int currentTemp = *INPUT_DATA;
    if (currentTemp < 98)
        *OUTPUT_DATA = 128;
    else if (currentTemp > 102)
        *OUTPUT_DATA = 0;
}

// Handle Timer1 interrupt
void HandleTimer1(void) {
    *TIMER1_STATUS = 0; // Clear Timer1 interrupt
    update_flag = 1; // Set update flag
}

// Interrupt handler
void interrupt_handler(void) {
    unsigned int ipending = NIOS2_READ_IPENDING();
    if (ipending & 0x1)
        HandleTimer0();
    if (ipending & 0x2)
        HandleTimer1();
}

// Main function
int main(void) {
    Init();
    PrintChar(' ');
    while (1) {
        if (update_flag) {
            unsigned int currentTemp = *INPUT_DATA;
            PrintChar('\b');
            if (currentTemp > 102)
                PrintChar('H');
            else if (currentTemp <= 102)
                PrintChar('-');
            else if (currentTemp < 98)
                PrintChar('L');
            update_flag = 0;
        }
    }
    return 0;
}
