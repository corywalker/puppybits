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
#ifndef _OMAP3_MCBSP_H
#define _OMAP3_MCBSP_H

// McBSP bases
#define MCBSP1_BASE          0x48074000
#define MCBSP5_BASE          0x48096000
#define MCBSP2_BASE          0x49022000
#define MCBSP3_BASE          0x49024000
#define MCBSP4_BASE          0x49026000
#define SIDETONE_McBSP2_BASE 0x49028000
#define SIDETONE_McBSP3_BASE 0x4902A000

// register offsets
#define MCBSPLP_DRR_REG 0x00000000
#define MCBSPLP_DXR_REG 0x00000008
#define MCBSPLP_SPCR2_REG 0x00000010
#define MCBSPLP_SPCR1_REG 0x00000014
#define MCBSPLP_RCR2_REG 0x00000018
#define MCBSPLP_RCR1_REG 0x0000001C
#define MCBSPLP_XCR2_REG 0x00000020
#define MCBSPLP_XCR1_REG 0x00000024
#define MCBSPLP_SRGR2_REG 0x00000028
#define MCBSPLP_SRGR1_REG 0x0000002C
#define MCBSPLP_MCR2_REG 0x00000030
#define MCBSPLP_MCR1_REG 0x00000034
#define MCBSPLP_RCERA_REG 0x00000038
#define MCBSPLP_RCERB_REG 0x0000003C
#define MCBSPLP_XCERA_REG 0x00000040
#define MCBSPLP_XCERB_REG 0x00000044
#define MCBSPLP_PCR_REG 0x00000048
#define MCBSPLP_RCERC_REG 0x0000004C
#define MCBSPLP_RCERD_REG 0x00000050
#define MCBSPLP_XCERC_REG 0x00000054
#define MCBSPLP_XCERD_REG 0x00000058
#define MCBSPLP_RCERE_REG 0x0000005C
#define MCBSPLP_RCERF_REG 0x00000060
#define MCBSPLP_XCERE_REG 0x00000064
#define MCBSPLP_XCERF_REG 0x00000068
#define MCBSPLP_RCERG_REG 0x0000006C
#define MCBSPLP_RCERH_REG 0x00000070
#define MCBSPLP_XCERG_REG 0x00000074
#define MCBSPLP_XCERH_REG 0x00000078
#define MCBSPLP_RINTCLR_REG 0x00000080
#define MCBSPLP_XINTCLR_REG 0x00000084
#define MCBSPLP_ROVFLCLR_REG 0x00000088
#define MCBSPLP_SYSCONFIG_REG 0x0000008C
#define MCBSPLP_THRSH2_REG 0x00000090
#define MCBSPLP_THRSH1_REG 0x00000094
#define MCBSPLP_IRQSTATUS_REG 0x000000A0
#define MCBSPLP_IRQENABLE_REG 0x000000A4
#define MCBSPLP_WAKEUPEN_REG 0x000000A8
#define MCBSPLP_XCCR_REG 0x000000AC
#define MCBSPLP_RCCR_REG 0x000000B0
#define MCBSPLP_XBUFFSTAT_REG 0x000000B4
#define MCBSPLP_RBUFFSTAT_REG 0x000000B8
#define MCBSPLP_SSELCR_REG 0x000000BC
#define MCBSPLP_STATUS_REG 0x000000C0

// word lengths used in various bit fields
#define MCBSP_WDLEN_8 0
#define MCBSP_WDLEN_12 1
#define MCBSP_WDLEN_16 2
#define MCBSP_WDLEN_20 3
#define MCBSP_WDLEN_24 4
#define MCBSP_WDLEN_32 5

// MCBSPLP_SPCR2_REG
#define MCBSP_FREE (1<<9)
#define MCBSP_SOFT (1<<8)
#define MCBSP_FRST (1<<7)
#define MCBSP_GRST (1<<6)
//#define MCBSP_XINTM (legacy)
#define MCBSP_XSYNCERR (1<<3)
#define MCBSP_XEMPTY (1<<2)
#define MCBSP_XRDY (1<<1)
#define MCBSP_XRST (1<<0)

// MCBSPLP_SPCR1_REG
#define MCBSP_ALB (1<<15)
#define MCBSP_RJUST_ZE (0<<13)	/* zero-extend msb */
#define MCBSP_RJUST_SE (0<<13)	/* sign-extend msb */
#define MCBSP_RJUST_LJ (0<<13)	/* left-adjust and zero-extend lsb */
#define MCBSP_DXENA (1<<7)
//#define MCBSP_RINTM (legacy)
#define MCBSP_RSYNCERR (1<<3)
#define MCBSP_RFULL (1<<2)
#define MCBSP_RRDY (1<<1)
#define MCBSP_RRST (1<<0)

// MCBSPLP_RCR2_REG
#define MCBSP_RPHASE (1<<15)
#define MCBSPB_RFRLEN2 8
#define MCBSPM_RFRLEN2 0x7f00
#define MCBSPB_RWDLEN2 5
#define MCBSPM_RWDLEN2 (0x7 << 5)
#define MCBSP_RREVERSE_MSB (0<<3)
#define MCBSP_RREVERSE_LSB (1<<3)
#define MCBSPM_RDATDLY 0x03

// MCBSPLP_RCR1_REG
#define MCBSPB_RFRLEN1 8
#define MCBSPM_RFRLEN1 0x7f00
#define MCBSPB_RWDLEN1 5
#define MCBSPM_RWDLEN1 (0x7 << 5)

// MCBSPLP_XCR2_REG
#define MCBSP_XPHASE (1<<15)
#define MCBSPB_XFRLEN2 8
#define MCBSPM_XFRLEN2 0x7f00
#define MCBSPB_XWDLEN2 5
#define MCBSPM_XWDLEN2 (0x7 << 5)
#define MCBSP_XREVERSE_MSB (0<<3)
#define MCBSP_XREVERSE_LSB (1<<3)
#define MCBSPM_XDATDLY 0x03

// MCBSPLP_XCR1_REG
#define MCBSPB_XFRLEN1 8
#define MCBSPM_XFRLEN1 0x7f00
#define MCBSPB_XWDLEN1 5
#define MCBSPM_XWDLEN1 (0x7 << 5)

// MCBSPLP_SRGR2_REG
#define MCBSP_GSYNC (1<<15)
#define MCBSP_CLKSP (1<<14)
#define MCBSP_CLKSM (1<<13)
#define MCBSP_FSGM (1<<12)
#define MCBSPBM_FPER (0x0fff)
#define MCBSPBB_FPER 0

// MCBSPLP_SRGR1_REG
#define MCBSPBB_FWID 8
#define MCBSPBM_FWID 0xff00
#define MCBSPBB_CLKGDV 0
#define MCBSPBM_CLKGDV 0x00ff

// PCR_MCBSPLP_PCR_REG
#define MCBSP_IDLE_EN (1<<14)
#define MCBSP_XIOEN (1<<13)
#define MCBSP_RIOEN (1<12)
#define MCBSP_FSXM (1<<11)
#define MCBSP_FSRM (1<<10)
#define MCBSP_CLKXM (1<<9)
#define MCBSP_CLKRM (1<<8)
#define MCBSP_SCLKME (1<<7)
#define MCBSP_CLKS_STAT (1<<6)
#define MCBSP_DX_STAT (1<<5)
#define MCBSP_DR_STAT (1<<4)
#define MCBSP_FSXP (1<<3)
#define MCBSP_FSRP (1<<2)
#define MCBSP_CLKXP (1<<1)
#define MCBSP_CLKRP (1<<0)

#endif
