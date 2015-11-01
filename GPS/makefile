TARGET = src/driver.c

OUTPUT_OBJECT = obj/program_1.o

C_INCLUDES = src/usb_serial.c src/UART.c src/gps.c

OUTPUT_HEX = program.hex


make:	$(OBJ_FILES)
	@avr-gcc -mmcu=atmega32u4 -Os -DF_CPU=16000000UL $(TARGET) $(C_INCLUDES) -I include/ -Wl,-u,vfprintf -lprintf_flt -lm -o $(OUTPUT_OBJECT)
	@avr-objcopy -O ihex $(OUTPUT_OBJECT) $(OUTPUT_HEX)
	@echo "complete"

clean:
	rm $(OUTPUT_OBJECT)
	rm $(OUTPUT_HEX)