/*  omap3-mmy.h: OMAP3 MMU related stuff (actually it's cortex-a8)

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
#ifndef _OMAP3_MMU_H
#define _OMAP3_MMU_H

// Translation table level 1 types
#define MMUTTL1_FAULT 0x00000000
#define MMUTTL1_PAGE 0x00000001
#define MMUTTL1_SECTION 0x00000002
#define MMUTTL1_SUPER 0x00040002

// Translation table level 2 types
#define MMUTTL2_FAULT 0x00000000
#define MMUTTL2_LARGE 0x00000001
#define MMUTTL2_SMALL 0x00000002

// section/super-section bits
#define MMU_SECT_NS (1<<19)
#define MMU_SECT_nG (1<<17)
#define MMU_SECT_S (1<<16)
#define MMU_SECT_AP2 (1<<15)
#define MMU_SECT_TEX2 (1<<14)
#define MMU_SECT_TEX1 (1<<13)
#define MMU_SECT_TEX0 (1<<12)
#define MMU_SECT_AP1 (1<<11)
#define MMU_SECT_AP0 (1<<10)
// domain only applies to plain sections
#define MMUB_SECT_DOMAIN 5
#define MMUM_SECT_DOMAIN 0x1e0
#define MMU_SECT_XN (1<<4)
#define MMU_SECT_C (1<<3)
#define MMU_SECT_B (1<<2)

// large table bits
#define MMU_LARGE_XN (1<<15)
#define MMU_LARGE_TEX2 (1<<14)
#define MMU_LARGE_TEX1 (1<<13)
#define MMU_LARGE_TEX0 (1<<12)
#define MMU_LARGE_nG (1<<11)
#define MMU_LARGE_S (1<<10)
#define MMU_LARGE_AP2 (1<<9)
#define MMU_LARGE_AP1 (1<<5)
#define MMU_LARGE_AP0 (1<<4)
#define MMU_LARGE_C (1<<3)
#define MMU_LARGE_B (1<<2)

// and small table bits
#define MMU_SMALL_nG (1<<11)
#define MMU_SMALL_S (1<<10)
#define MMU_SMALL_AP2 (1<<9)
#define MMU_SMALL_TEX2 (1<<8)
#define MMU_SMALL_TEX1 (1<<7)
#define MMU_SMALL_TEX0 (1<<6)
#define MMU_SMALL_AP1 (1<<5)
#define MMU_SMALL_AP0 (1<<4)
#define MMU_SMALL_C (1<<3)
#define MMU_SMALL_B (1<<2)
#define MMU_SMALL_XN (1<<0)

// Permissions are defined in terms of MMU_3AP_*, these define the perm types,
// and use the mmy_3ap_* macros to convert them for the right page type

/* The meaning of AP, TEX, CB depend on various settings ...
   These are for:
   SCTLR.AFE = 0 - AP[0] defines attributes, not a `dirty' bit
   SCTLR.TRE = 0 - Tex remap disabled   
*/

// for 3-bit AP, these are the permissions
// *_[priv][user]
// NA = no access, RO = read-only, RW = read-write
#define MMU_3AP_NANA 0
#define MMU_3AP_RWNA 1
#define MMU_3AP_RWRO 2
#define MMU_3AP_RWRW 3
#define MMU_3AP_RONA 5
#define MMU_3AP_RORO 7

// build an ap for a (super)section
#define mmu_3ap_sect(x) ((((x) & 4) << 13) | (((x) & 3) << 10))
// large table
#define mmu_3ap_large(x) ((((x) & 4) << 7) | (((x) & 3) << 4))
// small
#define mmu_3ap_small(x) mmu_3ap_large(x)

// Similarly for the perm bits, the memory type bits are defined
// using a common number, then use the mmu_texcb_* macros to reap
// to each type

// Defines for different TEXCB options with TEX as [4:2], C[1], B[0]
// SH - shared
// WT - write-through
// WB - write-back
// WA - write-allocate
// CA - cacheable
// prefix N - not
#define MMU_TEXCB_ORDERED 0
#define MMU_TEXCB_SH_DEVICE 1
#define MMU_TEXCB_WT_NWA 2
#define MMU_TEXCB_WB_NWA 3
#define MMU_TEXCB_NCA 4
#define MMU_TEXCB_WB_WA 7
#define MMU_TEXCB_NSH_DEVICE 8
// cacheable memory -[outer]_[inner]
#define MMU_TEXCB_CA 0x10
// cacheable memory has inner-outer cacheable attributes at [3:2] and [1:0] for outer, inner, respectively
#define MMU_TEXCB_CA_NCA 0
#define MMU_TEXCB_CA_WB_WA 1
#define MMU_TEXCB_CA_WT_NWA 2
#define MMU_TEXCB_CA_WB_NWA 3

// and some helpers to simplify all this mess

// Code, `write-through' cacheable with no write-allocate
#define MMU_CODE (MMU_TEXCB_CA | MMU_TEXCB_CA_WT_NWA | (MMU_TEXCB_CA_WT_NWA<<2))
// Data, write-back cache with write-allocate
#define MMU_DATA (MMU_TEXCB_CA | MMU_TEXCB_CA_WB_WA | (MMU_TEXCB_CA_WB_WA<<2))
// Data for DMA regions (e.g. frame-buffer), write-through with no write-allocate
#define MMU_DMA (MMU_TEXCB_CA | MMU_TEXCB_CA_WT_NWA | (MMU_TEXCB_CA_WT_NWA<<2))
// shared device, e.g. i/o registers accessible by other cpus (dsp)
#define MMU_DEVICE_SHARED MMU_TEXCB_SH_DEVICE
// non-shared device, cpu-only device (not sure if this is needed)
#define MMU_DEVICE_NSHARED MMU_TEXCB_NSH_DEVICE

// convert bits above for a (super)section, large, small table
#define mmu_texcb_sect(x) ((((x) & 0x1c) << 10) | (((x) & 3) << 2))
#define mmu_texcb_large(x) mmu_texcb_sect(x)
#define mmu_texcb_small(x) ((((x) & 0x1c) << 4) | (((x) & 3) << 2))

// Flush all TLB entries
#define mmu_flush_tlb() mcr15(CP15_TLBIALL, 0)
// Set the translation table base register (0)
#define mmu_set_ttbr0(x) mcr15(CP15_TTBR0, (uint32_t)(x))

#endif
