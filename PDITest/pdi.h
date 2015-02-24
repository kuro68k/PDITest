/*
 * pdi.h
 *
 * Created: 06/10/2014 15:10:19
 *  Author: paul.qureshi
 */ 


#ifndef PDI_H_
#define PDI_H_


/**************************************************************************************************
** Hardware
*/
#define PDI_PORT			PORTC
#define PDI_XCLK_PIN_bm		PIN1_bm
#define PDI_RX_PIN_bm		PIN2_bm
#define PDI_TX_PIN_bm		PIN3_bm

#define PDI_USART			USARTC0



/**************************************************************************************************
** PDI protocol
*/
#define PDI_BITS_IN_FRAME		12

#define PDI_NVM_TIMEOUT_MS		200

#define PDI_NVM_BASE_ADDR		0x010001C0

#define PDI_CMD_LDS(AddressSize, DataSize)		(0x00 | (AddressSize << 2) | DataSize)
#define PDI_CMD_LD(PointerAccess, DataSize)		(0x20 | (PointerAccess << 2) | DataSize)
#define PDI_CMD_STS(AddressSize, DataSize)		(0x40 | (AddressSize << 2) | DataSize)
#define PDI_CMD_ST(PointerAccess, DataSize)		(0x60 | (PointerAccess << 2) | DataSize)
#define PDI_CMD_LDCS(PDIReg)					(0x80 | PDIReg)
#define PDI_CMD_REPEAT(DataSize)				(0xA0 | DataSize)
#define PDI_CMD_STCS(PDIReg)					(0xC0 | PDIReg)
#define PDI_CMD_KEY								0xE0

#define PDI_REG_STATUS			0
#define PDI_REG_RESET			1
#define PDI_REG_CTRL			2

#define PDI_STATUS_NVM_bm		(1<<1)
#define PDI_RESET_KEY			0x59

#define PDI_NVM_ENABLE_KEY		(uint8_t[]){0x12, 0x89, 0xAB, 0x45, 0xCD, 0xD8, 0x88, 0xFF}

#define PDI_POINTER_INDRECT		0
#define PDI_POINTER_INDIREC_PI	1
#define PDI_POINTER_DIRECT		2

#define PDI_DATASIZE_1BYTE		0
#define PDI_DATASIZE_2BYTES		1
#define PDI_DATASIZE_3BYTES		2
#define PDI_DATASIZE_4BYTES		3


/**************************************************************************************************
** External functions
*/
extern bool		PDI_wake(void);
extern void		PDI_sleep(void);
extern void		PDI_send_byte(uint8_t byte);
extern void		PDI_send_break(void);
extern uint8_t	PDI_receive_byte(void);




#endif /* PDI_H_ */