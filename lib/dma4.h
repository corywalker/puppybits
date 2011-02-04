/*  dma4.h: Defines for DMA4 registers

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
#ifndef _DMA4_H
#define _DMA4_H

#include <stdint.h>

struct dma4channel {
  uint32_t CCR;
  uint32_t CLNK_CTRL;
  uint32_t CICR;
  uint32_t CSR;
  uint32_t CSDP;
  uint32_t CEN;
  uint32_t CFN;
  uint32_t CSSA;
  uint32_t CDSA;
  uint32_t CSEl;
  uint32_t CSFl;
  uint32_t CDEl;
  uint32_t CDFl;
  uint32_t CSAC;
  uint32_t CDAC;
  uint32_t CCEN;
  uint32_t CCFN;
  uint32_t COLOR;
  uint32_t dummy[6];
};

struct dma4 {
  uint32_t dummy0;
  uint32_t dummy1;
  uint32_t irqstatus_li[4];
  uint32_t irqenable_li[4];
  uint32_t sysstatus;
  uint32_t ocp_sysconfig;
  uint32_t dummy2[13];
  uint32_t caps_0;
  uint32_t dummy3;
  uint32_t caps_2;
  uint32_t caps_3;
  uint32_t caps_4;
  uint32_t GCR;
  uint32_t dummy4;
  struct dma4channel chan[32];
};

#define DMA4_DATA_TYPE_8 (0)
#define DMA4_DATA_TYPE_16 (1)
#define DMA4_DATA_TYPE_32 (2)

#define DMA4_DST_AMODE_POSTINC (1<<14)
#define DMA4_DST_AMODE_SIDX (2<<14)
#define DMA4_DST_AMODE_DIDX (3<<14)
#define DMA4_DST_PACKED (1<<13)

#define DMA4_SRC_AMODE_POSTINC (1<<12)
#define DMA4_SRC_AMODE_SIDX (2<<12)
#define DMA4_SRC_AMODE_DIDX (3<<12)
#define DMA4_SRC_PACKED (1<<11)

#define DMA4_CONST_FILL_ENABLE (1<<16)
#define DMA4_ENABLE (1<<7)

// TODO: add some basic dma functions

#endif
