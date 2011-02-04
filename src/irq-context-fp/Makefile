
TOP=../..

SRCS=irq-context-fp.c
ASM=context.S
OBJS=$(SRCS:.c=.o) $(ASM:.S=.o)

IMAGE_ELF=irq-context-fp

include $(TOP)/make-alone.cfg

# disable optimisations so the contrived test routines aren't optimised away
CFLAGS += -O0
