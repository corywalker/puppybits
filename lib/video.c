/*  video.c: OMAP3 video driver

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

  video.c

  Very basic 'video' driver.

  Can initialise the LCD video output, and attach (multiple) overlays.

*/

#include "omap3-video.h"
#include "graphics.h"
#include "pbits.h"

typedef unsigned int uint32;

#define VENC_MODE_NTSC (0)
#define VENC_MODE_PAL (1)

struct video_mode {
	short width, height;
	char *name;
	uint32 dispc_size;
	uint32 dispc_timing_h;
	uint32 dispc_timing_v;
	uint32 dispc_divisor;
	uint32 dss_divisor;
};

// Master clock is 864 Mhz, and changing it is a pita since it cascades to other
// devices.
// Pixel clock is  864 / cm_clksel_dss.dss1_alwan_fclk / dispc_divisor.divisor.pcd
// So most of these modes are just approximate.
// List must be in ascending order.
struct video_mode modes[] = {
	// 640x480@72	31.500		640	24	40	128  	480	9	3	28
	{
		640, 480, "640x480-71",
		((480-1) << 16) | (640-1),
		(128 << DISPCB_HBP) | (24 << DISPCB_HFP) | (40 << DISPCB_HSW),
		(28 << DISPCB_VBP) | (9 << DISPCB_VFP) | (3 << DISPCB_VSW),
		2, 14
	},
	//	  800x600, 60Hz  	40.000  	800	40	128	88  	600	1	4	23
	{
		800, 600, "800x600-59",
		((600-1) << 16) | (800-1),
		(88 << DISPCB_HBP) | (40 << DISPCB_HFP) | (128 << DISPCB_HSW),
		(23 << DISPCB_VBP) | (1 << DISPCB_VFP) | (4 << DISPCB_VSW),
		2, 11
	},       	  
	// 1024x768, 60Hz  	65.000  	1024	24	136	160  	768	3	6	29
	{
		1024, 768, "1024x768-61",
		((768-1) << 16) | (1024-1),
		(160 << DISPCB_HBP) | (24 << DISPCB_HFP) | (136 << DISPCB_HSW),
		(29 << DISPCB_VBP) | (3 << DISPCB_VFP) | (6 << DISPCB_VSW),
		1, 13
	},
	{
		1280, 1024, "1280x1024-60",
		((1024-1) << 16) | (1280-1),
		(248 << DISPCB_HBP) | (48 << DISPCB_HFP) | (112 << DISPCB_HSW),
		(38 << DISPCB_VBP) | (1 << DISPCB_VFP) | (3 << DISPCB_VSW),
		1, 8
	},
};

static inline void Write32(void *base, unsigned int reg, unsigned int v) {
	*((volatile unsigned int *)(base+reg)) = v;
}

static inline uint32 Read32(void *base, unsigned int reg) {
	return *((volatile unsigned int *)(base+reg));
}

static inline void Set32(void *base, unsigned int reg, unsigned int m, unsigned int v) {
	unsigned int o;

	o = *((volatile unsigned int *)(base+reg));
	o &= ~m;
	o |= v;
	*((volatile unsigned int *)(base+reg)) = o;
}

/*
  Default video-encoder setup for NTSC and PAL from OMAP TRM S15.8.5.4
*/
struct	{
	int reg;
	unsigned int mode[2];
} venc_setup[] = {
	{ VENC_VIDOUT_CTRL, 		{ 0x00000001, 0x00000001 } },
	{ VENC_LLEN, 			{ 0x00000359, 0x0000035F } },
	{ VENC_FLENS, 			{ 0x0000020C, 0x00000270 } },
	{ VENC_HFLTR_CTRL, 		{ 0x00000000, 0x00000000 } },
	{ VENC_CC_CARR_WSS_CARR,	{ 0x043F2631, 0x2F7225ED } },
	{ VENC_C_PHASE, 		{ 0x00000000, 0x00000000 } },
	{ VENC_GAIN_U, 			{ 0x00000102, 0x00000111 } },
	{ VENC_GAIN_V, 			{ 0x0000016C, 0x00000181 } },
	{ VENC_GAIN_Y, 			{ 0x0000012F, 0x00000140 } },
	{ VENC_BLACK_LEVEL, 		{ 0x00000043, 0x0000003B } },
	{ VENC_BLANK_LEVEL, 		{ 0x00000038, 0x0000003B } },
	{ VENC_X_COLOR, 		{ 0x00000007, 0x00000007 } },
	{ VENC_M_CONTROL, 		{ 0x00000001, 0x00000002 } },
	{ VENC_BSTAMP_WSS_DATA,		{ 0x00000038, 0x0000003F } },
	{ VENC_S_CARR, 			{ 0x21F07C1F, 0x2A098ACB } },
	{ VENC_LINE21, 			{ 0x00000000, 0x00000000 } },
	{ VENC_LN_SEL, 			{ 0x01310011, 0x01290015 } },
	{ VENC_L21_WC_CTL, 		{ 0x0000F003, 0x0000F603 } },
	{ VENC_HTRIGGER_VTRIGGER, 	{ 0x00000000, 0x00000000 } },
	{ VENC_SAVID_EAVID, 		{ 0x069300F4, 0x06A70108 } },
	{ VENC_FLEN_FAL, 		{ 0x0016020C, 0x00180270 } },
	{ VENC_LAL_PHASE_RESET, 	{ 0x00060107, 0x00040135 } },
	{ VENC_HS_INT_START_STOP_X, 	{ 0x008E0350, 0x00880358 } },
	{ VENC_HS_EXT_START_STOP_X, 	{ 0x000F0359, 0x000F035F } },
	{ VENC_VS_INT_START_X, 		{ 0x01A00000, 0x01A70000 } },
	{ VENC_VS_INT_STOP_X_VS_INT_START_Y, 	{ 0x020701A0, 0x000001A7 } },
	{ VENC_VS_INT_STOP_Y_VS_EXT_START_X, 	{ 0x01AC0024, 0x01AF0000 } },
	{ VENC_VS_EXT_STOP_X_VS_EXT_START_Y, 	{ 0x020D01AC, 0x000101AF } },
	{ VENC_VS_EXT_STOP_Y, 			{ 0x00000006, 0x00000025 } },
	{ VENC_AVID_START_STOP_X, 		{ 0x03480078, 0x03530083 } },
	{ VENC_AVID_START_STOP_Y, 		{ 0x02060024, 0x026C002E } },
	{ VENC_FID_INT_START_X_FID_INT_START_Y, { 0x0001008A, 0x0001008A } },
	{ VENC_FID_INT_OFFSET_Y_FID_EXT_START_X,{ 0x01AC0106, 0x002E0138 } },
	{ VENC_FID_EXT_START_Y_FID_EXT_OFFSET_Y,{ 0x01060006, 0x01380001 } },
	{ VENC_TVDETGP_INT_START_STOP_X, 	{ 0x00140001, 0x00140001 } },
	{ VENC_TVDETGP_INT_START_STOP_Y, 	{ 0x00010001, 0x00010001 } },
	{ VENC_GEN_CTRL, 		{ 0x00F90000, 0x00FF0000 } },
	{ VENC_OUTPUT_CONTROL, 		{ 0x0000000D, 0x0000000D } },	// s-video
	//{ VENC_OUTPUT_CONTROL, 		{ 0x0000000A, 0x0000000A } },	// composite
	{ VENC_OUTPUT_TEST, 		{ 0x00000000, 0x00000000 } },

	// S15.5.8.3 "The DSS.VENC_F_CONTROL and DSS.VENC_SYNC_CTRL registers must be
	//            the last ones to be changed by software."
	//{ VENC_F_CONTROL, 		{ 0x00000000, 0x00000000 } },
	{ VENC_SYNC_CTRL,	 	{ 0x00008040, 0x00000040 } },
	{ -1 }
};

// init video encoder.
// by default just show test pattern
// This doesn't work
void omap_venc_init(int mode) {
	void *VENC = VENC_BASE;
	int i;

	Write32(VENC, VENC_F_CONTROL, VENC_RESET);
	dprintf("venc reset\n");
	while (Read32(VENC, VENC_F_CONTROL) & VENC_RESET)
		dprintf("waiting venc reset\n");

	for (i=0;venc_setup[i].reg != -1;i++)
		Write32(VENC, venc_setup[i].reg, venc_setup[i].mode[mode]);

	// Set digital size to match tv standard
	if (mode == VENC_MODE_PAL)
		Write32(DISPC_BASE, DISPC_SIZE_DIG, (286 << 16) | 719);
	else
		Write32(DISPC_BASE, DISPC_SIZE_DIG, (239 << 16) | 719);

	// set a test pattern only
	Write32(VENC, VENC_F_CONTROL, VENC_SVDS_COLOURBAR | (7<<2));
}

// init beagle gpio for video
void omap_beagle_init(void) {
	// setup GPIO stuff, i can't find any references to these
	Write32(GPIO1_BASE, GPIO_OE, 0xfefffedf);
	Write32(GPIO1_BASE, GPIO_SETDATAOUT, 0x01000120);
}

void omap_clock_init(void) {
	// sets pixel clock to 72MHz

	// sys_clk = 26.0 Mhz
	// DPLL4 = sys_clk * 432 / 13 = 864
	// DSS1_ALWON_FCLK = 864 / 6 = 144
	// Pixel clock (DISPC_DIVISOR) = 144 / 2 = 72Mhz
	// and also VENC clock = 864 / 16 = 54MHz

	// The clock multiplier/divider cannot be changed
	// without affecting other system clocks - do don't.

	// pll4 clock multiplier/divider
	Write32(CM_CLOCK_BASE, CM_CLKSEL2_PLL, (432 << 8) | 12);
	// tv clock divider, dss1 alwon fclk divider
	Write32(CM_DSS_BASE, CM_CLKSEL_DSS, (16 << 8) | 6);
	// core/peripheral PLL to 1MHz
	Write32(CM_CLOCK_BASE, CM_CLKEN_PLL, 0x00370037);
}

void omap_dss_init(void) {
	Write32(DSS_BASE, DSS_SYSCONFIG, DSS_AUTOIDLE);
	// Select DSS1 ALWON as clock source
	Write32(DSS_BASE, DSS_CONTROL, DSS_VENC_OUT_SEL | DSS_DAC_POWERDN_BGZ | DSS_DAC_DEMEN | DSS_VENC_CLOCK_4X_ENABLE);
}

void omap_dispc_init(void) {
	void *DISPC = DISPC_BASE;

	Write32(DISPC, DISPC_SYSCONFIG,
		DISPC_MIDLEMODE_SMART
		| DISPC_SIDLEMODE_SMART
		| DISPC_ENWAKEUP
		| DISPC_AUTOIDLE);

	Write32(DISPC, DISPC_CONFIG, DISPC_LOADMODE_FRAME);

	// LCD default = CBM blue
	Write32(DISPC, DISPC_DEFAULT_COLOR0, 0x7c70da);
	// digital default = black
	Write32(DISPC, DISPC_DEFAULT_COLOR1, 0x000000);

	Write32(DISPC, DISPC_TRANS_COLOR0, 0x000000);
	Write32(DISPC, DISPC_TRANS_COLOR1, 0x000000);

	Write32(DISPC, DISPC_POL_FREQ,
		DISPC_POL_IPC
		| DISPC_POL_IHS
		| DISPC_POL_IVS
		| (2<<DISPCB_POL_ACBI)
		| (8<<DISPCB_POL_ACB));

	// Set pixel clock divisor = 2
	Write32(DISPC, DISPC_DIVISOR,
		(1<<DISPCB_DIVISOR_LCD)
		| (2<<DISPCB_DIVISOR_PCD));

	// Disable graphical output
	Write32(DISPC, DISPC_GFX_ATTRIBUTES, 0);

	// Set palette / gamma table address
	//Write32(DISPC, DISPC_GFX_TABLE_BA, xx);

	// Set colour conversion coefficients for YUV modes
	// this is BT.601-5, see Table 15-47  S 15.5.3.3
	Write32(DISPC, DISPC_VID1_CONV_COEF0, 296 | (409 << 16));
	Write32(DISPC, DISPC_VID1_CONV_COEF1, 0 | (298 << 16));
	Write32(DISPC, DISPC_VID1_CONV_COEF2, ((-208) & 0xffff) | (-100 << 16));
	Write32(DISPC, DISPC_VID1_CONV_COEF3, 298 | (0<<16));
	Write32(DISPC, DISPC_VID1_CONV_COEF4, 517);

	Write32(DISPC, DISPC_VID2_CONV_COEF0, 296 | (409 << 16));
	Write32(DISPC, DISPC_VID2_CONV_COEF1, 0 | (298 << 16));
	Write32(DISPC, DISPC_VID2_CONV_COEF2, ((-208) & 0xffff) | (-100 << 16));
	Write32(DISPC, DISPC_VID2_CONV_COEF3, 298 | (0<<16));
	Write32(DISPC, DISPC_VID2_CONV_COEF4, 517);

	// Turn on the LCD data
	Write32(DISPC, DISPC_CONTROL,
		DISPC_GPOUT1
		| DISPC_GPOUT0
		| DISPC_TFTDATALINES_24
		| DISPC_STDITHERENABLE
		| DISPC_GOLCD
		| DISPC_STNTFT
		| DISPC_LCDENABLE
		);

	while ((Read32(DISPC, DISPC_CONTROL) & DISPC_GOLCD))
		;
}

void omap_set_lcd_mode(int w, int h) {
	void *DISPC = DISPC_BASE;
	int i;
	struct video_mode *m;

	dprintf("omap3: set_lcd_mode %d,%d\n", w, h);

	for (i=0;i<sizeof(modes)/sizeof(modes[0]);i++) {
		if (w <= modes[i].width 
		    && h <= modes[i].height)
		goto found;
	}
	i -= 1;
found:
	m = &modes[i];
	
	dprintf("omap3: found mode[%s]\n", m->name);

	Write32(DISPC, DISPC_SIZE_LCD, m->dispc_size);
	Write32(DISPC, DISPC_TIMING_H, m->dispc_timing_h);
	Write32(DISPC, DISPC_TIMING_V, m->dispc_timing_v);

	Set32(DISPC, DISPC_DIVISOR, 0xffff, m->dispc_divisor);
	Set32(CM_DSS_BASE, CM_CLKSEL_DSS, 0xffff, m->dss_divisor);

	// Tell hardware to update, and wait for it
	Set32(DISPC, DISPC_CONTROL,
	      DISPC_GOLCD,
	      DISPC_GOLCD);

	while ((Read32(DISPC, DISPC_CONTROL) & DISPC_GOLCD))
		;
}

void omap_disable_display(int vid) {
	switch (vid) {
	case 0:
		Set32(DISPC_BASE, DISPC_GFX_ATTRIBUTES, DISPC_GFXENABLE, 0);
		break;
	case 1:
		Set32(DISPC_BASE, DISPC_VID1_ATTRIBUTES, DISPC_VIDENABLE, 0);
		break;
	case 2:
		Set32(DISPC_BASE, DISPC_VID2_ATTRIBUTES, DISPC_VIDENABLE, 0);
		break;
	}
}

// set a bitmap on the given video source
// id: 0 = gfx, 1 = vid1, 2 = vid2
// id: (1<<8) = set digital (tv) out
void omap_attach_framebuffer(int id, struct BitMap *bm) {
	void *DISPC = DISPC_BASE;
	uint32 vsize, attr = 0, gobit;
	uint32 rowinc = 1;
	uint32 data0 = (uint32)bm->data;
	uint32 data1 = data0;

	dprintf("omap3: attach bitmap (%d,%d) 0x%08x to channel %d\n", bm->width, bm->height, bm->data, id);

	// FIXME: other formats than RGB16 taken from bitmap
	// FIXME: add a scale-to-fit bit for video overlays?

	if (id & VID_TVOUT) {
		vsize = Read32(DISPC, DISPC_SIZE_DIG);
		gobit = DISPC_GODIGITAL | DISPC_DIGITALENABLE;
		attr = id > VID_TVOUT ? DISPC_VIDCHANNELOUT : DISPC_GFXCHANNELOUT;
		// turn off test pattern
		Set32(VENC_BASE, VENC_F_CONTROL, VENC_SVDS_MASK, VENC_SVDS_EXTERNAL);
		// data is interlaced
		rowinc += bm->stride;
		data0 += bm->stride;
	} else {
		vsize = Read32(DISPC, DISPC_SIZE_LCD);
		gobit = DISPC_GOLCD;
		attr = 0;
	}

	// Centre graphics on display for now
	uint32 vheight = (vsize >> 16) + 1;
	uint32 vwidth = (vsize & 0xffff) + 1;
	uint32 dheight = bm->height;
	uint32 dwidth = bm->width;

	if (dheight > vheight)
		dheight = vheight;
	if (dwidth > vwidth) {
		// FIXME: adjust for data format width
		rowinc += bm->stride - vwidth*2;
		dwidth = vwidth;
	}

	uint32 pos = (((vheight - dheight) / 2) << 16)
		| (vwidth - dwidth) / 2;
	uint32 gsize = ((dheight - 1) << 16)
		| (dwidth - 1);

	dprintf("ompa3: display offset %08x\n", pos);
	dprintf("ompa3: display size   %08x\n", gsize);
	dprintf("ompa3: display rowinc %08x\n", rowinc);

	switch (id & 0x0f) {
	case 0:
		Write32(DISPC, DISPC_GFX_BA0, data0);
		Write32(DISPC, DISPC_GFX_BA1, data1);
		Write32(DISPC, DISPC_GFX_POSITION, pos);
		Write32(DISPC, DISPC_GFX_SIZE, gsize);
		Write32(DISPC, DISPC_GFX_FIFO_THRESHOLD, (0x3ff << 16) | 0x3c0);
		Write32(DISPC, DISPC_GFX_ROW_INC, rowinc);
		Write32(DISPC, DISPC_GFX_PIXEL_INC, 1);
		Write32(DISPC, DISPC_GFX_WINDOW_SKIP, 0);
		Write32(DISPC, DISPC_GFX_ATTRIBUTES,
			DISPC_GFXFORMAT_RGB16
			| attr
			| DISPC_GFXBURSTSIZE_16x32
			| DISPC_GFXENABLE);
		break;
	case 1:
		Write32(DISPC, DISPC_VID1_BA0, data0);
		Write32(DISPC, DISPC_VID1_BA1, data1);
		Write32(DISPC, DISPC_VID1_POSITION, pos);
		Write32(DISPC, DISPC_VID1_SIZE, gsize);
		Write32(DISPC, DISPC_VID1_PICTURE_SIZE, gsize);
		Write32(DISPC, DISPC_VID1_FIFO_THRESHOLD, (0x3ff << 16) | 0x3c0);
		Write32(DISPC, DISPC_VID1_ROW_INC, rowinc);
		Write32(DISPC, DISPC_VID1_PIXEL_INC, 1);

		Write32(DISPC, DISPC_VID1_ATTRIBUTES,
			//DISPC_VIDFORMAT_RGB16
			DISPC_VIDFORMAT_UYVY
			| attr
			| DISPC_VIDBURSTSIZE_16x32
			| DISPC_VIDENABLE);
		break;
	case 2:
		Write32(DISPC, DISPC_VID2_BA0, data0);
		Write32(DISPC, DISPC_VID2_BA1, data1);
		Write32(DISPC, DISPC_VID2_POSITION, pos);
		Write32(DISPC, DISPC_VID2_SIZE, gsize);
		Write32(DISPC, DISPC_VID2_PICTURE_SIZE, gsize);
		Write32(DISPC, DISPC_VID2_FIFO_THRESHOLD, (0x3ff << 16) | 0x3c0);
		Write32(DISPC, DISPC_VID2_ROW_INC, rowinc);
		Write32(DISPC, DISPC_VID2_PIXEL_INC, 1);
		Write32(DISPC, DISPC_VID2_ATTRIBUTES,
			DISPC_VIDFORMAT_RGB16
			| attr
			| DISPC_VIDBURSTSIZE_16x32
			| DISPC_VIDENABLE);
		break;
	}

	// Tell hardware to update, and wait for it
	Set32(DISPC, DISPC_CONTROL, gobit, gobit);

	gobit &= DISPC_GODIGITAL | DISPC_GOLCD;

	while ((Read32(DISPC, DISPC_CONTROL) & gobit))
		;
}

void video_init(int width, int height) {
	dprintf("omap3: video_init()\n");

	Write32(DISPC_BASE, DISPC_IRQENABLE, 0x00000);
	Write32(DISPC_BASE, DISPC_IRQSTATUS, 0x1ffff);

	omap_beagle_init();
	omap_clock_init();
	omap_dss_init();

	omap_venc_init(VENC_MODE_PAL);
	omap_dispc_init();

	omap_set_lcd_mode(width, height);
}
