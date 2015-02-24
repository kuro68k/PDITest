/*
 * target_fw.h
 *
 * Created: 08/10/2014 16:20:39
 *  Author: paul.qureshi
 */ 


#ifndef TARGET_FW_H_
#define TARGET_FW_H_


#define TARGET_FW_SIZE			32768
#define TARGET_FW_PAGE_SIZE		128
#define TARGET_FW_NUM_PAGES		256

#define TARGET_FW_FUSES			(uint8_t[]){0xFF, 0xAA, 0x41, 0xFF, 0x12, 0x25, 0xC0}
#define TARGET_FW_NUM_FUSES		7

#define TARGET_DEVICE_ID		XMEGA_DEVID_32E5


extern const __flash unsigned char _binary_target_fw_bin_start[];



#endif /* TARGET_FW_H_ */