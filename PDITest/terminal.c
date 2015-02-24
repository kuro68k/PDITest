/*
 * terminal.c
 *
 * Created: 18/10/2013 15:24:08
 *  Author: paul.qureshi
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stddef.h>

#include "terminal.h"


char	term_tx_buffer[255];


/**************************************************************************************************
** Power up the USART and prepare terminal
*/
void TERM_wake(void)
{
	int		bsel = 3301;		// 19200 @ 16MHz
	uint8_t	bscale = -5;

	// USART
	TERM_PORT.DIRCLR = PIN2_bm;
	TERM_PORT.DIRSET = PIN3_bm;
	PR.PRPD	&= ~PR_USART0_bm;
	TERM_USART.CTRLA = 0;
	TERM_USART.CTRLB = 0;
	TERM_USART.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	TERM_USART.BAUDCTRLA = (uint8_t) bsel;
	TERM_USART.BAUDCTRLB = (bscale << 4) | (bsel >> 8);
	TERM_USART.DATA;	// clear RX FIFO
	TERM_USART.CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;
	TERM_USART.CTRLA = USART_RXCINTLVL_LO_gc;
}

/**************************************************************************************************
** Send single char. Note that this function returns as soon as the character is in the FIFO and it
** will take some time to be transmitted.
*/
void TERM_tx_char(char c)
{
	while ((TERM_USART.STATUS & USART_DREIF_bm) == 0)	// wait for a free FIFO slot
		;
	TERM_USART.DATA = c;
}

/**************************************************************************************************
** Power up the USART and prepare terminal
*/
void TERM_print_P(const __flash char *string)
{
	while (*string != '\0')
		TERM_tx_char(*string++);
}

/**************************************************************************************************
** Send null terminated string of chars from SRAM. Returns as soon as the last character is in
** the FIFO. Does not send the terminating null.
*/
void TERM_print(const char *string)
{
	while (*string != '\0')
		TERM_tx_char(*string++);
}

/**************************************************************************************************
** Wrapper for printf() that outputs to the terminal
*/
void TERM_printf_P(PGM_P format, ...)
{
	va_list args;
	va_start(args, format);
	vsprintf_P(term_tx_buffer, format, args);
	va_end(args);
	TERM_print(term_tx_buffer);
}
