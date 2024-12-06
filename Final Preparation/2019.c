#include "nios2_control.h"
#define TIMER_STATUS ((volatile unsigned int *) 0x00005000)
#define TIMER_CONTROL ((volatile unsigned int *) 0x00005004)
#define TIMER_START_LO ((volatile unsigned int *) 0x00005008)
#define TIMER_START_HI ((volatile unsigned int *) 0x0000500C)
#define LEDS ((volatile unsigned int *) 0x00006000)
#define INPORT_DATA ((volatile unsigned int *) 0x00006800)
#define INPORT_STATUS ((volatile unsigned int *) 0x00006804)
#define INPORT_CONTROL ((volatile unsigned int *) 0x00006808)
#define JTAG_UART_DATA ((volatile unsigned int *) 0x00006A00)
#define JTAG_UART_STATUS ((volatile unsigned int *) 0x00006A04)

unsigned int second_flag, input_data, data_placed, data_sum, global_count, total_data_arrivals, recent_data_arrivals;

void Init(void) {
    second_flag, input_data, data_placed, data_sum, global_count, total_data_arrivals, recent_data_arrivals = 0, 0, 0, 0, 0, 0, 0;  
    *TIMER_STATUS = 0x0; 
    *TIMER_START_LO = (6250000 & 0xFFFF); 
    *TIMER_START_HI = (6250000 >> 16); 
    *TIMER_CONTROL = 0x7; 
    *LEDS = 0x1; 
    *INPORT_CONTROL = 0x20; 
    NIOS2_WRITE_IENABLE(0x3);  // or (1 << 0 | 1 << 1)
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

void PrintHexDigit(unsigned int n) {
    unsigned int ch;
    if (n < 10)
        ch = n + '0'; 
    else 
        ch = 'A' + (n - 10); 
    PrintChar(ch); 
}

void HandleTimer(void) {
    *TIMER_STATUS = 0;
    global_count++; 
    unsigned int second_index = global_count % 8;  
    if (second_index == 0) 
        second_flag = 1; 
    *LEDS = (1 << second_index); 
}

void HandleInputPort(void) {
    *INPORT_STATUS = 0x0; 
    input_data = *INPORT_DATA; 
    data_placed = 1; 
}

void interrupt_handler(void) {
    unsigned int pending = NIOS2_READ_IPENDING(); 
    if (pending & 0x1)
        HandleTimer();
    if (pending & 0x2)
        HandleInputPort(); 
}

int main(void) {
    unsigned int average_data_arrivals = 0; 
    while (1) {
        if (data_placed) {
            recent_data_arrivals+=1;
            total_data_arrivals+=1;
            data_sum += input_data; 
            average_data_arrivals = data_sum / total_data_arrivals; 
            data_placed = 0; 
        }
        if (second_flag) {
            PrintChar('\b');
            PrintChar('\b');
            PrintHexDigit((recent_data_arrivals >> 4) & 0xF);  
            PrintHexDigit(recent_data_arrivals & 0xF); 
            recent_data_arrivals = 0; 
            second_flag = 0; 
        }
    }
    return 0; 
}