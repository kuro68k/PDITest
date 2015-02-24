/*
 * hw.h
 *
 * Created: 17/10/2013 15:22:49
 *  Author: paul.qureshi
 */ 


#ifndef HW_H_
#define HW_H_



#pragma region Macros
/**************************************************************************************************
** Macros
*/

// port settings
#define DISABLE_DIGITAL_INPUT(A)	A = (A & ~PORT_ISC_gm) | PORT_ISC_INPUT_DISABLE_gc
#define ENABLE_PULLUP(A)			A = (A & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc
#define ENABLE_PULLDOWN(A)			A = (A & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc
#define ENABLE_TOTEM(A)				A = (A & ~PORT_OPC_gm) | PORT_OPC_TOTEM_gc

#pragma endregion


/**************************************************************************************************
** Public variables and functions
*/


extern void		HW_init(void);
extern void		HW_CCPWrite(volatile uint8_t *address, uint8_t value);



#endif /* HW_H_ */