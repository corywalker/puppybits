/* Simple example using vblank interrupt */

#include "graphics.h"
#include "pbits.h"

#include "omap3-video.h"
#include "omap3-system.h"
#include "omap3-intc.h"

#define WIDTH 1024
#define HEIGHT 768
#define FBADDR ((void *)0x88000000)

volatile int hidden = 0;

// DSS IRQ handler (irq 25)
// Is also invoked for DSI interrupts
void dispc_handler(int id) {
	static int line = 0;
	static int dir = 1;
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

		if (irqstatus & DISPC_VSYNC) {
			uint32_t addr = (uint32_t)FBADDR + line * WIDTH * 2;

 			// update the graphic layer 0 address (video out) to scroll it
			reg32w(DISPC_BASE, DISPC_GFX_BA0, addr);
			reg32w(DISPC_BASE, DISPC_GFX_BA1, addr);
			reg32s(DISPC_BASE, DISPC_CONTROL, DISPC_GOLCD, ~0);

			if (line == 0) {
				dir = 1;
				hidden = 1;
			} else if (line == 768) {
				dir = -1;
			}
			
			line += dir*4;
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

int main(int argc, char **argv) {
	struct RastPort *rp;
	struct RastPort rp1;
	struct BitMap bm1;

	video_init(1280, 1024);
	rp = graphics_init(FBADDR, WIDTH, HEIGHT, BM_RGB16);
	omap_attach_framebuffer(0, rp->drawable.bitmap);

	// also set it to the tv out (top-left corner of same data)
	omap_attach_framebuffer(VID_VID2 | VID_TVOUT, rp->drawable.bitmap);

	moveTo(rp, 0, 0);
	setColour(rp, 0x3e31a2);
	drawRect(rp, WIDTH, HEIGHT);

	// setup anothe rastport to draw to the `second screen' after the other
	rp1 = *rp;
	bm1 = *rp->drawable.bitmap;
	rp1.drawable.bitmap = &bm1;
	bm1.data = FBADDR + WIDTH*HEIGHT*2;
	moveTo(&rp1, 0, 0);
	setColour(&rp1, 0x0000ff);
	drawRect(&rp1, WIDTH, HEIGHT);

	setColour(rp, 0xffffff);
	moveTo(rp, 20, 20);
	drawString(rp, "Hello very small subset of the world!");

	moveTo(&rp1, 900, 750);
	setColour(&rp1, 0xffffff);
	drawString(&rp1, "Peek a boo!");

	// add an irq handler for the vsync interrupt (lcd display?)
	irq_set_handler(INTC_DSS_IRQ, dispc_handler);
	irq_set_mask(INTC_DSS_IRQ, 1);

	// disable all but vsync
	reg32w(DISPC_BASE, DISPC_IRQENABLE, DISPC_VSYNC);
	reg32w(DISPC_BASE, DISPC_IRQSTATUS, ~0);
	// dss intterrupt can also receive DSI, so disable those too
	reg32w(DSI_BASE, DSI_IRQENABLE, 0);
	reg32w(DSI_BASE, DSI_IRQSTATUS, ~0);

	// We enter main with irq's disabled
	irq_enable();

	while (1) {
		int w = rand() % (WIDTH - 50);
		int h = rand() % (HEIGHT - 50);
		int x = rand() % (WIDTH-w-50) + 25;
		int y = rand() % (HEIGHT-h-50) + 25;
		int c = rand() & 0xffffff;
		struct RastPort *r = rp;

		if (hidden) {
			hidden = 0;
			r = &rp1;
		}
		setColour(r, c);
		moveTo(r, x, y);
		drawRect(r, w, h);
	}

	return 0;
}
