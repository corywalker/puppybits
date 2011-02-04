
CC=arm-none-eabi-gcc

all: start.o start-libc.o start-kernel.o

start.o: start.S
start-libc.o: start-libc.S
start-kernel.o: start-kernel.S

clean:
	-rm *.o