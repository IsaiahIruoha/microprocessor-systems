#include "nios2_control.h"

#define TIMER0_STATUS ((volatile unsigned int *)0x00005000)
#define TIMER0_CONTROL ((volatile unsigned int *)0x00005004)
#define TIMER0_START_LO ((volatile unsigned int *)0x00005008)
#define TIMER0_START_HI ((volatile unsigned int *)0x0000500C)
#define TIMER1_STATUS ((volatile unsigned int *)0x00005010)
#define TIMER1_CONTROL ((volatile unsigned int *)0x00005014)
#define TIMER1_START_LO ((volatile unsigned int *)0x00005018)
#define TIMER1_START_HI ((volatile unsigned int *)0x0000501C)
#define INPUT_PORT ((volatile unsigned int *)0x00006000)
#define OUTPUT_PORT ((volatile unsigned int *)0x00006800)
#define JTAG_UART_DATA ((volatile unsigned int *)0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *)0x00006A04)

unsigned int timer1_flag, timer0_count;

void Init(void) {
    timer1_flag = 0;
    timer0_count = 0;

    *TIMER0_STATUS = 0x0;
    *TIMER0_CONTROL = 0x7;
    *TIMER0_START_LO = (12500000 & 0xFFFF);
    *TIMER0_START_HI = (12500000 >> 16);

    *TIMER1_STATUS = 0x0;
    *TIMER1_CONTROL = 0x7;
    *TIMER1_START_LO = (25000000 & 0xFFFF);
    *TIMER1_START_HI = (25000000 >> 16);

    *OUTPUT_PORT = 0x0FC0;

    NIOS2_WRITE_IENABLE(0x28); 
    NIOS2_WRITE_STATUS(0x1);  
}

void PrintChar(unsigned int ch) {
    unsigned int status;

    do {
        status = *JTAG_UART_STATUS;
        status = (status & 0xFFFF0000);
    } while (status == 0);

    *JTAG_UART_DATA = ch;
}

void HandleTimer0(void) {
    *TIMER0_STATUS = 0x0;
    timer0_count++;
    unsigned int led_index = (timer0_count % 4) * 2;
    *OUTPUT_PORT = (*OUTPUT_PORT & 0xFF00) | (0xC0 >> led_index);
}

void HandleTimer1(void) {
    *TIMER1_STATUS = 0x0;
    *OUTPUT_PORT = (*OUTPUT_PORT ^ 0xFF00);
    timer1_flag = 1;
}

void interrupt_handler(void) {
    unsigned int pending = NIOS2_READ_IPENDING();
    if (pending & 0x8) {
        HandleTimer0();
    }
    if (pending & 0x20) {
        HandleTimer1();
    }
}

int main(void) {
    Init();
    volatile unsigned int input_data;
    while (1) {
        if (timer1_flag) {
            input_data = *INPUT_PORT;
            if (input_data >= 0x8000) {
                PrintChar('H');
            } else {
                PrintChar('L');
            }
            PrintChar('\n');
            timer1_flag = 0;
        }
    }
    return 0;
}
