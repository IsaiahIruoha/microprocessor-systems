/* for standalone testing of this file by itself using the simulator,
   keep the following line, but for in-lab activity with the Monitor Program
   to have a multi-file project, comment out the following line */

#define TEST_CHARIO


/* no #include statements should be required, as the character I/O functions
   do not rely on any other code or definitions (the .h file for these
   functions would be included in _other_ .c files) */

/* because all character-I/O code is in this file, the #define statements
   for the JTAG UART pointers can be placed here; they should not be needed
   in any other file */

#define JTAG_UART_DATA (*(volatile unsigned int *)0x10001000)
#define JTAG_UART_STATUS (*(volatile unsigned int *)0x10001004)

/* place the full function definitions for the character-I/O routines here */

unsigned int GetChar(void) {
    unsigned int info, st, data;

    do {
        // Read JTAG UART data register
        info = JTAG_UART_DATA;

        // Check if a character is available by masking the status bit (bit 15)
        st = info & 0x8000;  // Equivalent to (1 << 15)
    } while (st == 0);  // Wait until a character is available

    // Extract the character (lower 8 bits)
    data = info & 0xFF;
    return data;
}

void PrintChar(int ch) {
    unsigned int st;

    do {
        // Read JTAG UART status register
        st = JTAG_UART_STATUS;

        // Mask out irrelevant bits
        st = st & 0xFFFF0000;
    } while (st == 0);

    // Write character to JTAG UART data register
    JTAG_UART_DATA = (unsigned int)ch;
}

void PrintString(char *str) {
    char ch;
    
    for (;;) {
        // Get the current character
        ch = *str;

        // Break if end of string
        if (ch == '\0') 
            break;

        // Print the character
        PrintChar(ch);

        // Move to the next character
        str = str + 1;
    }
}

void PrintHexDigit(unsigned int n) {
    unsigned int ch;

    if (n < 10) {
        ch = n + '0';  // Convert to ASCII for '0' - '9'
    } else {
        ch = (n - 10) + 'A';  // Convert to ASCII for 'A' - 'F'
    }

    PrintChar(ch);
}

#ifdef TEST_CHARIO

/* this portion is conditionally compiled based on whether or not
   the symbol exists; it is only for standalone testing of the routines
   using the simulator; there is a main() routine in lab4.c, so
   for the in-lab activity, the following code would conflict with it */

int main (void)
{
   /* place calls here to the various character-I/O routines
       to test their behavior */
   /* Testing the various character-I/O routines */
   PrintString("Hello, world!\n");  // Test PrintString
   PrintChar('A');                  // Test PrintChar
   PrintHexDigit(0xA);              // Test PrintHexDigit with hex value 'A' (should print 'A')
   Printf(GetChar());  // Test GetChar (should return 'A' if 'A' was sent to the JTAG UART)
   return 0;
} 

#endif /* TEST_CHARIO */
