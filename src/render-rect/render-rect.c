
#include "graphics.h"
#include "pbits.h"
#include "pmon.h"
#include "dma4.h"

#define WIDTH 1024
#define HEIGHT 768
#define FBADDR ((void *)0x88000000)

static void drawRect_short(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	int i, j;

	for (j = 0;j<h;j++) {
		for (i=0;i<w;i++) {
			outp[i] = colour;
		}
		outp = (unsigned short *)((char *)outp + rp->drawable.bitmap->stride);
	}
}

static void drawRect_long(struct RastPort *rp, int w, int h) {
	unsigned int colour = (rp->colour << 16) | rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	int i, j;
	int rem = 3 - ((((unsigned int)outp) + w * 2) & 3);
	int rw = w / 2;

	// This may or may not compile properly on newer gcc.
	// just ignore it if so, it isn't important.

	for (j = 0;j<h;j++) {
		unsigned short *rowp = outp;
		
		if (((unsigned int)rowp) & 2)
			*((unsigned short *)rowp)++ = colour;

		unsigned int *rowi = (unsigned int *)rowp;

		for (i=0;i<rw;i++) {
			rowi[i] = colour;
		}

		if (rem & 2)
			rowp[i*2] = colour;

		outp = (unsigned short *)((char *)outp + rp->drawable.bitmap->stride);
	}
}

extern void fill_rect_565(void *, int, int, unsigned int, int);

static void drawRect_arm(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	fill_rect_565(outp, w, h, colour, rp->drawable.bitmap->stride);
}

extern void fill_rect_565_neon_test(void *, int, int, unsigned int, int);

static void drawRect_neon(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	fill_rect_565_neon_test(outp, w, h, colour, rp->drawable.bitmap->stride);
}

extern void fill_rect_565_neon_test2(void *, int, int, unsigned int, int);

static void drawRect_neon2(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	fill_rect_565_neon_test2(outp, w, h, colour, rp->drawable.bitmap->stride);
}

extern void fill_rect_565_neon_test3(void *, int, int, unsigned int, int);

static void drawRect_neon3(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	fill_rect_565_neon_test3(outp, w, h, colour, rp->drawable.bitmap->stride);
}

extern void fill_rect_565_neon_test4(void *, int, int, unsigned int, int);

static void drawRect_neon4(struct RastPort *rp, int w, int h) {
	unsigned int colour = rp->colour;
	unsigned short *outp = rp->point;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	fill_rect_565_neon_test3(outp, w, h, colour, rp->drawable.bitmap->stride);
}

extern void memset16_armneon(void *addr, unsigned short c, unsigned int len);

static void memset_neonasm(struct RastPort *rp, int w, int h) {
	// not rect-fill but should be same amount
	memset16_armneon((void *)rp->drawable.bitmap->data, rp->colour, 0x14a000);
}

static void drawRect_dma(struct RastPort *rp, int w, int h) {
	struct dma4 volatile *dma = (void *)0x48056000;
	unsigned int colour = (rp->colour << 16) | rp->colour;
	const int c = 10;

	if (w + rp->x > rp->drawable.bitmap->width)
		w = rp->drawable.bitmap->width - rp->x;
	if (h + rp->y > rp->drawable.bitmap->height)
		h = rp->drawable.bitmap->height - rp->y;

	// wait for dma to be free
	while (dma->chan[c].CCR & DMA4_ENABLE)
		;

	dma->chan[c].CCR = DMA4_DST_AMODE_DIDX | DMA4_CONST_FILL_ENABLE;

	dma->chan[c].CSDP = DMA4_DATA_TYPE_16 | ( 2 << 16 );
	dma->chan[c].CEN = w;	/* elements per frame */
	dma->chan[c].CFN = h;	/* frames per block */
	dma->chan[c].CDSA = (uint32_t)rp->point; /* dest address */

	dma->chan[c].CLNK_CTRL = 0;


	dma->chan[c].CDEl = 1;	/* element index */
	dma->chan[c].CDFl = rp->drawable.bitmap->stride+1-w*2;	/* frame index */

	dma->chan[c].COLOR = colour;

	// turn it on!
	dma->chan[c].CCR = DMA4_DST_AMODE_DIDX | DMA4_CONST_FILL_ENABLE | DMA4_DST_PACKED | DMA4_ENABLE;

	// wait for dma to finish
	while (dma->chan[c].CCR & DMA4_ENABLE)
		;
}

static struct {
	const char *name;
	void (*draw)(struct RastPort *, int, int);
	unsigned int total;
	unsigned int writes;
	unsigned int external;
	unsigned int exec;
	unsigned int stall;
} tobm[] = {
	{ "C short", drawRect_short },
	{ "C long", drawRect_long },
	{ "ARM asm", drawRect_arm },
	{ "NEON", drawRect_neon, 1 },
	{ "NEON2", drawRect_neon2, 1 },
	{ "NEON3", drawRect_neon3, 1 },
	{ "NEON4", drawRect_neon4, 1 },
	{ "msNEON", memset_neonasm, 1 },
	{ "sDMA", drawRect_dma },
	{ 0 }
};

int main(int argc, char **argv) {
	struct RastPort *rp;

	// this sets up the cached memory
	mmu_simple_init();

	video_init(1280, 1024);
	rp = graphics_init(FBADDR, WIDTH, HEIGHT, BM_RGB16);
	omap_attach_framebuffer(0, rp->drawable.bitmap);

	moveTo(rp, 0, 0);
	setColour(rp, 0x3e31a2);
	drawRect(rp, WIDTH, HEIGHT);

	// init timers
	pmon_init();

	// to use timers:
	// 1. disable timers
	// 2. set what to time
	// 3. reset timers
	// 4. enable timers
	// 5. execute target code
	// 6. disable timers
	// 7. read timers (& check overflow)
	
	int i;
	unsigned int max = 0;
	unsigned int min = ~0;
	for (i=0;tobm[i].name;i++) {
		setColour(rp, 0x000000);
		moveTo(rp, 32, 32);

		pmon_disable(CNTEN_ALL);

		pmon_pcevent(0, EVTSEL_DWRITE_INSTNS);
		pmon_pcevent(1, EVTSEL_AXIWRITE);
		pmon_pcevent(2, EVTSEL_INSTNS);
		// for neon code, monitor neon instruction queue full instead
		if (tobm[i].total)
			pmon_pcevent(3, EVTSEL_NEON_STALL);
		else
			pmon_pcevent(3, EVTSEL_ISTALL);

		pmon_reset(PMNC_CCRESET | PMNC_PCRESET);

		pmon_enable(CNTEN_ALL);
		tobm[i].draw(rp, WIDTH-64, HEIGHT-64);
		pmon_disable(CNTEN_ALL);

		dprintf("drawRect() %s\n", tobm[i].name);
		dprintf(" total cycles=%d\n", pmon_ccnt());
		dprintf(" dwrite intns=%d\n", pmon_pc(0));
		dprintf(" ext writes  =%d\n", pmon_pc(1));
		dprintf(" iexec       =%d\n", pmon_pc(2));
		if (tobm[i].total)
			dprintf(" neon stall  =%d\n", pmon_pc(3));
		else
			dprintf(" istall      =%d\n", pmon_pc(3));

		tobm[i].total = pmon_ccnt();
		tobm[i].writes = pmon_pc(0);
		tobm[i].external = pmon_pc(1);
		tobm[i].exec = pmon_pc(2);
		tobm[i].stall = pmon_pc(3);

		// i think ccount == 600Mhz on the rev3.  *shrug*
#define mbytes ((1024 - 64) * (768 - 64) * 2.0 / 1048576.0) 
#define seconds (tobm[i].total / 600000000.0)
		dprintf(" MB/S = %d\n",  (int) (mbytes / seconds));

		if (tobm[i].total > max)
			max = tobm[i].total;
		if (tobm[i].total < min)
			min = tobm[i].total;
	}

	dprintf("\n\nCode      \t       Total\tSlowest\tFastest\n");
	for (i=0;tobm[i].name;i++) {		
		unsigned int pc = tobm[i].total * 100 / max;
		unsigned int fc = tobm[i].total * 100 / min;

		dprintf("%-10s\t%12u\t%u.%02u\t%u.%02u\n",
			tobm[i].name,
			tobm[i].total,
			pc / 100, pc % 100,
			fc / 100, fc % 100);
	}

	// Now just loop forever with a 'screensaver' using each routine 1000x
	int k = 0;
	while (1) {
		srand(k++);

		for (i=0;tobm[i].name;i++) {
			int j;
			
			dprintf("Drawing 1000 boxes using %s\n", tobm[i].name);

			moveTo(rp, 0, 0);
			setColour(rp, 0x3e31a2);
			drawRect_dma(rp, WIDTH, HEIGHT);
			
			moveTo(rp, 20, 20);
			setColour(rp, 0xffffff);
			drawString(rp, tobm[i].name);
			
			for (j=0;j<1000;j++) {
				int w = rand() % (WIDTH - 50);
				int h = rand() % (HEIGHT - 50);
				int x = rand() % (WIDTH-w-50) + 25;
				int y = rand() % (HEIGHT-h-50) + 25;
				int c = rand() & 0xffffff;
				
				setColour(rp, c);
				moveTo(rp, x, y);
				tobm[i].draw(rp, w, h);
			}
		}
	}

	debugmon();

	return 0;
}
