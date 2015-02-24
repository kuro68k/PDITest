/*
 * PDITest.c
 *
 * Created: 06/10/2014 14:20:27
 *  Author: paul.qureshi
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stddef.h>
#include <stdbool.h>
#include "terminal.h"
#include "pdi.h"
#include "nvm.h"
#include "hw.h"
#include "target_fw.h"

const __flash uint8_t test_pattern[] = {0x01, 0x02, 0x03, 0x04, 0xCA, 0xFE, 0xBE, 0xEF, 0x88, 0x91, 0x42};

int main(void)
{
	HW_init();
	TERM_wake();
	
	TERM_print_P(PSTR("\r\ntest\r\n"));
	NVM_program_target();
	
	for(;;);
}