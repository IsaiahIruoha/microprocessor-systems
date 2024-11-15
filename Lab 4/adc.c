/*-------------------------------------------------------------------
  Simplified support code for performing analog-to-digital conversion
  using the Companion Board for the DE0 or DE0-CV

  N. Manjikian, ECE Dept., Queen's University, November 2018
  -------------------------------------------------------------------*/

/* prepare pointer symbols using appropriate memory-mapped I/O addresses */

#define JP1_DATA ((volatile unsigned int *) 0x10000060)
#define JP1_DIR  ((volatile unsigned int *) 0x10000064)

/*-------------------------------------------------------------------
  This func. sets up data direction reg. and data reg. for JP1 port
  -------------------------------------------------------------------*/

/* NOTE: assumes adc_sel is in the range 0..2 and mux_sel is in the range 0..3.
   More code could be introduced to ensure above ranges are respected. */

void	InitADC (int adc_sel, int mux_sel)
{
  unsigned int bits;

  /* for data direction, bits 31, 28..27, 26..25, 24, 23..22, and 8
     must be set for output direction; the remaining bits are default input */

  bits = (0x1 << 31) | (0x3 << 27) | (0x3 << 25) | (0x1 << 24)
       | (0x3 << 22) | (0x1 << 8);

  /* the compiler generates a constant at compile time for the right-hand side
     of the the statement above -- alternatively, set bits to 0x9FC00100 */


  *JP1_DIR = bits;  /* write to data direction register for parallel port */


  /* for the actual bit output values, build the pattern below */

  bits = 0;                  /* prepare all zero initial bits */
  bits |= (1 << 24);         /* set DAC_write_n to 1 */
  bits |= (1 << 8);          /* set ADC_read_n to 1 */
  bits |= (adc_sel << 25);   /* set 2-bit field to choose active chip */
  bits |= (mux_sel << 27);   /* set 2-bit field to choose among 4 channels */

  *JP1_DATA = bits;  /* write to data register for parallel port */
}


/*-------------------------------------------------------------------
  This func. performs conversion for MUX/ADC set with initialization;
  no input arguments; uses data reg. to find which ADC chip selected
  -------------------------------------------------------------------*/

unsigned int ADConvert (void)
{
  unsigned int bits, done_mask_value, result, adc_sel;
  int loop;

  bits = *JP1_DATA;	/* get current port data register contents */

  adc_sel = (bits >> 25) & 0x3;  /* which chip was selected ? */

  /* must set ADC_read_n to low and keep it low to perform conversion */

  bits &= ~(1 << 8);  /* force ADC_read_n bit to 0, preserving other bits */
  *JP1_DATA = bits;   /* write new pattern to data register */


  /* now wait until the active-low 'done' bit for selected chip is asserted */

  done_mask_value = (1 << adc_sel); /* 001, 010, or 100 for mask value */

  do
  {
    bits = (*JP1_DATA >> 11);       /* read data register, shift field down */
    bits = bits & done_mask_value;  /* isolate desired bit with mask */
  } while (bits != 0);              /* check if active-low bit is still high */

  /* when loop is exited, extract the digital value from the conversion */

  bits = *JP1_DATA;
  result = bits & 0xFF;  /* isolate the lowest 8 bits */

  /* final step is to set ADC_read_n back to 1 */

  bits |= (1 << 8);    /* bit was previously 0, so OR in 1 to that position */ 
  *JP1_DATA = bits;

  return result;   /* return the converted 8-bit unsigned value */
}

