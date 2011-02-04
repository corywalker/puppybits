/*  omap3-video.h: OMAP3 Video registers

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
#ifndef _OMAP3_VIDEO_H
#define _OMAP3_VIDEO_H

#include "omap3-gpio.h"

/* OMAP video related registers */

/* ********************************************************************** */
/* Clock management registers - to be moved elsewhere */

#define CM_DSS_BASE ((void *)0x48004E00)

#define CM_FCLKEN_DSS 0x00000000
#define CM_ICLKEN_DSS 0x00000010
#define CM_IDLEST_DSS 0x00000020
#define CM_AUTOIDLE_DSS 0x00000030
#define CM_CLKSEL_DSS 0x00000040
#define CM_SLEEPDEP_DSS 0x00000044
#define CM_CLKSTCTRL_DSS 0x00000048
#define CM_CLKSTST_DSS 0x0000004C

#define CM_DSS_EN_TV (1<<2)
#define CM_DSS_EN_DSS1 (1<<1)
#define CM_DSS_EN_DSS2 (1<<0)

#define CM_CLOCK_BASE ((void *)0x48004D00)

#define CM_CLKEN_PLL 0x00000000
#define CM_CLKEN2_PLL 0x00000004
#define CM_IDLEST_CKGEN 0x00000020
#define CM_IDLEST2_CKGEN 0x00000024
#define CM_AUTOIDLE_PLL 0x00000030
#define CM_AUTOIDLE2_PLL 0x00000034
#define CM_CLKSEL1_PLL 0x00000040
#define CM_CLKSEL2_PLL 0x00000044
#define CM_CLKSEL3_PLL 0x00000048
#define CM_CLKSEL4_PLL 0x0000004C
#define CM_CLKSEL5_PLL 0x00000050
#define CM_CLKOUT_CTRL 0x00000070

/* ********************************************************************** */
/* S 1.7.2 Display Subsystem & SDI Registers */

#define DSS_BASE ((void *)0x48050000)

#define DSS_SYSCONFIG 0x010
#define DSS_SYSSTATUS 0x014
#define DSS_IRQSTATUS 0x018
#define DSS_CONTROL 0x040
#define DSS_SDI_CONTROL 0x044
#define DSS_PLL_CONTROL 0x048
#define DSS_SDI_STATUS 0x05C

/* SYSCONFIG bits */
#define DSS_AUTOIDLE 1
#define DSS_SOFTRESET 2
/* SYSSTATUS bits */
#define DSS_RESETDONE 1
/* IRQSTATUS */
#define DSS_DSI_IRQ 2
#define DSS_DISPC_IRQ 1
/* CONTROL bits */
#define DSS_VENC_OUT_SEL (1<<6)	/* set s-video */
#define DSS_DAC_POWERDN_BGZ (1<<5)
#define DSS_DAC_DEMEN (1<<4)
#define DSS_VENC_CLOCK_4X_ENABLE (1<<3)
#define DSS_VENC_CLOCK_MODE (1<<2) /* set square pixel */
#define DSS_DSI_CLK_SWITCH (1<<1) /* select DSI2 PLL FCLK otherwise DSS1 ALWON FCLK */
#define DSS_DSS1_CLK_SWITCH (1<<0) /* same */

/* ********************************************************************** */
/* S 1.7.3 Display controller */

#define DISPC_BASE ((void *)0x48050400)

#define DISPC_SYSCONFIG 0x010
#define DISPC_SYSSTATUS 0x014
#define DISPC_IRQSTATUS 0x018
#define DISPC_IRQENABLE 0x01C
#define DISPC_CONTROL 0x040
#define DISPC_CONFIG 0x044
#define DISPC_DEFAULT_COLOR0 0x04c
#define DISPC_DEFAULT_COLOR1 0x050
#define DISPC_TRANS_COLOR0 0x54
#define DISPC_TRANS_COLOR1 0x58
#define DISPC_LINE_STATUS 0x05C
#define DISPC_LINE_NUMBER 0x060
#define DISPC_TIMING_H 0x064
#define DISPC_TIMING_V 0x068
#define DISPC_POL_FREQ 0x06C
#define DISPC_DIVISOR 0x070
#define DISPC_GLOBAL_ALPHA 0x074
#define DISPC_SIZE_DIG 0x078
#define DISPC_SIZE_LCD 0x07C

#define DISPC_GFX_BA0 0x080
#define DISPC_GFX_BA1 0x084
#define DISPC_GFX_POSITION 0x088
#define DISPC_GFX_SIZE 0x08C
#define DISPC_GFX_ATTRIBUTES 0x0A0
#define DISPC_GFX_FIFO_THRESHOLD 0x0A4
#define DISPC_GFX_FIFO_SIZE_STATUS 0x0A8
#define DISPC_GFX_ROW_INC 0x0AC
#define DISPC_GFX_PIXEL_INC 0x0B0
#define DISPC_GFX_WINDOW_SKIP 0x0B4
#define DISPC_GFX_TABLE_BA 0x0B8

#define DISPC_VID1_BA0 0x0BC
#define DISPC_VID1_BA1 0x0C0
#define DISPC_VID2_BA0 0x14C
#define DISPC_VID2_BA1 0x150

#define DISPC_VID1_POSITION 0x0C4
#define DISPC_VID2_POSITION (0x0C4+0x90)
#define DISPC_VID1_SIZE 0x0C8
#define DISPC_VID2_SIZE (0x0C8+0x90)
#define DISPC_VID1_ATTRIBUTES 0x0CC
#define DISPC_VID2_ATTRIBUTES (0x0CC+0x90)

#define DISPC_VID1_FIFO_THRESHOLD 0x0D0
#define DISPC_VID2_FIFO_THRESHOLD (0x0D0+0x90)
#define DISPC_VID1_FIFO_SIZE_STATUS 0x0D4
#define DISPC_VID2_FIFO_SIZE_STATUS (0x0D4+0x90)
#define DISPC_VID1_ROW_INC 0x0D8
#define DISPC_VID2_ROW_INC (0x0D8+0x90)
#define DISPC_VID1_PIXEL_INC 0x0DC
#define DISPC_VID2_PIXEL_INC (0x0DC+0x90)
/*
#define DISPC_VIDn_FIR 0x0E0+((n-1)*
*/
#define DISPC_VID1_PICTURE_SIZE 0x0E4
#define DISPC_VID2_PICTURE_SIZE (0x0E4+0x90)
/*
#define DISPC_VIDn_ACCUl 0x0E8
#define DISPC_VIDn_FIR_COEF_Hi 0x0F0+
#define DISPC_VIDn_FIR_COEF_HVi 0x0F4+
*/
#define DISPC_VID1_CONV_COEF0 0x130
#define DISPC_VID2_CONV_COEF0 (0x130+0x90)
#define DISPC_VID1_CONV_COEF1 0x134
#define DISPC_VID2_CONV_COEF1 (0x134+0x90)
#define DISPC_VID1_CONV_COEF2 0x138
#define DISPC_VID2_CONV_COEF2 (0x138+0x90)
#define DISPC_VID1_CONV_COEF3 0x13C
#define DISPC_VID2_CONV_COEF3 (0x13C+0x90)
#define DISPC_VID1_CONV_COEF4 0x140
#define DISPC_VID2_CONV_COEF4 (0x140+0x90)
/*
#define DISPC_DATA_CYCLEk 0x1D4+((k-1)*
#define DISPC_VIDn_FIR_COEF_Vi 0x1E0+
*/
#define DISPC_CPR_COEF_R 0x220
#define DISPC_CPR_COEF_G 0x224
#define DISPC_CPR_COEF_B 0x228
#define DISPC_GFX_PRELOAD 0x22C
#define DISPC_VID1_PRELOAD 0x230
#define DISPC_VID2_PRELOAD 0x234

/* DISPC_SYSCONFIG bits */
#define DISPC_MIDLEMODE_FORCE (0<<12)
#define DISPC_MIDLEMODE_NEVER (1<<12)
#define DISPC_MIDLEMODE_SMART (2<<12)

#define DISPC_SIDLEMODE_FORCE (0<<3)
#define DISPC_SIDLEMODE_NEVER (1<<3)
#define DISPC_SIDLEMODE_SMART (2<<3)

#define DISPC_ENWAKEUP (1<<2)
#define DISPC_SOFTRESET (1<<1)
#define DISPC_AUTOIDLE (1<<0)

/* DISPC_SYSSTATUS bits */
#define DISPC_RESETDONE 1

/* TODO: rest of IRQ bits */
#define DISPC_VSYNC 2
#define DISPC_FRAMEDONE 1

/* DISPC_CONTROL bits (incomplete) */
#define DISPC_DITHER_FRAMES_0 (0<<30)
#define DISPC_DITHER_FRAMES_2 (1<<30)
#define DISPC_DITHER_FRAMES_4 (2<<30)
#define DISPC_LCDENABLEPOL (1<<29)
#define DISPC_LCDENABLESIGNAL (1<<28)
#define DISPC_PCKFREEENABLE (1<<27)
#define DISPC_GPOUT1 (1<<16)
#define DISPC_GPOUT0 (1<<15)
#define DISPC_GPIN1 (1<<14)
#define DISPC_GPIN0 (1<<13)
#define DISPC_OVERLAYOPTIMISATION (1<<12)
#define DISPC_STALLMODE (1<<11)
#define DISPC_TFTDATALINES_12 (0<<8)
#define DISPC_TFTDATALINES_16 (1<<8)
#define DISPC_TFTDATALINES_18 (2<<8)
#define DISPC_TFTDATALINES_24 (3<<8)
#define DISPC_STDITHERENABLE (1<<7)
#define DISPC_GODIGITAL (1<<6)
#define DISPC_GOLCD (1<<5)
#define DISPC_M8B (1<<4)
#define DISPC_STNTFT (1<<3)
#define DISPC_MONOCOLOR (1<<2)
#define DISPC_DIGITALENABLE (1<<1)
#define DISPC_LCDENABLE (1<<0)

/* DISPC_CONFIG bits */
#define DISPC_TVALPHABLENDERENABLE (1<<19)
#define DISPC_LCDALPHABLENDERENABLE (1<<18)
#define DISPC_FIFOFILLING (1<<17)
#define DISPC_FIFOHANDCHECK (1<<16)
#define DISPC_CPR (1<<15)
#define DISPC_FIFOMERGE (1<<14)
#define DISPC_TCKDIGSELECTION (1<<13)
#define DISPC_TCKDIGENABLE (1<<12)
#define DISPC_TCKLCDSELECTION (1<<11)
#define DISPC_TCKLCDENABLE (1<<10)
#define DISPC_FUNCGATED (1<<9)
#define DISPC_ACBIASGATED (1<<8)
#define DISPC_VSYNCGATED (1<<7)
#define DISPC_HSYNCGATED (1<<6)
#define DISPC_PIXELCLOCKGATED (1<<5)
#define DISPC_PIXELDATAGATED (1<<4)
#define DISPC_PALETTEGAMMATABLE (1<<3)
#define DISPC_LOADMODE_EVERY (0<<1)
#define DISPC_LOADMODE_MANUAL (1<<1)
#define DISPC_LOADMODE_FRAME (2<<1)
#define DISPC_LOADMODE_FIRST (3<<1)
#define DISPC_PIXELGATED 1

/* DISPC_TIMING_H bits and masks */
#define DISPCB_HBP 20
#define DISPCB_HFP 8
#define DISPCB_HSW 0
#define DISPCM_HBP 0xfff00000
#define DISPCM_HFP 0x000fff00
#define DISPCM_HSW 0x000000ff

/* DISPC_TIMING_V bits and masks */
#define DISPCB_VBP 20
#define DISPCB_VFP 8
#define DISPCB_VSW 0
#define DISPCM_VBP 0x0ff00000
#define DISPCM_VFP 0x0000ff00
#define DISPCM_VSW 0x0000003f

/* DISPC_POL_FREQ bits */
#define DISPC_POL_ONOFF (1<<17)
#define DISPC_POL_RF (1<<16)
#define DISPC_POL_IEO (1<<15)
#define DISPC_POL_IPC (1<<14)
#define DISPC_POL_IHS (1<<13)
#define DISPC_POL_IVS (1<<12)
#define DISPCB_POL_ACBI 8
#define DISPCM_POL_ACBI 0xf00
#define DISPCB_POL_ACB 0
#define DISPCM_POL_ACB 0xff

/* DISPC_DIVISOR bits */
#define DISPCB_DIVISOR_LCD 16
#define DISPCB_DIVISOR_PCD 0

/* DISPC_GFX_ATTRIBUTES bits */
#define DISPC_GFXSELFREFRESH (1<<15)
#define DISPC_GFXARBITRATION (1<<14)
#define DISPC_GFXROTATION_0 (0<<12)
#define DISPC_GFXROTATION_90 (1<<12)
#define DISPC_GFXROTATION_180 (2<<12)
#define DISPC_GFXROTATION_270 (3<<12)
#define DISPC_GFXFIFOPRELOAD (1<<11)
#define DISPC_GFXENDIANNESS (1<<10)
#define DISPC_GFXNIBBLEMODE (1<<9)
#define DISPC_GFXCHANNELOUT (1<<8)
#define DISPC_GFXBURSTSIZE_4x32 (0<<6)
#define DISPC_GFXBURSTSIZE_8x32 (1<<6)
#define DISPC_GFXBURSTSIZE_16x32 (2<<6)
#define DISPC_GFXREPLICATIONENABLE (1<<5)
#define DISPC_GFXFORMAT_BITMAP1 (0<<1)
#define DISPC_GFXFORMAT_BITMAP2 (1<<1)
#define DISPC_GFXFORMAT_BITMAP4 (2<<1)
#define DISPC_GFXFORMAT_BITMAP8 (3<<1)
#define DISPC_GFXFORMAT_RGB12 (4<<1)
#define DISPC_GFXFORMAT_ARGB16 (5<<1)
#define DISPC_GFXFORMAT_RGB16 (6<<1)
#define DISPC_GFXFORMAT_RGB32 (8<<1) /* 24 bit, 32-bit un - packed */
#define DISPC_GFXFORMAT_RGB24 (9<<1)
#define DISPC_GFXFORMAT_ARGB32 (12<<1)
#define DISPC_GFXFORMAT_RGBA32 (13<<1)
#define DISPC_GFXFORMAT_RGBx (14<<1)
#define DISPC_GFXENABLE 1

/* DISPC_VIDn_ATTRIBUTES bits */
#define DISPC_VIDSELFREFRESH (1<<24)
#define DISPC_VIDARBITRATION (1<<23)
#define DISPC_VIDLINEBUFFERSPLOT (1<<22)
#define DISPC_VIDVERTICALTAPS (1<<21)
#define DISPC_VIDOPTIMISATION (1<<20)
#define DISPC_VIDFIFOPRELOAD (1<<19)
#define DISPC_VIDROWREPEATENABLE (1<<18)
#define DISPC_VIDENDIANNESS (1<<17)
#define DISPC_VIDCHANNELOUT (1<<16)
#define DISPC_VIDBURSTSIZE_4x32 (0<<14)
#define DISPC_VIDBURSTSIZE_8x32 (1<<14)
#define DISPC_VIDBURSTSIZE_16x32 (2<<14)
#define DISPC_VIDROTATION_0 (0<<12)
#define DISPC_VIDROTATION_90 (1<<12)
#define DISPC_VIDROTATION_180 (2<<12)
#define DISPC_VIDROTATION_270 (3<<12)
#define DISPC_VIDFULLRANGE (1<<11)
#define DISPC_VIDREPLICATIONENABLE (1<<10)
#define DISPC_VIDCOLORCONVENABLE (1<<9)
#define DISPC_VIDVRESIZECONF (1<<8)
#define DISPC_VIDHRESIZECONF (1<<7)
#define DISPC_VIDRESIZEENABLE_H (1<<5)
#define DISPC_VIDRESIZEENABLE_V (2<<5)
#define DISPC_VIDRESIZEENABLE_HV (3<<5)
// Only VID2 supports the alpha formats
#define DISPC_VIDFORMAT_RGB12 (4<<1)
#define DISPC_VIDFORMAT_ARGB16 (5<<1)
#define DISPC_VIDFORMAT_RGB16 (6<<1)
#define DISPC_VIDFORMAT_RGB32 (8<<1) /* 24 bit, 32-bit un - packed */
#define DISPC_VIDFORMAT_RGB24 (9<<1)
#define DISPC_VIDFORMAT_YUV2 (10<<1)
#define DISPC_VIDFORMAT_UYVY (11<<1)
#define DISPC_VIDFORMAT_ARGB32 (12<<1)
#define DISPC_VIDFORMAT_RGBA32 (13<<1)
#define DISPC_VIDFORMAT_RGBx (14<<1)
#define DISPC_VIDENABLE 1

/* TODO: video attributes, etc */

/* ********************************************************************** */
/* S 1.7.5 Video Encoder Registers */

#define VENC_BASE ((void *)0x48050c00)

#define VENC_STATUS 0x04
#define VENC_F_CONTROL 0x08
#define VENC_VIDOUT_CTRL 0x10
#define VENC_SYNC_CTRL 0x14
#define VENC_LLEN 0x1C
#define VENC_FLENS 0x20
#define VENC_HFLTR_CTRL 0x24
#define VENC_CC_CARR_WSS_CARR 0x28
#define VENC_C_PHASE 0x2C
#define VENC_GAIN_U 0x30
#define VENC_GAIN_V 0x34
#define VENC_GAIN_Y 0x38
#define VENC_BLACK_LEVEL 0x3C
#define VENC_BLANK_LEVEL 0x40
#define VENC_X_COLOR 0x44
#define VENC_M_CONTROL 0x48
#define VENC_BSTAMP_WSS_DATA 0x4C
#define VENC_S_CARR 0x50
#define VENC_LINE21 0x54
#define VENC_LN_SEL 0x58
#define VENC_L21_WC_CTL 0x5C
#define VENC_HTRIGGER_VTRIGGER 0x60
#define VENC_SAVID_EAVID 0x64
#define VENC_FLEN_FAL 0x68
#define VENC_LAL_PHASE_RESET 0x6C
#define VENC_HS_INT_START_STOP_X 0x70
#define VENC_HS_EXT_START_STOP_X 0x74
#define VENC_VS_INT_START_X 0x78
#define VENC_VS_INT_STOP_X_VS_INT_START_Y 0x7C
#define VENC_VS_INT_STOP_Y_VS_EXT_START_X 0x80
#define VENC_VS_EXT_STOP_X_VS_EXT_START_Y 0x84
#define VENC_VS_EXT_STOP_Y 0x88
#define VENC_AVID_START_STOP_X 0x90
#define VENC_AVID_START_STOP_Y 0x94
#define VENC_FID_INT_START_X_FID_INT_START_Y 0xA0
#define VENC_FID_INT_OFFSET_Y_FID_EXT_START_X 0xA4
#define VENC_FID_EXT_START_Y_FID_EXT_OFFSET_Y 0xA8
#define VENC_TVDETGP_INT_START_STOP_X 0xB0
#define VENC_TVDETGP_INT_START_STOP_Y 0xB4
#define VENC_GEN_CTRL 0xB8
#define VENC_OUTPUT_CONTROL 0xC4
#define VENC_OUTPUT_TEST 0xC8

/* VENC_F_CONTROL bits */
#define VENC_RESET (1<<8)
#define VENC_SVDS_EXTERNAL (0<<6)
#define VENC_SVDS_COLOURBAR (1<<6)
#define VENC_SVDS_SOLID (2<<6)
#define VENC_SVDS_MASK (3<<6)
#define VENC_RGBF (1<<5)
#define VENC_BCOLOR_BLACK (0<<2)
#define VENC_BCOLOR_BLUE (1<<2)
#define VENC_BCOLOR_RED (2<<2)
#define VENC_BCOLOR_MAGENTA (3<<2)
#define VENC_BCOLOR_GREEN (4<<2)
#define VENC_BCOLOR_CYAN (5<<2)
#define VENC_BCOLOR_YELLOW (6<<2)
#define VENC_BCOLOR_WHITE (7<<2)
#define VENC_FMT_24_RGB444 0
#define VENC_FMT_24_444 1
#define VENC_FMT_16_422 2
#define VENC_FMT_8_422 2

/* VENC_VIDOUT_CTRL bits */
#define VENC_VIDOUT_54 (0)
#define VENC_VIDOUT_27 (1)

/* VENC_SYNC_CTRL bits */
#define VENC_SYNC_FREE (1<<15)
#define VENC_SYNC_ESAV (1<<14)
#define VENC_SYNC_IGNP (1<<13)
#define VENC_SYNC_NBLNKS (1<<12)
#define VENC_SYNC_VBLKM_DFL (0<<10)
#define VENC_SYNC_VBLKM_LAL (1<<10)
#define VENC_SYNC_HBLKM_DFL (0<<10)
#define VENC_SYNC_HBLKM_EAV (1<<10)
#define VENC_SYNC_HBLKM_RANGE (2<<10)
#define VENC_SYNC_FID_POL (1<<6)

/* VENC_HFLTR_CTRL bits */
#define VENC_CINTP_EN (0<<1)
#define VENC_CINTP_BYPASS_1 (1<<1)
#define VENC_CINTP_BYPASS_2 (2<<1)
#define VENC_CINTP_BYPASS (3<<1)

#define VENC_YINTP_EN (0<<0)
#define VENC_YINTP_BYPASS (1<<0)

/* VENC_CC_CARR_WSS_CARR bits */
#define VENCB_FWSS 16
#define VENCB_FCC 0

/* VENC_X_COLOR bits */
#define VENC_XCE (1<<6)
#define VENC_XCBW_32_8 (0<<3)
#define VENC_XCBW_26_5 (1<<3)
#define VENC_XCBW_30_0 (2<<3)
#define VENC_XCBW_29_2 (3<<3)
#define VENC_LCD_0_0 0
#define VENC_LCD_0_5 1
#define VENC_LCD_1_0 2
#define VENC_LCD_1_5 3
#define VENC_LCD_m2_0 4
#define VENC_LCD_m1_5 5
#define VENC_LCD_m1_0 6
#define VENC_LCD_m0_5 7

/* VENC_M_CONTROL bits */
#define VENC_M_PALI (1<<7)
#define VENC_M_PALN (1<<6)
#define VENC_M_PALPHS (1<<5)
#define VENC_M_CBW_21_8 (0<<2)
#define VENC_M_CBW_19_8 (1<<2)
#define VENC_M_CBW_18_0 (2<<2)
#define VENC_M_CBW_23_7 (5<<2)
#define VENC_M_CBW_26_8 (6<<2)
#define VENC_M_CBW_BYPASS (7<<2)
#define VENC_M_PAL (1<<1)
#define VENC_M_NTSC (1<<0)

/* VENC_BSTAMP_WSS_DATA bits */
#define VENCB_WSS_DATA (8)
#define VENC_SQP (1<<7)

/* VENC_S_CARR values - presets assuming Fclkenc = 27Mhz */
#define VENC_FSC_R601_NTSC_M 0x21F07C1F
#define VENC_FSC_R601_PAL_M 0x21E6EFE3
#define VENC_FSC_R601_PAL_G 0x2A098ACB
#define VENC_FSC_R601_PAL_Nc 0x21E6EFE3

/* ********************************************************************** */
/* S 1.7.6 DSI registers */
#define DSI_BASE 0x4804FC00

// *incomplete*

#define DSI_IRQSTATUS 0x18
#define DSI_IRQENABLE 0x1c

#endif
