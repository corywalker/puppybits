/*  omap3-system.h: OMAP3 various system components/utils

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
#ifndef _OMAP3_SYSTMEM_H
#define _OMAP3_SYSTMEM_H

#include <stdint.h>

// System control module
#define CONTROL_GENERAL_BASE 0x48002270

#define CONTROL_DEVCONF0 0x04

// CONTROL_DEVCONF0 bits
#define CONTROL_MCBSP2_CLKS (1<<6)

// SDRAM Controller
#define SDRC_BASE 0x6D000000 

#define SDRC_SYSCONFIG 0x00000010
#define SDRC_SYSSTATUS 0x00000014
#define SDRC_CS_CFG 0x00000040
#define SDRC_SHARING 0x00000044
#define SDRC_ERR_ADDR 0x00000048
#define SDRC_ERR_TYPE 0x0000004C
#define SDRC_DLLA_CTRL 0x00000060
#define SDRC_DLLA_STATUS 0x00000064
#define SDRC_POWER_REG 0x00000070
#define SDRC_MCFG_0 0x00000080
#define SDRC_MCFG_1 (0x00000080+0x30)
#define SDRC_MR_0 0x00000084
#define SDRC_MR_1 (0x00000084+0x030)
#define SDRC_EMR2_p 0x0000008C
#define SDRC_ACTIM_CTRLA_p 0x0000009C
#define SDRC_ACTIM_CTRLB_p 0x000000A0
#define SDRC_RFR_CTRL_p 0x000000A4
#define SDRC_MANUAL_p 0x000000A8

// ramsize from MCFG_p, in 2MB chunks
#define SDRCM_RAMSIZE 0x3ff00
#define SDRCB_RAMSIZE 8

// offset for start address
#define SDRC_CS1OFFHIGH 27
#define SDRC_CS1OFFLOW 25

#define SDRCM_CS1STARTHIGH 0xf
#define SDRCM_CS1STARTLOW 0x300
#define SDRCB_CS1STARTHIGH 0
#define SDRCB_CS1STARTLOW 8

// SDRC chip select 0 start address
#define SDRC_NCS0 0x80000000

// High-Speed I2C Controller
#define I2C1_BASE 0x48070000
#define I2C2_BASE 0x48072000
#define I2C3_BASE 0x48060000

#define I2C_IE 0x04
#define I2C_STAT 0x08
#define I2C_WE 0x0C
#define I2C_SYSS 0x10
#define I2C_BUF 0x14
#define I2C_CNT 0x18
#define I2C_DATA 0x1C
#define I2C_SYSC 0x20
#define I2C_CON 0x24
#define I2C_OA0 0x28
#define I2C_SA 0x2C
#define I2C_PSC 0x30
#define I2C_SCLL 0x34
#define I2C_SCLH 0x38
#define I2C_SYSTEST 0x3C
#define I2C_BUFSTAT 0x40
#define I2C_OA1 0x44
#define I2C_OA2 0x48
#define I2C_OA3 0x4C
#define I2C_ACTOA 0x50
#define I2C_SBLOCK 0x54

// I2C_STAT bits
#define I2C_STAT_XDR (1<<14)
#define I2C_STAT_RDR (1<<13)
#define I2C_STAT_BB (1<<12)
#define I2C_STAT_XRDY (1<<4)
#define I2C_STAT_RRDY (1<<3)
#define I2C_STAT_ARDY (1<<2)
#define I2C_STAT_NACK (1<<1)
#define I2C_STAT_AL (1<<0)
// I2C_CON bits
#define I2C_CON_MST (1<<10)
#define I2C_CON_TRX (1<<9)
#define I2C_CON_STP 2
#define I2C_CON_STT 1


// assembly register names for mcr/mrc
#define CP15_SCTLR "c1, c0, 0"
#define CP15_TTBR0 "c2, c0, 0"
#define CP15_TTBR1 "c2, c0, 1"
#define CP15_TTBCR "c2, c0, 2"
#define CP15_DACR "c3, c0, 0"

#define CP15_TLBIALL "c8, c7, 0"

// Control register bits
// c1, SCTLR
#define SCTLR_TE (1<<30)
#define SCTLR_AFE (1<<29)
#define SCTLR_TRE (1<<28)
#define SCTLR_ICACHE (1<<12)	/* I */
#define SCTLR_PREDICT (1<<11)	/* Z */
#define SCTLR_DCACHE (1<<2)	/* C */
#define SCTLR_ALIGN (1<<1)	/* A */
#define SCTLR_MMUEN (1<<0)	/* M */

// read system co-processor register
#define mrc15(func) ({						\
	unsigned int v = 0;						\
	asm volatile("mrc p15, 0, %[res], " func :[res] "=r" (v)); \
	v;								\
})

// write system co-processor register
#define mcr15(func, v) \
	asm volatile("mcr p15, 0, %[val], " func ::[val] "r" (v));

// Read/write I/O registers
#define reg32r(b, r) (*(volatile uint32_t *)((b)+(r)))
#define reg32w(b, r, v) (*((volatile uint32_t *)((b)+(r))) = (v))
#define reg32s(b, r, m, v) reg32w(b, r, (reg32r(b, r) & ~(m)) | ((v) & (m)))
#define reg16r(b, r) (*(volatile uint16_t *)((b)+(r)))
#define reg16w(b, r, v) (*((volatile uint16_t *)((b)+(r))) = (v))
#define reg16s(b, r, m, v) reg16w(b, r, (reg16r(b, r) & ~(m)) | ((v) & (m)))

#endif
