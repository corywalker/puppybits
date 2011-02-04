
#include <stdint.h>
#include "pbits.h"

#include "omap3-system.h"

#if 0
// We don't need any of this stuff - just for idle/powerdown etc

// TODO: offsets are common for all PRM CM modules I think

// Clock manager
#define NEON_CM_BASE 0x48005300
#define CM_IDLEST_NEON 0x20
#define CM_CLKSTCTRL_NEON 0x48

#define IDLEST_ST 1

#define CLKTRCTRL_DISABLED 0
#define CLKTRCTRL_SLEEP 1
#define CLKTRCTRL_WAKE 2
#define CLKTRCTRL_AUTOMATIC 0x3

// Power manager
#define NEON_PRM_BASE 0x48307300

#define RM_RSTST_NEON 0x58
#define PM_WKDEP_NEON 0xc8
#define PM_PWSTCTRL_NEON 0xE0
#define PM_PWSTST_NEON 0x0E4
#define PM_PREPWSTST_NEON 0xe8

#define PM_INTRANSITION (1<<20)
#define PM_POWERSTATE_OFF 0
#define PM_POWERSTATE_RETENTION 1
#define PM_POWERSTATE_INACTIVE 2
#define PM_POWERSTATE_ON 3

#define read32(b, r) (*((volatile uint32_t *)(b+r)))
#define write32(b, r, v) (*((volatile uint32_t *)(b+r)) = (v))

#endif

// gcc 4.3.3 doesn't have vmrs/vmsr, is that a bad sign?
// emulate it using mrc/mcr
#define FPEXC "c8, c0, 0"
#define vmrs(func) ({ unsigned int v = 0;				\
	asm volatile("mrc p10, 7, %[res]," func :[res] "=r" (v));	\
	v;								\
})

#define vmsr(reg, v) \
	asm volatile("mcr p10, 7, %[val], " reg ::[val] "r" (v));

void neon_init(void) {
	uint32_t v;

	// Fuck this took a long time to discover ...

	// First, need to enable access to co-processors c10 and c11 - vfp and neon

	v = mrc15("c1, c0, 2");
	v |= 0xf<<20;
	mcr15("c1, c0, 2", v);

	// required apparently
	asm volatile("isb");

	// Next, just need to enable NEON instructions.  The ROM or uboot has already turned it on.

	vmsr(FPEXC, 1<<30);
}

