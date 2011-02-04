
#include <stdint.h>

#define UART_LSR_TX_FIFO_E 0x20
#define UART_LSR_RX_FIFO_E 0x01

#define UART_THR_REG (0x00/4)
#define UART_LSR_REG (0x14/4)

extern volatile uint32_t UART3[1024];

void dputc(int c) {
	while ((UART3[UART_LSR_REG] & UART_LSR_TX_FIFO_E) == 0)
		;
	UART3[UART_THR_REG] = c;
}

void dputs(const char *s) {
	char c;

	while ((c = *s++)) {
		if (c == 10)
			dputc(13);
		dputc(c);
	}
}

static const char *tohex = "0123456789ABCDEF";

void dputx(unsigned int v) {
	int i;

	for (i=7;i>=0;i--) {
		dputc(tohex[(v>>(i*4))&0x0f]);
	}
}

// TODO: parse kernel args or something
int main() {
	dputs("This should be running at > 0xC0000000\n");
	dputs("\n main = ");
	dputx((unsigned int)main);
	dputs("\n UART3 = ");
	dputx((unsigned int)UART3);
	//dputs("\n SP = ");
	//register unsigned int sp asm("r13");
	//dputx(sp);
	dputs("\n\nHalted.");
	while (1)
		;
}
