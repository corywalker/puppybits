
TOP=..

include $(TOP)/make.cfg

SRCS=video.c interrupts.c debug.c debugmon.c graphics.c font-misc-fixed.c \
	rand.c ksleep.c \
	lists.c avl.c \
	neon.c \
	mmu-simple.c \
	bus_i2c.c \
	pmon.c \
	gpio.c
ASM=exceptions.S fill-565.S
OBJS=$(SRCS:.c=.o) $(ASM:.S=.o)

all:: .depend libomap.a

libomap.a: $(OBJS)
	$(AR) r $@ $^

clean::
	-rm *.o
	-rm *.a

include $(TOP)/make.dep
