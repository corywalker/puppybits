/*  pmon.h: Defines and functions for performance monitor counters/timing

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
#ifndef _PMON_H
#define _PMON_H

// performance counters available
#define PMCNT0 0
#define PMCNT1 1
#define PMCNT2 2
#define PMCNT3 3

// pmnc flags
#define PMNC_DP (1<<5)
#define PMNC_EXPORT (1<<4)
#define PMNC_CCDIVIDER (1<<3)	/* /64 if set, /1 if not */
#define PMNC_CCRESET (1<<2)
#define PMNC_PCRESET (1<<1)
#define PMNC_ENABLE 1

// CNTENS counter enable bits
#define CNTEN_CC (1<<31)
#define CNTEN_P0 (1<<0)
#define CNTEN_P1 (1<<1)
#define CNTEN_P2 (1<<2)
#define CNTEN_P3 (1<<3)
#define CNTEN_ALL ((1<<31)|0x0f)

// See Cortex-A8 TRM S 3.2.49 c9, Event Selection Register
#define EVTSEL_SOFTINC (0)
#define EVTSEL_ICACHE_L2MISS (1)
#define EVTSEL_ITLB_L2MISS (2)
#define EVTSEL_DCACHE_L2MISS (3)
#define EVTSEL_DCACHE_L2ACCESS (4)
#define EVTSEL_DTLB_L2MISS (5)
#define EVTSEL_DREAD_INSTNS (6)
#define EVTSEL_DWRITE_INSTNS (7)
#define EVTSEL_INSTNS (8)
#define EVTSEL_EXCEPT (9)
#define EVTSEL_EXCEPT_RETURN (0x0a)
#define EVTSEL_CTX_WRITE (0x0b)
#define EVTSEL_SETPC (0x0c)
#define EVTSEL_IMMED_BRANCH_EXEC (0x0d)
#define EVTSEL_PROC_RETURN 0x0e
#define EVTSEL_DMISALIGNED 0x0f
#define EVTSEL_BRANCH_MISS 0x10
#define EVTSEL_CCOUNT 0x11
#define EVTSEL_BRANCH_HIT 0x12
#define EVTSEL_DWRITE_STALL 0x40
#define EVTSEL_DWRITE_L2MERGE 0x41
#define EVTSEL_DWRITE_L2BUFFER 0x42
#define EVTSEL_ANY_L2HIT 0x43
#define EVTSEL_ANY_L2MISS 0x44
#define EVTSEL_AXIREAD 0x45
#define EVTSEL_AXIWRITE 0x46
#define EVTSEL_ANY_REPLAY 0x47
#define EVTSEL_MISALIGNED_REPLAY 0x48
#define EVTSEL_DCACHE_L1MISS_HASH 0x49
#define EVTSEL_ICACHE_L1MISS_HASH 0x4a
// ok this is getting silly ...
#define EVTSEL_DCACHE_L1EVICT 0x4b
#define EVTSEL_NEON_DCACHE_L1HIT 0x4c
#define EVTSEL_NEON_DCACHE_L1CACHEABLE 0x4d
#define EVTSEL_NEON_L2ACCESS 0x4e
#define EVTSEL_NEON_L2HIT 0x4f
#define EVTSEL_ICACHE_L1HIT 0x50
#define EVTSEL_RETURN_MISS 0x51
#define EVTSEL_BRANCH_MISS2 0x52
#define EVTSEL_BRANCH_HIT_TAKEN 0x53
#define EVTSEL_BRANCH_TAKEN 0x54
#define EVTSEL_IISSUE 0x55
#define EVTSEL_ISTALL 0x56
#define EVTSEL_IMULTIISSUE 0x57
#define EVTSEL_NEON_MRC_STALL 0x58
#define EVTSEL_NEON_STALL 0x59
#define EVTSEL_NEON_ALU_USED 0x60
#define EVTSEL_EXTERNAL0 0x70
#define EVTSEL_EXTERNAL1 0x71
#define EVTSEL_EXTERNAL 0x72
// phew!

void pmon_init(void);

// should these just be macros/inline?

// use PMCNTx and EVTSEL_x
void pmon_pcevent(int pc, unsigned int event);

void pmon_reset(int flags);

// note that this affects all timers, so you can't turn individual ones on/off
void pmon_enable(int flags);
void pmon_disable(int flags);

unsigned int pmon_ccnt(void);
unsigned int pmon_pc(int id);

#endif
