/*  omap3-intc.h: OMAP3 interrupt controller

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
#ifndef _OMAP3_INTC_H
#define _OMAP3_INTC_H

#define INTCPS_BASE 0x48200000
#define INTCPS_SYSCONFIG 0x0010
#define INTCPS_SYSSTATUS 0x0014
#define INTCPS_SIR_IRQ 0x40
#define INTCPS_SIR_FIQ 0x44
#define INTCPS_CONTROL 0x48
#define INTCPS_PROTECTION 0x004C
#define INTCPS_IDLE       0x0050
#define INTCPS_IRQ_PRIORITY 0x0060
#define INTCPS_FIQ_PRIORITY 0x0064
#define INTCPS_THRESHOLD 0x0068

#define INTCPS_ITR0 0x0080
#define INTCPS_MIR0 0x0084
#define INTCPS_MIR_CLEAR0 0x88
#define INTCPS_MIR_SET0 0x8c
#define INTCPS_ISR_SET0 0x0090
#define INTCPS_ISR_CLEAR0 0x0094
#define INTCPS_PENDING_IRQ0 0x0098
#define INTCPS_PENDING_FIQ0 0x009C

#define INTCPS_ITR1 0x00a0
#define INTCPS_MIR1 0x00a4
#define INTCPS_MIR_CLEAR1 0xa8
#define INTCPS_MIR_SET1 0xac
#define INTCPS_ISR_SET1 0x00b0
#define INTCPS_ISR_CLEAR1 0x00b4
#define INTCPS_PENDING_IRQ1 0x00b8
#define INTCPS_PENDING_FIQ1 0x00bC

#define INTCPS_ITR2 0x00c0
#define INTCPS_MIR2 0x00c4
#define INTCPS_MIR_CLEAR2 0xc8
#define INTCPS_MIR_SET2 0xcc
#define INTCPS_ISR_SET2 0x00d0
#define INTCPS_ISR_CLEAR2 0x00d4
#define INTCPS_PENDING_IRQ2 0x00d8
#define INTCPS_PENDING_FIQ2 0x00dC

#define INTCPS_ILR0 0x0100

#ifndef __ASSEMBLER__
struct omap3_intcpsn {
	volatile uint32_t ITR;
	volatile uint32_t MIR;
	volatile uint32_t MIR_CLEAR;
	volatile uint32_t MIR_SET;
	volatile uint32_t ISR_SET;
	volatile uint32_t ISR_CLEAR;
	volatile uint32_t PENDING_IRQ;
	volatile uint32_t PENDING_FIQ;
};

struct omap3_intcps {
	uint32_t pad0[4];
	volatile uint32_t SYSCONFIG;
	volatile uint32_t SYSSTATUS;
	uint32_t pad1[(0x40 - 0x18) /4];
	volatile uint32_t SIR_IRQ;
	volatile uint32_t SIR_FIQ;
	volatile uint32_t CONTROL;
	volatile uint32_t PROTECTION;
	volatile uint32_t IDLE;
	uint32_t pad2[(0x60 - 0x54) /4];
	volatile uint32_t IRQ_PRIORITY;
	volatile uint32_t FIQ_PRIORITY;
	volatile uint32_t THRESHOLD;
	uint32_t pad3[(0x80 - 0x6c) /4];
	struct omap3_intcpsn irq[3];
	volatile uint32_t ILR[96];
};
#endif

// interrupt numbers for peripherals
#define INTC_BENCH 3
#define INTC_CAM_IRQ0 24
#define INTC_COMMRX 2
#define INTC_COMMTX 1
#define INTC_DSS_IRQ 25
#define INTC_EHCI_IRQ 77
#define INTC_EMUINT 0
#define INTC_GPIO1_MPU_IRQ 29
#define INTC_GPIO2_MPU_IRQ 30
#define INTC_GPIO3_MPU_IRQ 31
#define INTC_GPIO4_MPU_IRQ 32
#define INTC_GPIO5_MPU_IRQ 33
#define INTC_GPIO6_MPU_IRQ 34

#define INTC_GPMC_IRQ 20
#define INTC_GPT1_IRQ 37
#define INTC_GPT2_IRQ 38
#define INTC_GPT3_IRQ 39
#define INTC_GPT4_IRQ 40
#define INTC_GPT5_IRQ 41
#define INTC_GPT6_IRQ 42
#define INTC_GPT7_IRQ 43
#define INTC_GPT8_IRQ 44
#define INTC_GPT9_IRQ 45
#define INTC_GPT10_IRQ 46
#define INTC_GPT11_IRQ 47

#define INTC_HDQ_IRQ 58
#define INTC_HSUSB_DMA_NINT 93
#define INTC_HSUSB_MC_NINT 92
#define INTC_I2C1_IRQ 56
#define INTC_I2C2_IRQ 57
#define INTC_I2C3_IRQ 61
#define INTC_IVA2_MMU_IRQ 28
#define INTC_MAIL_U0_MPU_IRQ
#define INTC_MCBSP1_IRQ 16
#define INTC_MCBSP2_IRQ 17
#define INTC_MCBSP2_ST_IRQ 4
#define INTC_MCBSP3_IRQ 22
#define INTC_MCBSP3_IRQ_RX 90
#define INTC_SPI3_IRQ 91
#define INTC_MCBSP3_IRQ_TX 89
#define INTC_MCBSP3_ST_IRQ 5
#define INTC_MCBSP4_IRQ 23
#define INTC_MCBSP4_IRQ_RX 55
#define INTC_MCBSP4_IRQ_TX 54
#define INTC_MCBSP5_IRQ 27
#define INTC_MCBSP5_IRQ_RX 82
#define INTC_MCBSP5_IRQ_TX 81
#define INTC_MG_IRQ 53
#define INTC_MMC1_IRQ 83
#define INTC_MMC2_IRQ 86
#define INTC_MMC3_IRQ 94
#define INTC_MPU_ICR_IRQ 87
#define INTC_MS_IRQ 84
#define INTC_McBSP1_IRQ_RX 60
#define INTC_McBSP1_IRQ_TX 59
#define INTC_McBSP2_IRQ_RX 63
#define INTC_McBSP2_IRQ_TX 62
#define INTC_OHCI_IRQ 76
#define INTC_PBIAS_IRQ 75
#define INTC_PRCM_MPU_IRQ 11
#define INTC_SDMA_IRQ_0 12
#define INTC_SDMA_IRQ_1 13
#define INTC_SDMA_IRQ_2 14
#define INTC_SDMA_IRQ_3 15
#define INTC_SGX_IRQ 21
#define INTC_SMX_APP_IRQ 10
#define INTC_SMX_DBG_IRQ 9
#define INTC_SPI1_IRQ 65
#define INTC_SPI2_IRQ 66
#define INTC_SPI4_IRQ 48
#define INTC_SR1_IRQ 18
#define INTC_SR2_IRQ 19
#define INTC_TLL_IRQ 78
#define INTC_UART1_IRQ 72
#define INTC_UART2_IRQ 73
#define INTC_UART3_IRQ 74
#define INTC_WDT3_IRQ 36
#define INTC_sys_nirq 7

#endif
