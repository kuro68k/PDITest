/*
 * terminal.h
 *
 * Created: 18/10/2013 15:23:55
 *  Author: paul.qureshi
 */ 


#ifndef TERMINAL_H_
#define TERMINAL_H_



/**************************************************************************************************
** USART
*/

#define TERM_PORT		PORTD
#define TERM_USART		USARTD0



/**************************************************************************************************
** Public functions and variables
*/

extern void TERM_wake(void);

extern void TERM_tx_char(char c);
extern void TERM_print(const char *string);
extern void TERM_print_P(const __flash char *string);
extern void TERM_printf_P(PGM_P format, ...);



#endif /* TERMINAL_H_ */