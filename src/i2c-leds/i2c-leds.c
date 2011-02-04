
#include "pbits.h"

#include "omap3-system.h"
#include "omap3-scd.h"

// Sine approximated as a parabola, only valid for [0, pi]
// see: http://www.devmaster.net/forums/showthread.php?t=5784
#define pi ((float)3.14159265)
#define fabs(f) ((f) < 0 ? -(f) : (f))
static float sine(float x)
{
	const float B = 4/pi;
	const float C = -4/(pi*pi);

	float y = B * x + C * x * fabs(x);

	//  const float Q = 0.775;
        const float P = 0.225;

        y = P * (y * fabs(y) - y) + y;   // Q * y + P * y * abs(y)

	return y;
}

// pulses the led connected to the TPS65950

void pulse_leds(void) {
	uint32_t base = I2C1_BASE;
	uint8_t buffer[5];
	int i;

	dprintf("reading LED/PWM registers\n");
	bus_i2c_read(base, SCDA_LED, SCD_LEDEN, buffer, 5);
	for (i=0;i<5;i++) {
		dprintf(" %02x", buffer[i]);
	}
	dprintf("\n");

	// make sure it's on
	bus_i2c_write8(base, SCDA_LED, SCD_LEDEN, SCD_LEDBON | SCD_LEDBPWM);

	float a = 0.0;
	unsigned char v = 0;
	while (1) {
	    int delay = 240;
	    v += 1;
		bus_i2c_write8(base, SCDA_LED, SCD_PWMBOFF, v);
		ksleep(delay);
	}
}

int main(int argc, char **argv) {
	bus_i2c_init();
	pulse_leds();

	return 0;
}
