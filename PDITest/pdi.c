/*
 * pdi.c
 *
 * Created: 06/10/2014 15:10:11
 *  Author: paul.qureshi
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stddef.h>
#include <stdbool.h>
#include "terminal.h"
#include "pdi.h"

#define NOP()		asm("NOP");

bool	pdi_tx_mode = false;

/**************************************************************************************************
** Enable PDI interface and put target into PDI programming mode
*/
bool PDI_wake(void)
{
	int		bsel = 1;		// 10,000 @ 2MHz
	uint8_t	bscale = 2;

	PDI_PORT.OUTSET = PDI_XCLK_PIN_bm | PDI_TX_PIN_bm;
	PDI_PORT.DIRSET = PDI_XCLK_PIN_bm | PDI_TX_PIN_bm;
	PDI_PORT.DIRCLR = PDI_RX_PIN_bm;
	
	// hold data line high for at least 90ns
	NOP();NOP();

	// Set up USART in synchronous mode
	PDI_USART.CTRLA = 0;
	PDI_USART.BAUDCTRLA = (uint8_t) bsel;
	PDI_USART.BAUDCTRLB = (bscale << 4) | (bsel >> 8);
	PDI_USART.CTRLB = USART_TXEN_bm;
	PDI_USART.CTRLC = USART_CMODE_SYNCHRONOUS_gc | USART_PMODE_EVEN_gc |
						USART_SBMODE_bm | USART_CHSIZE_8BIT_gc;
	
	pdi_tx_mode = true;
	PDI_send_break();
	PDI_send_break();

	// try to set timeout
	PDI_send_byte(PDI_CMD_STCS(PDI_REG_CTRL));
	PDI_send_byte(0x03);
	PDI_send_byte(PDI_CMD_LDCS(PDI_REG_CTRL));
	uint8_t temp = PDI_receive_byte();
	if (temp != 0x03)
		return(false);

	// set up the reset key in RESET.PDI register
	PDI_send_byte(PDI_CMD_STCS(PDI_REG_RESET));
	PDI_send_byte(PDI_RESET_KEY);

	return(true);
}

/**************************************************************************************************
** Disable PDI interface
*/
void PDI_sleep(void)
{
	// clear the PDI.RESET register so that device resets when PDI clock stops
	PDI_send_byte(PDI_CMD_STCS(PDI_REG_RESET));
	PDI_send_byte(0x00);

	PDI_USART.CTRLB = 0;
}

/**************************************************************************************************
** Send a byte to PDI target
*/
void PDI_send_byte(uint8_t byte)
{
	if (!pdi_tx_mode)
	{
		while (PDI_PORT.IN & PDI_XCLK_PIN_bm)
			;
		while (!(PDI_PORT.IN & PDI_XCLK_PIN_bm))
			;
		while (PDI_PORT.IN & PDI_XCLK_PIN_bm)
			;

		PDI_PORT.DIRSET = PDI_TX_PIN_bm;
		PDI_USART.CTRLB = USART_TXEN_bm;				// switch to TX mode if required
		pdi_tx_mode = true;
	}

	while (!(PDI_USART.STATUS & USART_DREIF_bm))
		;
	PDI_USART.DATA = byte;
	PDI_USART.STATUS = USART_TXCIF_bm;
}

/**************************************************************************************************
** Recieve a byte from PDI target
*/
uint8_t PDI_receive_byte(void)
{
	// switch to RX mode if required
	if (pdi_tx_mode)
	{
		while (!(PDI_USART.STATUS & USART_TXCIF_bm))	// wait for buffer to clear
			;
		PDI_PORT.DIRCLR = PDI_TX_PIN_bm;
		PDI_USART.CTRLB = USART_RXEN_bm;
		PDI_USART.STATUS = USART_RXCIF_bm;
		pdi_tx_mode = false;
	}
	
	while (!(PDI_USART.STATUS & USART_RXCIF_bm))
		;
	return(PDI_USART.DATA);
}

/**************************************************************************************************
** Send a PDI break, consisting of 12 clock cycles with no data.
*/
void PDI_send_break(void)
{
	uint8_t	i;
	
	//if (!(PDI_USART.CTRLB & USART_TXEN_bm))		// make sure we are in TX mode
	PDI_USART.CTRLB = USART_TXEN_bm;
	PDI_PORT.DIRSET = PDI_TX_PIN_bm;
	
	// wait for one full frame
	for (i = 0; i < PDI_BITS_IN_FRAME; i++)
	{
		while (PDI_PORT.IN & PDI_XCLK_PIN_bm)
			;
		while (!(PDI_PORT.IN & PDI_XCLK_PIN_bm))
			;
		while (PDI_PORT.IN & PDI_XCLK_PIN_bm)
			;
	}
}

