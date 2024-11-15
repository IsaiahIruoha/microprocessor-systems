#ifndef _ADC_H_
#define _ADC_H_

/*-------------------------------------------------------------------
  Simplified support code for performing analog-to-digital conversion
  using the Companion Board for the DE0 or DE0-CV

  N. Manjikian, ECE Dept., Queen's University, November 2018
  -------------------------------------------------------------------*/

/* function prototypes for preparing then performing conversion */

extern void         InitADC (int adc_sel, int mux_sel);

extern unsigned int ADConvert (void);

#endif /* _ADC_H_ */
