/* simple multi-processing example with ipc and fpu */

#include <string.h>

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

typedef uint32_t sigmask_t;

typedef enum taskstate_t {
	TS_INIT,
	TS_READY,
	TS_RUN,
	TS_WAIT,
	TS_REMOVED
} taskstate_t;

// system task object
struct task {
	struct Node Node;

	struct tcb tcb;

	int id;
	taskstate_t state;

	sigmask_t sigAllocated;
	sigmask_t sigWaiting;
	sigmask_t sigReceived;
};

// current task
struct task *thistask;
// task who the fp state belongs to
struct task *fptask;
// all tasks ready to run
struct List tasks = INIT_LIST(tasks);
// waiting tasks
struct List tasks_wait = INIT_LIST(tasks_wait);
// halted tasks
struct List tasks_halted = INIT_LIST(tasks_halted);

// the assembly halpers
extern void irq_new_task(void *tcb);
extern void *irq_current_task(void);
extern void local_exceptions_init(void);

void task(int id);

/* user-interface to system calls */
int AllocSignal(int sigNum) {
	register int res asm ("r0");
	register int sig asm ("r0") = sigNum;

	asm volatile("mov r7,#0; svc #0"
		     : "=r" (res)
		     : "r" (sig)
		     : "r7", "r1", "r2", "r3", "r12");

	return res;
}

void FreeSignal(int sigNum) {
	register int sig asm ("r0") = sigNum;

	asm volatile("mov r7,#1; svc #1"
		     :
		     : "r" (sig)
		     : "r7", "r1", "r2", "r3", "r12");
}

void Signal(int taskid, sigmask_t sigMask) {
	register int tid asm ("r0") = taskid;
	register int sigm asm ("r1") = sigMask;

	asm volatile("mov r7,#2; svc #2"
		     :
		     : "r" (tid), "r" (sigm)
		     : "r7", "r2", "r3", "r12");
}

sigmask_t Wait(sigmask_t sigMask) {
	register int res asm ("r0");
	register int sigm asm("r0") = sigMask;

	asm volatile("mov r7,#3; svc #3"
		     : "=r" (res)
		     : "r" (sigm)
		     : "r7", "r1", "r2", "r3", "r12");

	return res;
}

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
// invalid svc call, suspend task
void task_error_handler(void) {
	// retire the task
	Remove(&thistask->Node);
	AddTail(&tasks_halted, &thistask->Node);

	// schedule the next read-to-run task
	thistask = (struct task *)tasks.Head;
	irq_new_task(tcb_to_sp(&thistask->tcb));
}

/* simple find task, search both queues */
struct task *findTask(int tid) {
	struct Node *nw = tasks.Head, *nn;

	nn = nw->Succ;
	while (nn) {
		if (((struct task *)nw)->id == tid)
			return (struct task *)nw;
		nw = nn;
		nn = nn->Succ;
	}

	nw = tasks_wait.Head;
	nn = nw->Succ;
	while (nn) {
		if (((struct task *)nw)->id == tid)
			return (struct task *)nw;
		nw = nn;
		nn = nn->Succ;
	}

	return 0;
}

// system call implementations
// Allocate a signal, if sigNum == -1, find one, otherwise try to use the specific one given
int svc_AllocSignal(int sigNum) {
	int res = -1;

	if (sigNum == -1) {
		res = ffs(~thistask->sigAllocated) - 1;
	} else {
		if ((thistask->sigAllocated & (1<<sigNum)) == 0)
			res = sigNum;
	}
	if (res >= 0)
		thistask->sigAllocated |= (1 << res);

	return res;
}

// Free a signal
void svc_FreeSignal(int sigNum) {
	sigmask_t m = (1<<sigNum);
	struct task *tcb = thistask;

	if (tcb->sigAllocated & m) {
		tcb->sigAllocated &= ~m;
		tcb->sigWaiting &= ~m;
		tcb->sigReceived &= ~m;
	} else {
		// panic/error?
	}
}

// Send a signal to another task
void svc_Signal(int tid, int sigNum) {
	struct task *tcb = findTask(tid);

	if (!tcb)
		return;

	sigmask_t m = (1<<sigNum);
	sigmask_t sigs;

	tcb->sigReceived |= m;
	// if waiting, wake it up, and set return code
	if (tcb->state == TS_WAIT) {
		sigs = tcb->sigReceived & tcb->sigWaiting;
		if (sigs) {
			Remove(&tcb->Node);
			AddHead(&tasks, &tcb->Node);

			tcb->state = TS_RUN;
			tcb->tcb.regs[0] = sigs;

			thistask = tcb;
			irq_new_task(tcb_to_sp(&thistask->tcb));
		}
	}
}

// Wait for any of a set of signals to be received
// If they are already here, just keep going, otherwise
// go to sleep and schedule next task ready to run
sigmask_t svc_Wait(sigmask_t sigMask) {
	struct task *tcb = thistask;
	sigmask_t sigs;

	sigs = tcb->sigReceived & sigMask;
	if (sigs) {
		tcb->sigReceived &= ~sigs;
	} else {
		dprintf("%d: waiting for signals %08x\n", tcb->id, sigMask);
		tcb->sigWaiting = sigMask;
		tcb->state = TS_WAIT;
		Remove(&tcb->Node);
		AddTail(&tasks_wait, &tcb->Node);

		thistask = (struct task *)tasks.Head;
		irq_new_task(tcb_to_sp(&thistask->tcb));
	}

	return sigs;
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
	int sigNum = AllocSignal(16);

	dprintf("task %08x colour %06x sp =~ %08x signum=%d\n", id, col, &rp1+1, sigNum);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		sigmask_t s = Wait((1<<sigNum));

		dprintf("%d: awoken with signals %08x\n", id, s);

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
	int sigNum = AllocSignal(16);

	dprintf("task %08x colour %06x sp =~ %08x signum=%d\n", id, col, &rp1+1, sigNum);

	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;

	while (1) {
		sigmask_t s = Wait((1<<sigNum));

		dprintf("%d: awoken with signals %08x\n", id, s);
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
// task4 tells the other tasks when to run
void task4(int id) {
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
		count++;
		// tell task 1 and 2 when to run
		if (count & 0x1)
			Signal(0, 16);
		if (count & 0x2)
			Signal(1, 16);
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

	AllocSignal(1);

	return 0;
}
