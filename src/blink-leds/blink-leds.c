
/*
  Simple program that blinks LED1
*/

#include "pbits.h"
#include "gpio.h"

#include "omap3-system.h"

int main(void) {
	// need the cache on otherwise we're too slow
	mmu_simple_init();

	dprintf("Entering main loop of blink-leds.\n");
	
	int delay = 100000;
	
	while(1) {
	    LEDS_ON_(LED0);
	    LEDS_OFF_(LED1);
	    ksleep(delay);
	    LEDS_OFF_(LED0);
	    LEDS_ON_(LED1);
	    ksleep(delay);
	    
        if(omap_get_gpio(BTN_USR))
            delay = 10000;
        else
            delay = 100000;
	}
}
