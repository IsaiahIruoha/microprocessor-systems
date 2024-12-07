#include "nios2_control.h"
#define TIMER0_STATUS ((volatile unsigned int *) 0x00005000)
#define TIMER0_CONTROL ((volatile unsigned int *) 0x00005004)
#define TIMER0_START_LO ((volatile unsigned int *) 0x00005008)
#define TIMER0_START_HI ((volatile unsigned int *) 0x0000500C)
#define TIMER1_STATUS ((volatile unsigned int *) 0x00006000)
#define TIMER1_CONTROL ((volatile unsigned int *) 0x00006004)
#define TIMER1_START_LO ((volatile unsigned int *) 0x00006008)
#define TIMER1_START_HI ((volatile unsigned int *) 0x0000600C)
#define INPUT_DATA ((volatile unsigned int *) 0x00006700)
#define LEDS ((volatile unsigned int *) 0x00006800)
#define JTAG_UART_DATA ((volatile unsigned int *) 0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x00006A04)

unsigned int input_data, timer1_flag, input_sum, input_avg, input_count, timer0_count; 

void Init(void) {
    input_data, timer1_flag, input_sum, input_avg, input_count, timer0_count = 0,0,0,0,0,0;
    *TIMER0_START_HI = (6250000 >> 16); 
    *TIMER0_START_LO = (6250000 & 0xFFFF);
    *TIMER0_CONTROL = 0x7; 
    *TIMER0_STATUS = 0x0; 
    *TIMER1_START_HI = (25000000 >> 16);
    *TIMER1_START_LO = (25000000 & 0xFFFF);
    *TIMER1_CONTROL = 0x7; 
    *TIMER1_STATUS = 0x0;
    *LEDS = 0x80;
    NIOS2_WRITE_IENABLE(0x60);
    NIOS2_WRITE_STATUS(0x1); 
}

void PrintChar(unsigned int ch) {
    unsigned int st; 
    do {
        st = *JTAG_UART_STATUS;
        st = (st & 0xFFFF0000);
    } while (st == 0); 
    *JTAG_UART_DATA = ch; 
}

void HandleTimer0(void) {
    *TIMER0_STATUS = 0x0; 
    unsigned int led_index; 
    timer0_count += 1;
    led_index = timer0_count % 8; 
    *LEDS = (0x80 >> led_index);
}

void HandleTimer1(void) {
    *TIMER1_STATUS = 0x0; 
    timer1_flag = 1; 
}

void interrupt_handler(void) {
    unsigned int ipending = NIOS2_READ_IPENDING();
    if (ipending & 0x20)
        HandleTimer0();
    if (ipending & 0x40)
        HandleTimer1(); 
}

int main (void) {
    unsigned int previous = 0; 
    while(1) {
        if (timer1_flag) {
            previous = input_data; 
            input_data = *INPUT_DATA; 
            if (input_data < previous) 
                PrintChar('<');
            else if (input_data > previous) 
                PrintChar('>');
            else 
                PrintChar('='); 
            PrintChar('\n'); 
            input_sum += input_data;
            input_count += 1;
            input_avg = input_sum / input_count; 
            timer1_flag = 0; 
        }
    }
    return 0; 
}
