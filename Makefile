#Compiler

CC=arm-none-eabi-gcc
LD=arm-none-eabi-g++

#Include files
INC = -Iinclude

#Compiler flags
CFLAGS = $(INC) -Wall -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g3 -O0

#Linker flags
LFLAGS = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -specs=nosys.specs -T STM32F103C8Tx_FLASH.ld -Wl,--gc-sections -lm


#Linking
all: adcdma.o addons.o usart.o main.o startup_stm32f103xb.o 
	$(LD) $(LFLAGS) output/adcdma.o output/addons.o output/usart.o output/main.o output/startup_stm32f103xb.o -o output/program.elf


#Compiling main.c
main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o output/main.o

#Compiling adcdma.c
adcdma.o: src/adcdma.c
	$(CC) $(CFLAGS) -c src/adcdma.c -o output/adcdma.o

#Compiling usart.c
usart.o: src/usart.c
	$(CC) $(CFLAGS) -c src/usart.c -o output/usart.o

#Compiling addons.c
addons.o: src/addons.c
	$(CC) $(CFLAGS) -c src/addons.c -o output/addons.o

#Compiling startup_stm32f103xb.s
startup_stm32f103xb.o: src/startup_stm32f103xb.s
	$(CC) $(CFLAGS) -c src/startup_stm32f103xb.s -o output/startup_stm32f103xb.o

#Cleaning
clean:
	rm -f output/program.elf output/*.o
