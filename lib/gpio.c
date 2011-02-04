/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This work is derived from the linux 2.6.27 kernel source
 * To fetch, use the kernel repository
 * git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux-2.6.git
 * Use the v2.6.27 tag.
 *
 * Below is the original's header including its copyright
 *
 *  linux/arch/arm/plat-omap/gpio.c
 *
 * Support functions for OMAP GPIO
 *
 * Copyright (C) 2003-2005 Nokia Corporation
 * Written by Juha Yrjölä <juha.yrjola@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "gpio.h"
#include "pbits.h"

// TODO: TAKE THESE OUT
#define GPIO1_BASE ((void *)0x48310000)
#define GPIO2_BASE ((void *)0x49050000)
#define GPIO3_BASE ((void *)0x49052000)
#define GPIO4_BASE ((void *)0x49054000)
#define GPIO5_BASE ((void *)0x49056000)
#define GPIO6_BASE ((void *)0x49058000)
#define	EINVAL		22	/* Invalid argument */

#define __raw_readl(a)			(*(volatile unsigned int *)(a))
#define __raw_writel(v,a)		(*(volatile unsigned int *)(a) = (v))

static struct gpio_bank gpio_bank_34xx[6] = {
	{ GPIO1_BASE, METHOD_GPIO_24XX },
	{ GPIO2_BASE, METHOD_GPIO_24XX },
	{ GPIO3_BASE, METHOD_GPIO_24XX },
	{ GPIO4_BASE, METHOD_GPIO_24XX },
	{ GPIO5_BASE, METHOD_GPIO_24XX },
	{ GPIO6_BASE, METHOD_GPIO_24XX },
};

static struct gpio_bank *gpio_bank = &gpio_bank_34xx[0];

static inline struct gpio_bank *get_gpio_bank(int gpio)
{
	return &gpio_bank[gpio >> 5];
}

static inline int get_gpio_index(int gpio)
{
	return gpio & 0x1f;
}

static inline int gpio_valid(int gpio)
{
	if (gpio < 0)
		return -1;
	if (gpio < 192)
		return 0;
	return -1;
}

static int check_gpio(int gpio)
{
	if (gpio_valid(gpio) < 0) {
		dprintf("ERROR : check_gpio: invalid GPIO %d\n", gpio);
		return -1;
	}
	return 0;
}

static void _set_gpio_direction(struct gpio_bank *bank, int gpio, int is_input)
{
	void *reg = bank->base;
	uint32_t l;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		reg += OMAP24XX_GPIO_OE;
		break;
	default:
		return;
	}
	l = __raw_readl(reg);
	if (is_input)
		l |= 1 << gpio;
	else
		l &= ~(1 << gpio);
	__raw_writel(l, reg);
}

void omap_set_gpio_direction(int gpio, int is_input)
{
	struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return;
	bank = get_gpio_bank(gpio);
	_set_gpio_direction(bank, get_gpio_index(gpio), is_input);
}

static void _set_gpio_dataout(struct gpio_bank *bank, int gpio, int enable)
{
	void *reg = bank->base;
	uint32_t l = 0;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		if (enable)
			reg += OMAP24XX_GPIO_SETDATAOUT;
		else
			reg += OMAP24XX_GPIO_CLEARDATAOUT;
		l = 1 << gpio;
		break;
	default:
		dprintf("omap3-gpio unknown bank method %s %d\n",
		       __FILE__, __LINE__);
		return;
	}
	__raw_writel(l, reg);
}

void omap_set_gpio_dataout(int gpio, int enable)
{
	struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return;
	bank = get_gpio_bank(gpio);
	_set_gpio_dataout(bank, get_gpio_index(gpio), enable);
}

int omap_get_gpio_datain(int gpio)
{
	struct gpio_bank *bank;
	void *reg;

	if (check_gpio(gpio) < 0)
		return -EINVAL;
	bank = get_gpio_bank(gpio);
	reg = bank->base;
	switch (bank->method) {
	case METHOD_GPIO_24XX:
		reg += OMAP24XX_GPIO_DATAIN;
		break;
	default:
		return -EINVAL;
	}
	return (__raw_readl(reg)
			& (1 << get_gpio_index(gpio))) != 0;
}

static void _reset_gpio(struct gpio_bank *bank, int gpio)
{
	_set_gpio_direction(bank, get_gpio_index(gpio), 1);
}

void omap_set_gpio(int gpio, int enable)
{
    if (!omap_request_gpio(gpio)) {
        omap_set_gpio_direction(gpio, 0);
        omap_set_gpio_dataout(gpio, enable);
        omap_free_gpio(gpio);
    }
}

int omap_get_gpio(int gpio)
{
    int value = -1;
    if(!omap_request_gpio(gpio)) {
        omap_set_gpio_direction(gpio, 1);
        value = omap_get_gpio_datain(gpio);
        omap_free_gpio(gpio);
    }
    return value;
}

int omap_request_gpio(int gpio)
{
	if (check_gpio(gpio) < 0)
		return -EINVAL;

	return 0;
}

void omap_free_gpio(int gpio)
{
	struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return;
	bank = get_gpio_bank(gpio);

	_reset_gpio(bank, gpio);
}
