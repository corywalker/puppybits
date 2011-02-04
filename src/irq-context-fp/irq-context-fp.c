/* simple multi-processing example */

#include "graphics.h"
#include "lists.h"
#include "pbits.h"

#include "omap3-video.h"
#include "omap3-system.h"
#include "omap3-intc.h"

#define WIDTH 1024
#define HEIGHT 768
#define FBADDR ((void *)0x88000000)

// task control block specific to this 'architecture'
struct tcb {
	uint32_t pc;
	uint32_t spsr;
	// <- sp_irq points here always
	uint32_t regs[15];
	uint32_t fpscr;
	union {
		float sregs[32];
		double dregs[32];
	} fpregs;
};

#define tcb_to_sp(tcb) (&(tcb)->regs[0])

// system task object
struct task {
	struct Node Node;

	struct tcb tcb;

	int id;

	// other crap for scheduling or kernel-per-task info would go here
};

// current task
struct task *thistask;
// task who the fp state belongs to
struct task *fptask;
// all tasks ready to run
struct List tasks = INIT_LIST(tasks);
// halted tasks
struct List tasks_halted = INIT_LIST(tasks_halted);

// the assembly halpers
extern void irq_new_task(void *tcb);
extern void *irq_current_task(void);
extern void local_exceptions_init(void);

void task(int id);

// DSS IRQ handler (irq 25)
// Is also invoked for DSI interrupts
void dispc_handler(int id) {
	uint32_t dssirq = reg32r(DSS_BASE, DSS_IRQSTATUS);

	{
		static int blink = 0;

		if (blink & (1<<6)) {
			LEDS_OFF(LED0);
			LEDS_ON(LED1);
		} else {
			LEDS_ON(LED0);
			LEDS_OFF(LED1);
		}
		blink++;
	}

	// see if we have any dispc interrupts
	if (dssirq & DSS_DISPC_IRQ) {
		uint32_t irqstatus = reg32r(DISPC_BASE, DISPC_IRQSTATUS);

		if (1 && (irqstatus & DISPC_VSYNC)) {
			// we do nothing special, just round-robin all tasks at every jiffy
			Remove(&thistask->Node);
			AddTail(&tasks, &thistask->Node);

			thistask = (struct task *)tasks.Head;
			irq_new_task(tcb_to_sp(&thistask->tcb));
		}

		// we handle no other interrrupts, so clear all interrupt status bits if any set
		reg32w(DISPC_BASE, DISPC_IRQSTATUS, irqstatus);
	}
	// check for dsi ints (to clear them)
	if (dssirq & DSS_DSI_IRQ) {
		// not expecting this, just clear everything
		reg32w(DSI_BASE, DSI_IRQSTATUS, ~0);
	}
}

// undefined instruction, suspend task
void uni_handler(void) {
	//	dprintf("%08x: illegal instruction, task %d halted\n", thistask->tcb.pc, thistask->id);

	// retire the task
	Remove(&thistask->Node);
	AddTail(&tasks_halted, &thistask->Node);

	// schedule the next read-to-run task
	thistask = (struct task *)tasks.Head;
	irq_new_task(tcb_to_sp(&thistask->tcb));
}

// global rastport, only used to init per-task ones
struct RastPort *rp;

const uint32_t colours[] = { 0xff0000, 0x00ff00, 0x0000ff, 0xffffff };

void dostuff(struct RastPort *rp, int id, uint32_t col) {
	int c;

	for (c=0;c<2;c++) {
		int y, x;
		int yt = id*196, yb = yt + 128;
		int colour = c ? col : ~col;

		setColour(rp, colour);

		// this is just to try and make it slow enough to be interesting
		for (y=yt;y<yb;y+=1) {
			for (x=0;x<WIDTH;x+=1) {
				moveTo(rp, x, y);
				drawRect(rp, 1, 1);
			}
		}
	}
}

// task1 does fops once in a while
void task1(int id) {
	struct RastPort rp1;
	struct BitMap bm1;
	uint32_t col = colours[id];
	int count = 0;
	float t1_total = 0.f;
	int t1_itotal = 0;

	dprintf("task %08x colour %06x sp =~ %08x\n", id, col, &rp1+1);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		dostuff(&rp1, id, col);

		count++;
		if (count >0) {
			dprintf("fptask = %08x\n", fptask);
			t1_total += 1;
			t1_itotal += 1;
			if ((int)(t1_total) != t1_itotal) {
				dprintf("fpu funny in task 0\n");
			}
			count = 0;
		}
	}
}

// task2 does fops at a different rate
void task2(int id) {
	struct RastPort rp1;
	struct BitMap bm1;
	uint32_t col = colours[id];
	int count = 0;
	float t2_total = 0.f;
	int t2_itotal = 0;

	dprintf("task %08x colour %06x sp =~ %08x\n", id, col, &rp1+1);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		dostuff(&rp1, id, col);

		count++;
		if (count > 3) {
			t2_total += 1;
			t2_itotal += 1;
			if ((int)(t2_total) != t2_itotal) {
				dprintf("fpu funny in task 1\n");
			}
			count = 0;
		}
	}
}

// task3 does an illegal instruction after a while
void task3(int id) {
	struct RastPort rp1;
	struct BitMap bm1;
	uint32_t col = colours[id];
	int count = 0;

	dprintf("task %08x colour %06x sp =~ %08x\n", id, col, &rp1+1);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		dostuff(&rp1, id, col);

		// some contrived examples to check the code.
		count++;
		if (count > 5) {
			asm("cdp p7, #15, c0, c0, c0, #7");
			dprintf("I'm suspended now .. shouldn't see this\n");
			count = 0;
		}
	}
}
// task4 just flashes colours
void task4(int id) {
	struct RastPort rp1;
	struct BitMap bm1;
	uint32_t col = colours[id];

	dprintf("task %08x colour %06x sp =~ %08x\n", id, col, &rp1+1);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		dostuff(&rp1, id, col);
	}
}

/* setup */
struct task t1, t2, t3, t4;
struct task *taskinit[] = { &t1, &t2, &t3, &t4, 0 };
uint32_t taskpc[] = { (uint32_t)task1, (uint32_t)task2, (uint32_t)task3, (uint32_t)task4 };

int main(int argc, char **argv) {

	local_exceptions_init();

	// example is too fast/boring with caches on, so don't
	//mmu_simple_init();

	video_init(1280, 1024);
	rp = graphics_init(FBADDR, WIDTH, HEIGHT, BM_RGB16);
	omap_attach_framebuffer(0, rp->drawable.bitmap);

	// also set it to the tv out (top-left corner of same data)
	omap_attach_framebuffer(VID_VID2 | VID_TVOUT, rp->drawable.bitmap);

	moveTo(rp, 0, 0);
	setColour(rp, 0x3e31a2);
	drawRect(rp, WIDTH, HEIGHT);

	// setup tasks to run
	int i;

	NewList(&tasks);
	dprintf("tasks   %08x,  Head=%08x Tail=%08x TailPred=%08x\n",
		&tasks, tasks.Head, tasks.Tail, tasks.TailPred);

	for (i=0;taskinit[i];i++ ){
		struct task *t = taskinit[i];

		// set initial pc and proc state
		t->tcb.pc = taskpc[i];

		// runs in user mode
		t->tcb.spsr = 0x10;
		// initial argument 0 in reg 0
		t->tcb.regs[0] = i;
		t->id = i;
		int j;
		for (j=1;j<10;j++)
			t->tcb.regs[j] = j;
		// initial stack - 4K each, before 32K of master stack
		t->tcb.regs[13] = 0x88000000 - 32768 - i*4096;
		AddTail((struct List *)&tasks, &t->Node);

		dprintf("adding task %d = %08x pc=%08x sp=%08x\n", i, t, t->tcb.pc, t->tcb.regs[13]);

	}
	dprintf("tasks   %08x,  Head=%08x Tail=%08x TailPred=%08x\n",
		&tasks, tasks.Head, tasks.Tail, tasks.TailPred);

	// set task 0 to `execute first' (it's actually us)
	thistask = &t1;
	fptask = 0;
	irq_new_task(tcb_to_sp(&thistask->tcb));

	// add an irq handler for the vsync interrupt (lcd display?)
	irq_set_handler(INTC_DSS_IRQ, dispc_handler);
	irq_set_mask(INTC_DSS_IRQ, 1);

	// disable all but vsync
	reg32w(DISPC_BASE, DISPC_IRQENABLE, DISPC_VSYNC);
	reg32w(DISPC_BASE, DISPC_IRQSTATUS, ~0);
	// dss intterrupt can also receive DSI, so disable those too
	reg32w(DSI_BASE, DSI_IRQENABLE, 0);
	reg32w(DSI_BASE, DSI_IRQSTATUS, ~0);

	dprintf("enabling interrupts\n");

	// data barrier for previous register writes
	asm volatile("dsb");

	// turn on irq's, which enables task switching
	irq_enable();

	// jump to user mode - we are now 'task 0'
	asm volatile("cps #0x10");
	asm volatile("ldr sp,=0x88000000 - 32768");

	task1(0);

	return 0;
}
