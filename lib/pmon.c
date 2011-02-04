/*  pmon.c: Utilities to access the performance monitor counters

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
  To use the performance monitor counters:

  1. disable counters
  2. set what to count
  3. reset counters
  4. enable counters
  5. execute target code
  6. disable counters
  7. read counts (& check overflow)
*/

#include "pmon.h"
#include "pbits.h"

#define mrc(func) ({						\
	unsigned int v = 0;						\
									\
	asm volatile("mrc p15, 0, %[res], " func :[res] "=r" (v)); \
	v;								\
})

#define mcr(func, v) \
	asm volatile("mcr p15, 0, %[val], " func ::[val] "r" (v));

#define read_pmnc() mrc("c9, c12, 0")
#define write_pmnc(v) mcr("c9, c12, 0", v)
#define read_cntens() mrc("c9, c12, 1")
#define write_cntens(v) mcr("c9, c12, 1", v)
#define read_cntenc() mrc("c9, c12, 2")
#define write_cntenc(v) mcr("c9, c12, 2", v)
//(FLAG), 3
#define read_swincrc() mrc("c9, c12, 4")
#define write_swincr(v) mcr("c9, c12, 4", v)
#define read_pmnxsel() mrc("c9, c12, 5")
#define write_pmnxsel(v) mcr("c9, c12, 5", v)
#define read_ccnt() mrc("c9, c13, 0")
#define write_ccnt(v) mcr("c9, c13, 0", v)
//(EVTSEL), c13,1
#define read_evtsel() mrc("c9, c13, 1")
#define write_evtsel(v) mcr("c9, c13, 1", v)
//(PMCNT0-PMCNT3), c13, 2
#define read_pmcntx() mrc("c9, c13, 2")
#define write_pmcntx(v) mcr("c9, c13, 2", v)
#define read_intenc() mrc("c9, c14, 2")
#define write_intenc(v) mcr("c9, c14, 2", v)

void pmon_init(void) {
	// ignore interrupts
	write_intenc(~0);

	pmon_disable(~0);
	pmon_reset(~0);
	write_pmnc(PMNC_CCRESET | PMNC_PCRESET | PMNC_ENABLE);

	dprintf("pmon_init(): have %d performance counters\n", (read_pmnc() >> 11) & 0x1f);
}

void pmon_inc(int flags) {
}

// reset specific timers
void pmon_reset(int flags) {
	write_pmnc(read_pmnc() | PMNC_CCRESET | PMNC_PCRESET);
}

// monitor specific events
void pmon_pcevent(int pc, unsigned int event) {
	write_pmnxsel(pc);
	write_evtsel(event);
}

// enable timers
void pmon_enable(int flags) {
	write_pmnc(read_pmnc() | 1);
	write_cntens(flags);
}

void pmon_disable(int flags) {
	write_pmnc(read_pmnc() & ~1);
	write_cntenc(flags);
}

// get cycle counter
unsigned int pmon_ccnt(void) {
	return read_ccnt();
}

unsigned int pmon_pc(int id) {
	write_pmnxsel(id);
	return read_pmcntx();
}
