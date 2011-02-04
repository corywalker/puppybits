/*  debug.c: Debugging helper functions.  clib-less i/o

    Copyright (C) 2010 Michael Zucchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Debug helper routines.

  Some basic i/o stuff.
*/

//#define TEST

#include <stdarg.h>

#define UART1 (0x4806A000)

#define UART_THR_REG (0)
#define UART_LSR_REG (0x14)

#define UART_SYSC_REG (0x54)

//#define ser_out (0x4806a000)
//#define ser_lsr (0x4806a014)
//#define ser_sysc (0x4806a054)
#define ser_out (0x49020000)
#define ser_lsr (0x49020014)
#define ser_sysc (0x49020054)

#define UART_LSR_TX_FIFO_E 0x20
#define UART_LSR_RX_FIFO_E 0x01

void dputc(int c) {
#ifndef TEST
	// wait for transmit fifo empty
	while ((((volatile unsigned int *)ser_lsr)[0] & UART_LSR_TX_FIFO_E) == 0)
		;
	((volatile unsigned char *)ser_out)[0] = c;
#else
	putchar(c);
#endif
}

int dgetc(void) {
	while ((((volatile unsigned int *)ser_lsr)[0] & UART_LSR_RX_FIFO_E) == 0)
		;
	return ((volatile unsigned char *)ser_out)[0];
}

// format an unsigned number to a given base from end-backwards, returns start
static char *ufmt(unsigned int v, unsigned int base, char *end) {
	unsigned int r, q = v;
	char *p = end;

	do {
		v = q;
		r = q % base;
		q = q / base;
		*--p = r >= 10 ? r - 10 + 'a' : r + '0';
	} while (q);

	return p;
}

static void nprint(char *start, char *end, int width, int pad) {
	width -= (end - start);
	while (width-- > 0)
		dputc(pad);
	while (start<end)
		dputc(*start++);	
}

// print unsigned with width and padding
static void uprint(unsigned int v, unsigned int base, unsigned int width, int pad) {
	char buffer[32], *e = buffer+32, *p;

	p = ufmt(v, base, e);
	nprint(p, e, width, pad);
}

// print signed with width and padding
static void iprint(int v, unsigned int base, unsigned int width, int pad) {
	char buffer[33], *e = buffer+32, *p;
	unsigned int u;

	u = v > 0 ? v : -v;
	p = ufmt(u, base, e);
	if (v < 0) {
		if (pad == '0') {
			dputc('-');
			width-=1;
		} else {
			*--p = '-';
		}
	}
	nprint(p, e, width, pad);
}

static int dstrlen(const char *s) {
	const char *p = s;

	while (*p)
		p++;
	return p-s;
}

void dputs(const char *s) {
	char c;

	while ((c = *s++)) {
		if (c == 10)
			dputc(13);
		dputc(c);
	}
}

/* reads a line of interactive input */
int dreadline(char *buffer, int len) {
	char c;
	char *p = buffer;

	while (p < buffer+len-1) {
		c = dgetc();
		if (c == 13 || c == 10)
			break;
		else if (c == 8 && p > buffer)
			p--;
		else {
			*p++ = c;
		}
		dputc(c);
	}
	dputc(13);
	dputc(10);
	*p = 0;

	return p-buffer;
}

void dprintf(const char *fmt, ...) {
	const char *p = fmt;
	char c;
	va_list ap;

	va_start(ap, fmt);

	while ((c = *p++)) {
		if (c == '%') {
			int flags = 0;
			int width = 0;
			int pad = ' ';

			if (*p == '0') {
				p++;
				pad = '0';
			}
			if (*p == '-') {
				p++;
				flags |= 1;
			}
			while (*p >= '0' && *p <= '9')
				width = width * 10 + (*p++) - '0';
			switch (*p++) {
			case 'x':
				uprint(va_arg(ap, unsigned int), 16, width, pad);
				break;
			case 'o':
				uprint(va_arg(ap, unsigned int), 8, width, pad);
				break;
			case 'b':
				uprint(va_arg(ap, unsigned int), 2, width, pad);
				break;
			case 'u':
				uprint(va_arg(ap, unsigned int), 10, width, pad);
				break;
			case 'd':
				iprint(va_arg(ap, unsigned int), 10, width, pad);
				break;
			case 's': {
				const char *s = va_arg(ap, const char *);

				width -= dstrlen(s);
				if (flags & 1) {
					dputs(s);
					while (width-- > 0)
						dputc(pad);
				} else {
					while (width-- > 0)
						dputc(pad);
					dputs(s);
				}
				break; }
			case 0:
				p--;
				break;
			}
#if 0
		} else if (c == '\\' && *p) {
			switch (*p++) {
			case 'n': dputc(13); dputc('\n'); break;
			case 'r': dputc('\r'); break;
			case 't': dputc('\t'); break;
				// case '0':
				// case 'x':
			}
#endif
		} else {
			if (c == 10)
				dputc(13);
			dputc(c);
		}
	}

	va_end(ap);
}

unsigned int dstrtoul(char *s, char **end, unsigned int base) {
	unsigned int v = 0;

	while (*s) {
		int c = *s;

		if (c >= 'a' && c <='z')
			c -= 0x20;

		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A')
			 c -= 'A' - 10;
		else
			break;

		s++;
		v = v * base + c;
	}

	if (end)
		*end = s;

	return v;
}


#ifdef TEST
int main() {
	dprintf("A simple string\n");
	dprintf("A %s string\n", "simple");
	dprintf("A an aligned ->%-20s string\n", "simple");
	dprintf("A an aligned %20s<- string\n", "simple");
	dprintf("1+2 = %d\n", 1+2);
	dprintf("A = 0b%b 0%o %d 0x%x\n", 'A', 'A', 'A', 'A');
	dprintf("A = 0b%08b 0%08o %08d 0x%08x\n", 'A', 'A', 'A', 'A');
}
#endif
