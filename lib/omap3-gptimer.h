/*  omap3-timer.h: OMAP3 timers

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
#ifndef _OMAP3_TIMER_H
#define _OMAP3_TIMER_H

#define GPTIMER1_BASE 0x48318000
#define GPTIMER2_BASE 0x49032000
#define GPTIMER3_BASE 0x49034000
#define GPTIMER4_BASE 0x49036000
#define GPTIMER5_BASE 0x49038000
#define GPTIMER6_BASE 0x4903a000
#define GPTIMER7_BASE 0x4903c000
#define GPTIMER8_BASE 0x4903e000
#define GPTIMER9_BASE 0x49040000
#define GPTIMER10_BASE 0x48086000
#define GPTIMER12_BASE 0x48088000
#define GPTIMER13_BASE 0x48304000

#define GPT_TIOCP_CFG 0x010
#define GPT_TISTAT 0x014
#define GPT_TISR 0x018
#define GPT_TIER 0x01C
#define GPT_TWER 0x020
#define GPT_TCLR 0x024
#define GPT_TCRR 0x028
#define GPT_TLDR 0x02C
#define GPT_TTGR 0x030
#define GPT_TWPS 0x034
#define GPT_TMAR 0x038
#define GPT_TCAR1 0x03C
#define GPT_TSICR 0x040
#define GPT_TCAR2 0x044
#define GPT_TPIR 0x048
#define GPT_TNIR 0x04C
#define GPT_TCVR 0x050
#define GPT_TOCR 0x054
#define GPT_TOWR 0x058

// TISR/TIER flags
#define TCAR_IT_FLAG (1<<2)
#define OVF_IT_FLAG (1<<1)
#define MAT_IT_FLAG (1<<0)

#ifndef __ASSEMBLER__
struct omap3_gptimer {
	uint32_t pad0[4];
	volatile uint32_t TIOCP_CFG;
	volatile uint32_t TISTAT;
	volatile uint32_t TISR;
	volatile uint32_t TIER;
	volatile uint32_t TWER;
	volatile uint32_t TCLR;
	volatile uint32_t TCCR;
	volatile uint32_t TLDR;
	volatile uint32_t TTGR;
	volatile uint32_t TWPS;
	volatile uint32_t TMAR;
	volatile uint32_t TCAR1;
	volatile uint32_t TSICR;
	volatile uint32_t TCAR2;
	volatile uint32_t TPIR;
	volatile uint32_t TNIR;
	volatile uint32_t TCVR;
	volatile uint32_t TOCR;
	volatile uint32_t TOWR;
};
#endif

#endif


