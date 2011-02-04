
#ifndef _OMAP3_GPIO_H
#define _OMAP3_GPIO_H

/* ********************************************************************** */
/* GPIO registers */
#define GPIO1_BASE ((void *)0x48310000)
#define GPIO2_BASE ((void *)0x49050000)
#define GPIO3_BASE ((void *)0x49052000)
#define GPIO4_BASE ((void *)0x49054000)
#define GPIO5_BASE ((void *)0x49056000)
#define GPIO6_BASE ((void *)0x49058000)

/* (incomplete) */
#define GPIO_OE 0x034
#define GPIO_DATAIN 0x038
#define GPIO_DATAOUT 0x03C
#define GPIO_SETDATAOUT 0x094

#endif
