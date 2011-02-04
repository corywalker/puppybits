/*  omap3-prcm.h: OMAP3 power and clock control registers

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
#ifndef _OMAP3_PRCM_H
#define _OMAP3_PRCM_H

/* CORE_CM registers S 4.14.1.5 */
#define CM_CORE_BASE 0x48004A00

#define CM_FCLKEN1_CORE 0x00000000
#define CM_FCLKEN3_CORE 0x00000008
#define CM_ICLKEN1_CORE 0x00000010
#define CM_ICLKEN2_CORE 0x00000014
#define CM_ICLKEN3_CORE 0x00000018
#define CM_IDLEST1_CORE 0x00000020
#define CM_IDLEST2_CORE 0x00000024
#define CM_IDLEST3_CORE 0x00000028
#define CM_AUTOIDLE1_CORE 0x00000030
#define CM_AUTOIDLE2_CORE 0x00000034
#define CM_AUTOIDLE3_CORE 0x00000038
#define CM_CLKSEL_CORE 0x00000040
#define CM_CLKSTCTRL_CORE 0x00000048
#define CM_CLKSTST_CORE 0x0000004C

// there's a million friggan bits for these, so just define the ones i'm using

// These bits apply to all of the (CM|PM)*1_CORE registers as EN_, ST_, and AUTO_* bits
#define CM_CORE_EN_I2C3 (1<<17)
#define CM_CORE_EN_I2C2 (1<<16)
#define CM_CORE_EN_I2C1 (1<<15)

// PER_CM registers
#define CM_PER_BASE 0x48005000

#define CM_FCLKEN_PER 0x00000000
#define CM_ICLKEN_PER 0x00000010
#define CM_IDLEST_PER 0x00000020
#define CM_AUTOIDLE_PER 0x00000030
#define CM_CLKSEL_PER 0x00000040
#define CM_SLEEPDEP_PER 0x00000044
#define CM_CLKSTCTRL_PER 0x00000048
#define CM_CLKSTST_PER 0x0000004C

// PER_* register enable bis
#define CM_PER_EN_MCBSP3 (1<<1)
#define CM_PER_EN_MCBSP2 (1<<0)

/* Clock control reg_CM S 4.14.1.8 */
#define CM_CLOCK_BASE 0x48004D00

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

/* DSS_CM registers S 4.14.1.9 */
#define CM_DSS_BASE 0x48004E00

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

#endif
