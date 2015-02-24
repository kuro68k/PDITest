avr-objcopy -I ihex -O binary Mikron3SCU.hex target_fw.bin
xxd -i target_fw.bin target_fw.c
avr-objcopy -I binary -B avr:106 -O elf32-avr --rename-section .data=.progmem target_fw.bin target_fw.o