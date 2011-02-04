/* simple multi-processing example */

#include "graphics.h"
#include "lists.h"
#include "pbits.h"

#include "omap3-video.h"
#include "omap3-system.h"
#include "omap3-intc.h"
#include "omap3-gptimer.h"

#define WIDTH 1024
#define HEIGHT 768
#define FBADDR ((void *)0x88000000)

// task control block specific to this 'architecture'
struct tcb {
	uint32_t pc;
	uint32_t spsr;
	// <- sp_irq points here always
	uint32_t regs[15];
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
// all tasks
struct List tasks = INIT_LIST(tasks);

// the assembly halpers
extern void irq_new_task(void *tcb);
extern void *irq_current_task(void);
extern void local_exceptions_init(void);

void task(int id);

// global rastport, only used to init per-task ones
struct RastPort *rp;

const uint32_t colours[] = { 0xff0000, 0x00ff00, 0x0000ff, 0xffffff };

// just writes the task id to the screen over and over
void task(int id) {
	struct RastPort rp1;
	struct BitMap bm1;
	uint32_t col = colours[id];

	dprintf("task %08x colour %06x sp =~ %08x\n", id, col, &rp1+1);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		int c;

		for (c=0;c<2;c++) {
			int y, x;
			int yt = id*196, yb = yt + 128;
			int colour = c ? col : ~col;

			setColour(&rp1, colour);

			for (y=yt;y<yb;y+=1) {
				for (x=0;x<WIDTH;x+=1) {
					moveTo(&rp1, x, y);
					drawRect(&rp1, 1, 1);
				}
			}
		}
	}
}

/* setup */
struct task t1, t2, t3, t4;
struct task *taskinit[] = { &t1, &t2, &t3, &t4, 0 };

void gptimer1_handler(int id) {
	uint32_t irq = reg32r(GPTIMER1_BASE, GPT_TISR);

	{
		static int blink = 0;

		if (blink & (1<<2)) {
			LEDS_OFF(LED0);
			LEDS_ON(LED1);
		} else {
			LEDS_ON(LED0);
			LEDS_OFF(LED1);
		}
		blink++;
	}

	// check for overflow int
	if (irq & OVF_IT_FLAG) {
		Remove(&thistask->Node);
		AddTail(&tasks, &thistask->Node);

		thistask = (struct task *)tasks.Head;
		irq_new_task(tcb_to_sp(&thistask->tcb));
	}

	// clear ints
	reg32w(GPTIMER1_BASE, GPT_TISR, ~0);
}

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
		t->tcb.pc = (uint32_t)task;
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
	irq_new_task(tcb_to_sp(&thistask->tcb));

	//
	// Setup timer interrupt via GPTIMER1
	//

	// timer off
	reg32s(GPTIMER1_BASE, GPT_TCLR, 1, 0);

	// Sets true 1Khz rate w/ 32768Hz clock:  S 16.2.4.3 TRM D
	reg32w(GPTIMER1_BASE, GPT_TPIR, 232000);
	reg32w(GPTIMER1_BASE, GPT_TNIR, -768000);
	reg32w(GPTIMER1_BASE, GPT_TLDR, 0xffffffe0);
	reg32w(GPTIMER1_BASE, GPT_TCRR, 0xffffffe0);

	// clear int status bits
	reg32w(GPTIMER1_BASE, GPT_TISR, ~0);
	// enable overflow int
	reg32w(GPTIMER1_BASE, GPT_TIER, OVF_IT_FLAG);

	// dividisor = 100 -> 10Hz so it's visible
	reg32w(GPTIMER1_BASE, GPT_TOCR, 0);
	reg32w(GPTIMER1_BASE, GPT_TOWR, 100);

	irq_set_handler(INTC_GPT1_IRQ, gptimer1_handler);
	irq_set_mask(INTC_GPT1_IRQ, 1);

	// force 32K clock
	reg32s(0x48004c00, 0x40, 1, 0);

	// turn timer back on
	reg32s(GPTIMER1_BASE, GPT_TCLR, 1|2|(2<<10), ~0);

	dprintf("enabling interrupts\n");

	// data barrier for previous register writes
	asm volatile("dsb");

	// turn on irq's, which enables task switching
	irq_enable();

	// jump to user mode - we are now 'task 0'
	asm volatile("cps #0x10");
	asm volatile("ldr sp,=0x88000000 - 32768");

	task(0);

	return 0;
}
