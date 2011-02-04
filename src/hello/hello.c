
#include "graphics.h"
#include "pbits.h"

#define WIDTH 1024
#define HEIGHT 768
#define FBADDR ((void *)0x88000000)

int main(int argc, char **argv) {
	struct RastPort *rp;

	//video_init(1280, 1024);
	video_init(1024, 768);
	rp = graphics_init(FBADDR, WIDTH, HEIGHT, BM_RGB16);
	omap_attach_framebuffer(0, rp->drawable.bitmap);

	// also set it to the tv out (top-left corner of same data)
	omap_attach_framebuffer(VID_VID2 | VID_TVOUT, rp->drawable.bitmap);

	moveTo(rp, 0, 0);
	setColour(rp, 0x3e31a2);
	drawRect(rp, WIDTH, HEIGHT);

	setColour(rp, 0xffffff);
	moveTo(rp, 20, 20);
	drawString(rp, "Hello very small subset of the world!");
	moveTo(rp, 40, 40);
	drawString(rp, "Hello very small subset of the world!");
	moveTo(rp, 60, 60);
	drawString(rp, "Hello very small subset of the world!");

	ksleep(5000);

	int i;

	for (i=0;i<128;i++) {
		moveTo(rp, 20+i + 1, 50+i*3);
		drawRect(rp, 128-i, 2);
	}

	ksleep(5000);

	for (i=0;i<128;i++) {
		moveTo(rp, 20, 50+i*3);
		drawRect(rp, i, 2);
	}

	ksleep(5000);

	while (1) {
		int w = rand() % (WIDTH - 50);
		int h = rand() % (HEIGHT - 50);
		int x = rand() % (WIDTH-w-50) + 25;
		int y = rand() % (HEIGHT-h-50) + 25;
		int c = rand() & 0xffffff;

		if (h + y >= 760)
			dprintf("rect max %d\n", h+y);

		setColour(rp, c);
		moveTo(rp, x, y);
		drawRect(rp, w, h);
	}

	debugmon();

	return 0;
}
