/*  pbits.h: Public header for puppy-bits utility functions

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
#ifndef _PBITS_H
#define _PBITS_H

#include <stdint.h>

struct BitMap;

/* debug i/o functions */
void dputc(int c);
int dgetc(void);
void dputs(const char *s);
void dprintf(const char *fmt, ...);
/* interactively read a line of text */
int dreadline(char *buffer, int len);
unsigned int dstrtoul(char *s, char **end, unsigned int base);

/* enter debugger */
void debugmon(void) __attribute__ ((noreturn));

/* sleep for a bit - order of milliseconds */
void ksleep(int time);

/* random number generator */
void srand(unsigned int seed);
unsigned long rand(void);

/* video 'driver' */
/* set physical video size */
void video_init(int width, int height);
/* change the physical video size */
void omap_set_lcd_mode(int w, int h);
/* set a graphics channel/overlay */
void omap_attach_framebuffer(int id, struct BitMap *bm);
/* turn off a channel/overlay */
void omap_disable_display(int vid);

// for attach id
#define VID_GFX (0)
#define VID_VID1 (1)
#define VID_VID2 (2)
#define VID_TVOUT (1<<8)
#define VID_INTER (1<<9)

/* required to use NEON SIMD or FPU */
void neon_init(void);

/* identity map all memory.  required for cpu caches, second bank is write-through */
void mmu_simple_init(void);

/* i2c bus */
void bus_i2c_init(void);
void bus_i2c_read(uint32_t base, uint8_t sa, uint8_t addr, uint8_t *buffer, int count);
// address stored as first byte(s) of buffer
void bus_i2c_write(uint32_t base, uint8_t sa, uint8_t *buffer, int count);
// write 8 bites
void bus_i2c_write8(uint32_t base, uint8_t sa, uint8_t addr, uint8_t v);

/* Lowest Level Interrupts */
typedef void (*irq_fn)(int);
// enable/disable interrupts, may be nested
void irq_enable(void);
void irq_disable(void);
// enable/disable specific interrupts
void irq_set_mask(int num, int on);
// set an irq handler
irq_fn irq_set_handler(int num, irq_fn irq);

#endif
