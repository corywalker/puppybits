/*  mmu-simple.c: MMU routines for identity mapping everything.

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
  SDRAM
  -----
  This maps all of the 2 SDRAM banks in two ways.  It should work for
  any sized memory present.

  The first is mapped as write-back write-allocate cacheable.  This is
  where code should reside.

  The second is mapped as write-through no-write-allocate.  So this is
  ... `chip memory' suitable for a frame-buffer and easier to user
  for other DMA type operations.
 */
#include "pbits.h"
#include "omap3-system.h"
#include "omap3-mmu.h"

uint32_t mmu_pt[4096] __attribute__ ((aligned(0x4000)));

static void map_empty(uint32_t *pt) {
	int i;

	for (i=0;i<4096;i++)
		pt[i] = MMUTTL1_FAULT;
}

// map a (super)section over range with attributes
// super-sections are automagically used if the range aligns
static void map_section(uint32_t *pt, uint32_t start, uint32_t end, uint32_t attr)
{
	uint32_t addr;

	if (((start | end) & 0x00ffffff) == 0)
		attr |= MMUTTL1_SUPER;
	else
		attr |= MMUTTL1_SECTION;

	// align range to 1MB blocks
	start = (start + 0xfffff) & 0xfff00000;
	end = (end + 0xfffff) & 0xfff00000;

	for (addr=start;addr != end;addr += 0x100000) {
		uint32_t pte = addr | attr;

		pt[addr >> 20] = pte;
	}
}

/* From OMAP35x TRM S 2.2

GPMC               0x00000000   0x3FFFFFFF    1GB  NAND, etc. GPMC regs drefine what where

Boot ROM secure    0x40000000   0x40013FFF   80KB
Boot ROM public    0x40014000   0x4001BFFF   32KB
SRAM internal      0x40200000   0x4020FFFF   64KB

L4-Core            0x48000000   0x48FFFFFF   16MB 
(L4-Wakeup)       (0x48300000) (0x4833FFFF) (256KB)
L4-Per             0x49000000   0x490FFFFF    1MB

SGX                0x50000000   0x5000FFFF   64KB
L4-Emu             0x54000000   0x547FFFFF    8MB

IVA2.2 SS          0x5C000000   0x5EFFFFFF   48MB

L3 Control Regs    0x68000000   0x68FFFFFF   16MB
SMS registers      0x6C000000   0x6CFFFFFF   16MB
SDRC registers     0x6D000000   0x6DFFFFFF   16MB
GPMC registers     0x6E000000   0x6EFFFFFF   16MB

SDRC/SMS VAS 0     0x70000000   0x7FFFFFFF  256MB
SDRC/SMS VAS 1     0xE0000000   0x00000000  256MB

*/

// simplified OMAP35x memory map using 1MB blocks
struct iomap {
	const char *name;
	uint32_t base;
	uint32_t end;
	uint32_t ap;
	uint32_t texcb;
} iomap[] = {
	{ "boot rom",      0x40000000, 0x40100000, MMU_3AP_RORO, MMU_CODE },
	{ "ram_internal",  0x40200000, 0x40300000, MMU_3AP_RWRW, MMU_DATA },
	{ "l4-core",       0x48000000, 0x49000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "l4-per",        0x49000000, 0x49100000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "sgx",           0x50000000, 0x50100000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "l4-emu",        0x54000000, 0x54800000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "iva 2.2",       0x5c000000, 0x5f000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "l3",            0x68000000, 0x69000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "sms",           0x6c000000, 0x6d000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "sdrc",          0x6d000000, 0x6e000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ "gpmc",          0x6e000000, 0x6f000000, MMU_3AP_RWRW, MMU_DEVICE_SHARED },
	{ 0 }
};

/* Very simple mmu init code,

   sets up a page table which identity maps everything we might use using 1MB sections
   or 16MB super-sections.
*/
void
mmu_simple_init(void) {
	uint32_t v;
	uint32_t ram0_size;
	uint32_t ram1_size, ram1_base = SDRC_NCS0;

	// Make sure MMU & caches off, and MMU has TEX remap and access flag disabled.
        mcr15(CP15_SCTLR, mrc15(CP15_SCTLR)
	      & ~(SCTLR_AFE | SCTLR_TRE | SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_MMUEN ));

	map_empty(mmu_pt);

	// find out how much memory we have, and where bank 1 is
	v = reg32r(SDRC_BASE, SDRC_MCFG_0);
	ram0_size = ((v & SDRCM_RAMSIZE) >> SDRCB_RAMSIZE) * 0x200000;
	v = reg32r(SDRC_BASE, SDRC_MCFG_1);
	ram1_size = ((v & SDRCM_RAMSIZE) >> SDRCB_RAMSIZE) * 0x200000;
	v = reg32r(SDRC_BASE, SDRC_CS_CFG);
	ram1_base += ((v & SDRCM_CS1STARTHIGH) << SDRC_CS1OFFHIGH)
		| ((v & SDRCM_CS1STARTLOW) << SDRC_CS1OFFLOW);
		
	int i;

	dprintf("mmu_simple_init: memory map\n");
	dprintf("  Base     End      AP  TEXCB Name\n");

	// Map all the fixed ranges
	for (i=0;iomap[i].name;i++) {
		dprintf("  %08x-%08x %03b %05b %s\n",
			iomap[i].base, iomap[i].end-1,
			iomap[i].ap, iomap[i].texcb, iomap[i].name);
		map_section(mmu_pt, iomap[i].base, iomap[i].end,
			    mmu_texcb_sect(iomap[i].texcb)
			    | mmu_3ap_sect(iomap[i].ap));			    
	}

	// Map the RAM (it's only done second so the debug is sorted)
	dprintf("  %08x-%08x %03b %05b %s\n",
		SDRC_NCS0, ram0_size + SDRC_NCS0-1,
		MMU_3AP_RWRW, MMU_DATA,
		"CS0 Ram");

	map_section(mmu_pt, SDRC_NCS0, ram0_size + SDRC_NCS0,
		    mmu_texcb_sect(MMU_DATA)
		    | mmu_3ap_sect(MMU_3AP_RWRW));

	dprintf("  %08x-%08x %03b %05b %s\n",
		ram1_base, ram1_size + ram1_base-1,
		MMU_3AP_RWRW, MMU_DMA,
		"CS1 Ram");

	map_section(mmu_pt, ram1_base, ram1_base + ram1_size,
		    mmu_texcb_sect(MMU_DMA)
		    | mmu_3ap_sect(MMU_3AP_RWRW));

	// A couple of steps are required to turn it on
	mmu_flush_tlb();
	mmu_set_ttbr0(mmu_pt);
	mmu_flush_tlb();
	mcr15(CP15_DACR, 0xffffffff);		      /* set all domains to manager */

	mcr15(CP15_SCTLR, mrc15(CP15_SCTLR)
	      | SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_PREDICT | SCTLR_MMUEN);
}
