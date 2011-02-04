
TOP=../..

SRCS=ipc-signal.c
ASM=context.S
OBJS=$(SRCS:.c=.o) $(ASM:.S=.o)

IMAGE_ELF=ipc-signal

include $(TOP)/make-alone.cfg

# disable optimisations so the contrived test routines aren't optimised away
#CFLAGS += -O0
