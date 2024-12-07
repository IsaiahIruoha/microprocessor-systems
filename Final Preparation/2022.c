#include "nios2_control.h"
#define TIMER0_STATUS ((volatile unsigned int *) 0x00005000)
#define TIMER0_CONTROL ((volatile unsigned int *) 0x00005004)
#define TIMER0_START_LO ((volatile unsigned int *) 0x00005008)
#define TIMER0_START_HI ((volatile unsigned int *) 0x0000500C)
#define TIMER1_STATUS ((volatile unsigned int *) 0x00005100)
#define TIMER1_CONTROL ((volatile unsigned int *) 0x00005104)
#define TIMER1_START_LO ((volatile unsigned int *) 0x00005108)
#define TIMER1_START_HI ((volatile unsigned int *) 0x0000510C)
#define INPUT_DATA ((volatile unsigned int *) 0x00006700)
#define OUTPUT_DATA ((volatile unsigned int *) 0x00006800)
#define JTAG_UART_DATA ((volatile unsigned int *) 0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x00006A04)

unsigned int timer1_flag, greater_count, less_count, equal_count, timer0_count;

void Init(void) {
    timer1_flag, greater_count, less_count, equal_count, timer0_count = 0,0,0,0,0; 
    *TIMER0_STATUS = 0x0;
    *TIMER0_CONTROL = 0x7; 
    *TIMER0_START_HI = (6250000 >> 16);
    *TIMER0_START_LO = (6250000 & 0xFFFF); 
    *TIMER1_STATUS = 0x0; 
    *TIMER1_CONTROL = 0x7; 
    *TIMER1_START_HI = (25000000 >> 16);   
    *TIMER1_START_LO = (25000000 & 0xFFFF); 
    *OUTPUT_DATA = 0x1; 
    NIOS2_WRITE_IENABLE(0xC); 
    NIOS2_WRITE_STATUS(0x1); 
}

void PrintChar(unsigned int ch) {
    unsigned int st;
    do {
        st = *JTAG_UART_STATUS;
        st = (st & 0xFFFF0000); 
    } while (st == 0); 
    *JTAG_UART_STATUS = ch; 
}

void HandleTimer0(void){
    *TIMER0_STATUS = 0x0;
    timer0_count += 1;
    unsigned int led_index = timer0_count % 8; 
    unsigned int store_higher_output = (*OUTPUT_DATA & 0xFF00); 
    *OUTPUT_DATA = store_higher_output | (1 << led_index); 
}

void HandleTimer1(void){
    *TIMER1_STATUS = 0x0; 
    timer1_flag = 1;
}

void interrupt_handler(void){
    unsigned int ipending = NIOS2_READ_IPENDING();
    if (ipending & 0x4)
        HandleTimer0();
    if (ipending & 0x8)
        HandleTimer1();
}

int main(void){
    unsigned int previous_data = 0; 
    unsigned int current_data = 0;
    unsigned int store_lower_output = 0;
    while(1){
        if(timer1_flag) {
            previous_data = current_data;
            current_data = *INPUT_DATA;
            if(current_data > previous_data){
                greater_count += 1;
                PrintChar('G');
            } else if (current_data < previous_data) {
                less_count += 1;
                PrintChar('L');
            } else {
                equal_count += 1;
                PrintChar('E');
            }
            PrintChar('\n');   
            store_lower_output = (*OUTPUT_DATA & 0x00FF); 
            *OUTPUT_DATA = (store_lower_output | current_data); 
        }
    }
    return 0;
}