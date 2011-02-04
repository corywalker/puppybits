/*  interrupts.c: C code exception handlers

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

#include "pbits.h"

#include "omap3-intc.h"

struct exception_detail {
	uint32_t reg[16];
	uint32_t sr;
};

const char const * const extypes[8] = {
	"Reset",
	"Undefined Instruction",
	"Software Interrupt",
	"Prefetch Abort",
	"Data Abort",
	"Not used",
	"IRQ",
	"FIQ"
};

void exception_fatal_dump(int type, struct exception_detail *e) {
	int i;
	static int inexcept = 0;

	if (inexcept) {
		dprintf("Double-exception\n");
		while (1);
	}
	inexcept++;

	dprintf("Exception: %s\n", extypes[type]);
	dprintf(" pc: 0x%08x sr: 0x%08x\n", e->reg[15], e->sr);
	for (i=0;i<16;i++) {
		if (i<10)
			dprintf(" ");
		dprintf("r%d: 0x%08x", i, e->reg[i]);
		// stack pointer?
		if (i == 13 && e->reg[i]) {
			uint32_t *p = (uint32_t *)e->reg[i];
			int j;

			for (j=0;j<8;j++) {
				dprintf(" 0x%08x", p[j]);
			}
		}
		dprintf("\n");
	}

	debugmon();
}

void exception_swi(unsigned int no, void *arg) {
	dprintf("Exception: Software int\n");
}

void exception_irq(int id) {
	// turns off both leds
	//*((volatile unsigned int *)0x49056090) = 0x00600000;
	//dprintf("Exception: IRQ %d\n", id);
}

void exception_fiq(void) {
	// turns off 1 led
	*((volatile unsigned int *)0x49056090) = 0x00400000;
	dprintf("Exception: FIQ\n");
}


/* ********************************************************************** */
/* IRQs */

extern irq_fn irq_vectors[128];

static volatile int irq_depth = 1;

// for LEDS_ON
#include "omap3-system.h"

// must only be called from priv mode
// must not be called from interrupt handler
void irq_disable(void) {
	if (irq_depth == 0) {
		asm volatile("dsb");
		asm volatile("cpsid i");
	}
	irq_depth++;
}

// must only be called from priv mode
// must not be called from interrupt handler
void irq_enable(void) {
	// 'error'
	if (irq_depth <= 0)
		return;

	if (irq_depth == 1) {
		irq_depth = 0;
		asm volatile("dsb");
		asm volatile("cpsie i");
	} else
		irq_depth -= 1;
}

// set irq mask bits, num = int number to change, on = clear mask if true
// TODO: what about fast/irq, priority levels, and all that jazz?
void irq_set_mask(int num, int on) {
	volatile unsigned int *p = (volatile unsigned int *)(INTCPS_BASE + (on ? INTCPS_MIR_CLEAR0 : INTCPS_MIR_SET0));

	p[(num >> 5)*(32/4)] = 1 << (num & 0x1f);

	// not sure if we need this when we toggle the bits, presumably irq's are off anyway
	asm volatile("dsb");
}

// set irq handler, returns old one
irq_fn irq_set_handler(int num, irq_fn irq) {
	irq_fn old = irq_vectors[num];

	irq_vectors[num] = irq;

	return old;
}
