/*
 * nvm.h
 *
 * Created: 07/10/2014 10:32:50
 *  Author: paul.qureshi
 */ 


#ifndef NVM_H_
#define NVM_H_


/**************************************************************************************************
** NVM controller
*/

#define NVM_CRC_LENGTH_BYTES			3

#define	NVM_REG_ADDR0					0x00
#define	NVM_REG_ADDR1					0x01
#define	NVM_REG_ADDR2					0x02
#define	NVM_REG_DAT0					0x04
#define	NVM_REG_DAT1					0x05
#define	NVM_REG_DAT2					0x06
#define	NVM_REG_CMD						0x0A
#define	NVM_REG_CTRLA					0x0B
#define	NVM_REG_CTRLB					0x0C
#define	NVM_REG_INTCTRL					0x0D
#define	NVM_REG_STATUS					0x0F
#define	NVM_REG_LOCKBITS				0x10

#define	NVM_BIT_CTRLA_CMDEX				(1<<0)

#define	NVM_CMD_NOOP					0x00
#define	NVM_CMD_CHIPERASE				0x40
#define	NVM_CMD_READNVM					0x43
#define	NVM_CMD_LOADFLASHPAGEBUFF		0x23
#define	NVM_CMD_ERASEFLASHPAGEBUFF		0x26
#define	NVM_CMD_ERASEFLASHPAGE			0x2B
#define	NVM_CMD_WRITEFLASHPAGE			0x2E
#define	NVM_CMD_ERASEWRITEFLASH			0x2F
#define	NVM_CMD_FLASHCRC				0x78
#define	NVM_CMD_ERASEAPPSEC				0x20
#define	NVM_CMD_ERASEAPPSECPAGE			0x22
#define	NVM_CMD_WRITEAPPSECPAGE			0x24
#define	NVM_CMD_ERASEWRITEAPPSECPAGE	0x25
#define	NVM_CMD_APPCRC					0x38
#define	NVM_CMD_ERASEBOOTSEC			0x68
#define	NVM_CMD_ERASEBOOTSECPAGE		0x2A
#define	NVM_CMD_WRITEBOOTSECPAGE		0x2C
#define	NVM_CMD_ERASEWRITEBOOTSECPAGE	0x2D
#define	NVM_CMD_BOOTCRC					0x39
#define	NVM_CMD_READUSERSIG				0x03
#define	NVM_CMD_ERASEUSERSIG			0x18
#define	NVM_CMD_WRITEUSERSIG			0x1A
#define	NVM_CMD_READCALIBRATION			0x02
#define	NVM_CMD_READFUSE				0x07
#define	NVM_CMD_WRITEFUSE				0x4C
#define	NVM_CMD_WRITELOCK				0x08
#define	NVM_CMD_LOADEEPROMPAGEBUFF		0x33
#define	NVM_CMD_ERASEEEPROMPAGEBUFF		0x36
#define	NVM_CMD_ERASEEEPROM				0x30
#define	NVM_CMD_ERASEEEPROMPAGE			0x32
#define	NVM_CMD_WRITEEEPROMPAGE			0x34
#define	NVM_CMD_ERASEWRITEEEPROMPAGE	0x35
#define	NVM_CMD_READEEPROM				0x06


/**************************************************************************************************
** XMEGA memory map
*/

#define XMEGA_ADDR_FLASH_BASE			0x00800000UL
#define XMEGA_ADDR_EEPROM_BASE			0x008C0000
#define XMEGA_ADDR_FUSE_BASE			0x008F0020
#define XMEGA_ADDR_DATAMEM_BASE			0x01000000

#define XMEGA_APP_BASE					XMEGA_ADDR_FLASH_BASE
#define XMEGA_PROD_SIG_BASE				0x008E0200
#define XMEGA_USER_SIG_BASE				0x008E0300

// SFRs, starting at XMEGA_ADDR_DATAMEM_BASE
#define XMEGA_SFR_DEVID0				0x0090
#define XMEGA_SFR_DEVID1				0x0091
#define XMEGA_SFR_DEVID2				0x0092
#define XMEGA_SFR_REVID					0x0093


/**************************************************************************************************
** Target IDs
*/

#define XMEGA_DEVID_MASK				0x00FFFFFF
#define XMEGA_DEVID_32E5				0x004C951E



extern bool NVM_unlock(void);
extern bool NVM_read_memory(uint32_t address, uint8_t *buffer, uint16_t length);
extern bool NVM_write_flash(uint16_t page, const __flash uint8_t *buffer, uint16_t length);
extern bool NVM_chip_erase(void);
extern bool NVM_program_target(void);



#endif /* NVM_H_ */