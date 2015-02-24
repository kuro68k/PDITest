/*
 * nvm.c
 *
 * Created: 07/10/2014 10:32:38
 *  Author: paul.qureshi
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include "terminal.h"
#include "pdi.h"
#include "target_fw.h"
#include "nvm.h"


#pragma region Utility Functions

/**************************************************************************************************
** Send a 32 bit address to the target
*/
void nvm_send_address(uint32_t address)
{
	// little endian
	PDI_send_byte(address & 0xFF);
	PDI_send_byte(address >> 8);
	PDI_send_byte(address >> 16);
	PDI_send_byte(address >> 24);
}

/**************************************************************************************************
** Select an NVM register to read/write
*/
void nvm_select_nvm_reg(uint8_t reg)
{
	uint32_t address = PDI_NVM_BASE_ADDR | reg;
	nvm_send_address(address);
}

/**************************************************************************************************
** Wait for the NVM controller to become idle. Can time out.
*/
bool nvm_wait_for_nvm(void)
{
	uint16_t	timeout = 0;
	
	do
	{
		uint8_t	status;

		PDI_send_byte(PDI_CMD_LDS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
		nvm_select_nvm_reg(NVM_REG_STATUS);
		status = PDI_receive_byte();
		if (!(status & NVM_NVMBUSY_bm))
			return(true);
		
		_delay_ms(1);
		timeout++;
	} while(timeout < PDI_NVM_TIMEOUT_MS);
	
	TERM_print_P(PSTR("nvm_wait_for_nvm: timeout\r\n"));
	return(false);
}

/**************************************************************************************************
** Wait for the NVM controller to become idle. Can time out.
*/
bool nvm_wait_for_pdi_status(void)
{
	uint16_t	timeout = 0;
	
	do
	{
		uint8_t	status;

		PDI_send_byte(PDI_CMD_LDCS(PDI_REG_STATUS));
		status = PDI_receive_byte();
		if (status & PDI_STATUS_NVM_bm)
			return(true);
		
		_delay_ms(1);
		timeout++;
	} while(timeout < PDI_NVM_TIMEOUT_MS);
	
	TERM_print_P(PSTR("nvm_wait_for_nvm: timeout\r\n"));
	return(false);
}

#pragma endregion

#pragma region Commands

/**************************************************************************************************
** Unlock the NVM controller and set micro to reset when PDI session ends
*/
bool NVM_unlock(void)
{
	PDI_send_byte(0xC0);
	PDI_send_byte(0xFD);
	PDI_send_byte(0x80);
	PDI_receive_byte();
	
	// set up the reset key in RESET PDI register
	PDI_send_byte(PDI_CMD_STCS(PDI_REG_RESET));
	PDI_send_byte(PDI_RESET_KEY);
	
	// enable access to NVM
	PDI_send_byte(PDI_CMD_KEY);
	for (uint8_t i = sizeof(PDI_NVM_ENABLE_KEY); i > 0; i--)
		PDI_send_byte(PDI_NVM_ENABLE_KEY[i - 1]);

	return(nvm_wait_for_pdi_status());
}

/**************************************************************************************************
** Read arbitrary memory from target into buffer. Max length = 256
*/
bool NVM_read_memory(uint32_t address, uint8_t *buffer, uint16_t length)
{
	if (length > 256)
		return(false);
	
	if (!nvm_wait_for_pdi_status())
		return(false);

	// set up READNVM command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CMD);
	PDI_send_byte(NVM_CMD_READNVM);
	
	if (length > 1)
	{
		// load the PDI pointer
		PDI_send_byte(PDI_CMD_ST(PDI_POINTER_DIRECT, PDI_DATASIZE_4BYTES));
		nvm_send_address(address);
		
		// send REPEAT command with number of bytes to read
		PDI_send_byte(PDI_CMD_REPEAT(PDI_DATASIZE_1BYTE));
		PDI_send_byte(length - 1);
		
		// read bytes with LD
		PDI_send_byte(PDI_CMD_LD(PDI_POINTER_INDIREC_PI, PDI_DATASIZE_1BYTE));
		while (length--)
			*buffer++ = PDI_receive_byte();
	}
	else
	{
		// LDS command to read requested byte
		PDI_send_byte(PDI_CMD_LDS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
		nvm_send_address(address);
		*buffer++ = PDI_receive_byte();
	}

	return(true);
}

/**************************************************************************************************
** Write to the target's flash memory. Length is maximum 256.
*/
bool NVM_write_flash(uint16_t page, const __flash uint8_t *buffer, uint16_t length)
{
#pragma region Erase Page Buffer

	if (!nvm_wait_for_pdi_status())
		return(false);

	// load command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CMD);
	PDI_send_byte(NVM_CMD_ERASEFLASHPAGEBUFF);

	// execute command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CTRLA);
	PDI_send_byte(NVM_BIT_CTRLA_CMDEX);

#pragma endregion
	
#pragma region Write Flash Page Buffer

	// load command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CMD);
	PDI_send_byte(NVM_CMD_LOADFLASHPAGEBUFF);
	
	// set PDI pointer
	PDI_send_byte(PDI_CMD_ST(PDI_POINTER_DIRECT, PDI_DATASIZE_4BYTES));
	nvm_send_address(XMEGA_ADDR_FLASH_BASE + (page * TARGET_FW_PAGE_SIZE));	// start of page
	
	// set up REPEAT count
	PDI_send_byte(PDI_CMD_REPEAT(PDI_DATASIZE_1BYTE));
	PDI_send_byte(length - 1);
	
	// Repeat ST commands to load data
	PDI_send_byte(PDI_CMD_ST(PDI_POINTER_INDIREC_PI, PDI_DATASIZE_1BYTE));
	//uint8_t test = 0;
	while (length--)
		PDI_send_byte(*buffer++);
		//PDI_send_byte(test++);

#pragma endregion

#pragma region Commit Buffer to Flash Memory

	// load command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CMD);
	PDI_send_byte(NVM_CMD_WRITEFLASHPAGE);
	//PDI_send_byte(NVM_CMD_WRITEAPPSECPAGE);
	
	// send page address
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_send_address(XMEGA_ADDR_FLASH_BASE + (page * TARGET_FW_PAGE_SIZE));
	//nvm_send_address(XMEGA_ADDR_FLASH_BASE);
	PDI_send_byte(0xFF);		// starts the page write

	if (!nvm_wait_for_nvm())
		return(false);

#pragma endregion

	return(true);
}

/**************************************************************************************************
** Erase target
*/
bool NVM_chip_erase(void)
{
	if (!nvm_wait_for_pdi_status())
		return(false);

	// set up chip erase command in NVM controller
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CMD);
	PDI_send_byte(NVM_CMD_CHIPERASE);
	
	// execute command
	PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
	nvm_select_nvm_reg(NVM_REG_CTRLA);
	PDI_send_byte(NVM_BIT_CTRLA_CMDEX);

	// wait for command to complete
	if (!nvm_wait_for_nvm())
		return(false);

	return(true);
}

#pragma endregion

#pragma region Programming

/**************************************************************************************************
** Write firmware image to target
*/
bool nvm_write_firmware_image(void)
{
	uint16_t	page = 0;
	uint16_t	bytes_left = TARGET_FW_SIZE;
						
	while (bytes_left)
	{
		if (!NVM_write_flash(page, &_binary_target_fw_bin_start[page * TARGET_FW_PAGE_SIZE], TARGET_FW_PAGE_SIZE))
			return(false);
							
		bytes_left -= TARGET_FW_PAGE_SIZE;
		page++;
	}
	
	return(true);
}

/**************************************************************************************************
** Set fuses on target
*/
bool nvm_set_fuses(void)
{
	uint8_t i;
	
	for (i = 0; i < TARGET_FW_NUM_FUSES; i++)
	{
		// set up NVM command
		PDI_send_byte(PDI_CMD_STS(PDI_DATASIZE_4BYTES, PDI_DATASIZE_1BYTE));
		nvm_select_nvm_reg(NVM_REG_CMD);
		PDI_send_byte(NVM_CMD_WRITEFUSE);

		// set pointer
		PDI_send_byte(PDI_CMD_ST(PDI_POINTER_DIRECT, PDI_DATASIZE_4BYTES));
		nvm_send_address(XMEGA_ADDR_FUSE_BASE + i);
		
		// send fuse byte
		PDI_send_byte(PDI_CMD_ST(PDI_POINTER_INDIREC_PI, PDI_DATASIZE_1BYTE));
		PDI_send_byte(TARGET_FW_FUSES[i]);
		
		// wait for programming to complete
		if (!nvm_wait_for_nvm())
			return(false);
	}
	
	return(true);
}

/**************************************************************************************************
** Compare the target's firmware to the stored firmware image, and check fuses are set
*/
bool nvm_verify_firmware(void)
{
	const __flash uint8_t *ptr = _binary_target_fw_bin_start;
	uint8_t		buffer[TARGET_FW_PAGE_SIZE];
	uint16_t	page = 0;
	uint16_t	bytes_left = TARGET_FW_SIZE;
							
	// check firmware image
	while (bytes_left)
	{
		if (!NVM_read_memory(XMEGA_ADDR_FLASH_BASE + (page * TARGET_FW_PAGE_SIZE), buffer, TARGET_FW_PAGE_SIZE))
		{
			TERM_print_P(PSTR("read failure\r\n"));
			return(false);
		}
								
		for (uint16_t j = 0; j < TARGET_FW_PAGE_SIZE; j++)
		{
			if (buffer[j] != *ptr++)
			{
				TERM_print_P(PSTR("firmware verification failure\r\n"));
				return(false);
			}
		}

		bytes_left -= TARGET_FW_PAGE_SIZE;
		page++;
	}


	// check fuses
	uint8_t fuses[TARGET_FW_NUM_FUSES];
	if (!NVM_read_memory(XMEGA_ADDR_FUSE_BASE, fuses, TARGET_FW_NUM_FUSES))
	{
		TERM_print_P(PSTR("read failure\r\n"));
		return(false);
	}
	for (uint8_t i = 0; i < TARGET_FW_NUM_FUSES; i++)
	{
		if (fuses[i] != TARGET_FW_FUSES[i])
		{
			TERM_print_P(PSTR("fuse verification failure\r\n"));
			return(false);
		}
	}

	return(true);
}

/**************************************************************************************************
** Program target with firmware and fuses. Firmware details in target_fw.h.
*/
bool NVM_program_target(void)
{
	__label__ fail;
	
	// start PDI interface
	if (!PDI_wake())
	{
		TERM_print_P(PSTR("Unable to communicate with target.\r\n"));
		goto fail;
	}


	// unlock NVM controller
	if (!NVM_unlock())
	{
		TERM_print_P(PSTR("Unable to unlock NVM controller.\r\n"));
		goto fail;
	}
	TERM_print_P(PSTR("NVM controller unlocked.\r\n"));
			
		
	// check for correct target device
	uint32_t device_id;
	if (!NVM_read_memory(XMEGA_ADDR_DATAMEM_BASE + XMEGA_SFR_DEVID0, (uint8_t *)&device_id, 4))
	{
		TERM_print_P(PSTR("NVM read failed\r\n"));
		goto fail;
	}
	TERM_printf_P(PSTR("Device ID: 0x%02lX%02lX%02lX\t"), device_id & 0xFF, (device_id >> 8) & 0xFF, (device_id >> 16) & 0xFF);
	if ((device_id & XMEGA_DEVID_MASK) != TARGET_DEVICE_ID)
	{
		TERM_print_P(PSTR("failed\r\n"));
		goto fail;
	}
	TERM_print_P(PSTR("OK\r\n"));
				

	// erase target
	TERM_print_P(PSTR("Erasing target...\t"));
	if (!NVM_chip_erase())
	{
		TERM_print_P(PSTR("failed\r\n"));
		goto fail;
	}
	TERM_print_P(PSTR("OK\r\n"));



	// write firmware
	TERM_print_P(PSTR("Writing firmware...\t"));
	if (!nvm_write_firmware_image())
	{
		TERM_print_P(PSTR("failed\r\n"));
		goto fail;
	}
	TERM_print_P(PSTR("OK\r\n"));

	// set fuses
	TERM_print_P(PSTR("Setting fuses...\t"));
	if (!nvm_set_fuses())
	{
		TERM_print_P(PSTR("failed\r\n"));
		goto fail;
	}
	TERM_print_P(PSTR("OK\r\n"));

	

	// check firmware
	TERM_print_P(PSTR("Checking firmware...\t"));
	if (!nvm_verify_firmware())
		goto fail;
	TERM_print_P(PSTR("OK\r\n"));

	
	PDI_sleep();
	return(true);

fail:
	PDI_sleep();
	return(false);
}

#pragma endregion
