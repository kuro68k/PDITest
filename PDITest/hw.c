/*
 * hw.c
 *
 * Created: 17/10/2013 15:22:58
 *  Author: paul.qureshi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <string.h>

#include "hw.h"


#pragma region Low Level

/**************************************************************************************************
** Set up hardware after reset
*/
void HW_init(void)
{
	// Set 16MHz clock
	OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;
	while (!(OSC.STATUS & OSC_RC32MRDY_bm))
		;
	while (!(OSC.STATUS & OSC_RC32KRDY_bm))
		;
	HW_CCPWrite(&CLK.CTRL, CLK_SCLKSEL_RC32M_gc);
	HW_CCPWrite(&CLK.PSCTRL, CLK_PSADIV_2_gc | CLK_PSBCDIV_1_1_gc);
	OSC.CTRL &= (OSC_RC32MEN_bm | OSC_RC32KEN_bm);
}

/**************************************************************************************************
** Write a CCP protected register. Registers protected by CCP require the CCP register to be written
** first, followed by writing the protected register within 4 instruction cycles.
**
** Interrupts are temporarily disabled during the write. Code mostly adapted/stolen from Atmel's
** clksys_driver.c and avr_compiler.h.
*/
inline void HW_CCPWrite(volatile uint8_t *address, uint8_t value)
{
        uint8_t	saved_sreg;

        // disable interrupts if running
		saved_sreg = SREG;
		cli();
		
		volatile uint8_t * tmpAddr = address;
        //RAMPZ = 0;

        asm volatile(
                "movw r30,  %0"       "\n\t"
                "ldi  r16,  %2"       "\n\t"
                "out   %3, r16"       "\n\t"
                "st     Z,  %1"       "\n\t"
                :
                : "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
                : "r16", "r30", "r31"
                );

        SREG = saved_sreg;
}
