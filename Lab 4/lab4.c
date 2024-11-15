#include "nios2_control.h"
#include "chario.h"

/* place additional #define macros here */
#define TIMER1_STATUS	((volatile unsigned int *) 0x10004020)

#define TIMER1_CONTROL	((volatile unsigned int *) 0x10004024)

#define TIMER1_START_LO	((volatile unsigned int *) 0x10004028)

#define TIMER1_START_HI	((volatile unsigned int *) 0x1000402C)

#define TIMER1_SNAP_LO	((volatile unsigned int *) 0x10004030)

#define TIMER1_SNAP_HI	((volatile unsigned int *) 0x10004034)


#define TIMER2_STATUS	((volatile unsigned int *) 0x10004040)

#define TIMER2_CONTROL	((volatile unsigned int *) 0x10004044)

#define TIMER2_START_LO	((volatile unsigned int *) 0x10004048)

#define TIMER2_START_HI	((volatile unsigned int *) 0x1000404C)

#define TIMER2_SNAP_LO	((volatile unsigned int *) 0x10004050)

#define TIMER2_SNAP_HI	((volatile unsigned int *) 0x10004054)

#define TIMER_TO_BIT 0x1

#define LEDS	((volatile unsigned int *) 0x10000010)

#define SWITCHES_DATA (volatile unsigned int *) 0x10000040

#define HEX_DISPLAYS (volatile unsigned int*) 0x10000020

/* define global program variables here */
#define TIMER1_INTERVAL 10000000 //0.2 seconds

#define TIMER2_INTERVAL 25000000 //0.5 second
unsigned int timer_count = 0;
volatile int timer_flag_1 = 0;
volatile int timer_flag_2 = 0;

unsigned int led_table[] =
{
0x201, 0x102, 0x84, 0x48, 0x30,
};

/* place additional functions here */



/*-----------------------------------------------------------------*/

/* this routine is called from the_exception() in exception_handler.c */

void interrupt_handler(void)
{
	unsigned int ipending;

	/* read current value in ipending register */
	ipending = NIOS2_READ_IPENDING();

	/* do one or more checks for different sources using ipending value */
	if (ipending & (1<<14)){
      
      /* remember to clear interrupt sources */
      *TIMER1_STATUS = TIMER_TO_BIT;
	  
	  *LEDS = led_table[timer_count%5];
	  timer_count++;
	  timer_flag_1 = 1;
   }
   

   if (ipending & (1<<15)){
      
      *TIMER2_STATUS = TIMER_TO_BIT;
	  
	  timer_flag_2 = 1;
   }

	
	/* remember to clear interrupt sources */
}

/*-----------------------------------------------------------------*/

void Init (void)
{
	/* initialize software variables */
	

	/* set up each hardware interface */
	*TIMER1_START_LO = TIMER1_INTERVAL & 0xFFFF;
    *TIMER1_START_HI = (TIMER1_INTERVAL >> 16) & 0xFFFF;
   
    *TIMER2_START_LO = TIMER2_INTERVAL & 0xFFFF;
    *TIMER2_START_HI = (TIMER2_INTERVAL >> 16) & 0xFFFF;
   
	*TIMER1_CONTROL = 0x7; /* start timer, enable interrupts, continuous mode */
    *TIMER2_CONTROL = 0x7; /* start timer, enable interrupts, continuous mode */
	
	InitADC(0x2, 0x2);
	/* set up ienable */
	NIOS2_WRITE_IENABLE((1<<14) | (1<<15));

	/* enable global recognition of interrupts in procr. status reg. */
	NIOS2_WRITE_STATUS(1);
}

/*-----------------------------------------------------------------*/

int main (void)
{
	Init ();	/* perform software/hardware initialization */
	int leftmost = 0;
	
	unsigned int character = GetChar();
	if (character == 'l' || character == 'L'){
		leftmost = 1;
	}else{
		leftmost = 0;
	}
	
	PrintString("/nELEC 371 Lab 4 by Alex Morra, Isaiah Iruoha, Keven Li\n");
	PrintString("\nA/D result divided by 16: 0x?");
	
	while (1)
	{
		/* fill in body of infinite loop */
		if (timer_flag_2){
			unsigned int data = *SWITCHES_DATA;
			data = data & 0x7F;
			if (leftmost){
				data = data << 24;
			}
			*HEX_DISPLAYS = data;
			timer_flag_2 = 0;
		}
		
		if (timer_flag_1){
			unsigned int adc_value = ADConvert();
			adc_value = adc_value / 16;
			PrintChar('\b');
			PrintHexDigit(adc_value);
			timer_flag_1 = 0;
		}
	}

	return 0;	/* never reached, but main() must return a value */
}
