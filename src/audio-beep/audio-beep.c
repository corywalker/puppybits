
/*
  This sets up the audio chip for stereo in and out,
  then plays some sounds and displays the waveform from
  line-in.
*/

#include "pbits.h"
#include "graphics.h"

#include "omap3-system.h"
#include "omap3-scd.h"
#include "omap3-prcm.h"
#include "omap3-mcbsp.h"

/* video setup */
#define FBADDR ((void *)0x88000000)
#define WIDTH 1024
#define HEIGHT 768
// sample rate
#define SRATE 48000
// wavelength of phasing test tone
#define WVLEN (SRATE / 440)

static struct RastPort *rp;

/* audio register access */
static inline void aregw(uint8_t addr, uint8_t val) {
	bus_i2c_write8(I2C1_BASE, SCDA_AUDIO_VOICE, addr, val);
}
static inline uint8_t aregr(uint8_t addr) {
	uint8_t buffer[1];

	bus_i2c_read(I2C1_BASE, SCDA_AUDIO_VOICE, addr, buffer, 1);

	return buffer[0];
}

void dump_audio(void) {
	uint8_t buffer[256];
	int i = 0;

	dprintf("audio regs:\n");
	while (i < 127) {
		int j = i + 128;

		bus_i2c_read(I2C1_BASE, SCDA_AUDIO_VOICE, i, buffer, 128);
		for (;i<j;i++) {
			if ((i & 15) == 0) dprintf("\n%02x:", i);
			dprintf(" %02x", buffer[i]);
		}
		i = j;
	}
	dprintf("\n");
}

void setup_audio(void) {

	// turn it off to configure
	aregw(AV_CODEC_MODE, 0);
	aregw(AV_CODEC_MODE, APPL_RATE_8 | OPT_MODE);

	// data and routing

	// audio audio 1 = in (transmit) audio 2 = out (receive)
	aregw(AV_OPTION, ARXR2_EN | ARXL2_EN | ATXR1_EN | ATXL1_EN);

	// route rx 1 to pga 1
	aregw(AV_RX_PATH_SEL,  (0 << B_RXL1_SEL) | (0 << B_RXR1_SEL));

	// set system clock speed and enable the local clock
	aregw(AV_APLL_CTL, APLL_EN | APLL_INFREQ_26);

	// turn on left and right dacs
	//aregw(AV_AVDAC_CTL, ADACL1_EN | ADACR1_EN);
	aregw(AV_AVDAC_CTL, ADACL2_EN | ADACR2_EN);
	//aregw(AV_AVDAC_CTL, 0x0c);
	
	// turn on left and right output
	aregw(AV_ARXL2_APGA_CTL, ARX1_PDZ | ARX1_DA_EN | (0x06 << B_ARX1_GAIN_SET));
	aregw(AV_ARXR2_APGA_CTL, ARX1_PDZ | ARX1_DA_EN | (0x06 << B_ARX1_GAIN_SET));
	// headset output select
	aregw(AV_HS_SEL, HSOL_AL2_EN | HSOR_AR2_EN);
	// left/right digital gain
	aregw(AV_ARXL2PGA, 0x30 | (3<<6));		/* 0dB */
	aregw(AV_ARXR2PGA, 0x30 | (3<<6));		/* 0dB */
	
	// set headset left/right gain (balance) to 0db each
	aregw(AV_HS_GAIN_SET, 0x0a);

	// audio interface.  master mode, i2s format, 16 bit data
	aregw(AV_AUDIO_IF, AIF_FORMAT_CODEC | DATA_WIDTH_16 | AIF_EN);

	// volume/effects
	// set base boost effect
	aregw(AV_BOOST_CTL, 0);
	// turn on soft volume delay and enable
	//	aregw(AV_SOFTVOL_CTL, SOFTVOL_EN | (3 << B_SOFTVOL_SET));
	// anti-pop volume changes
	//	aregw(AV_MISC_SET_1, SMOOTH_ANAVOL_EN);
	// anti-pop paramters
	aregw(AV_HS_POPN_SET, VMID_EN | RAMP_DELAY_161ms | RAMP_EN);



	// tramsit settings (audio in)
	// left/right gain
	aregw(AV_ATXL1PGA, 0x0);
	aregw(AV_ATXR1PGA, 0x0);

	// left/right ADC, and choose as TXx1
	aregw(AV_AVADC_CTL, ADCL_EN | ADCR_EN);
	aregw(AV_ADCMICSEL, 0);

	// line in on beagle comes from AUX, also enable anti-pop and start
	aregw(AV_ANAMICL, 0x34 | AUXL_EN | OFFSET_CNCL_SEL_RXALL | CNCL_OFFSET_START);
	aregw(AV_ANAMICR, 0x14 | AUXR_EN);
	aregw(AV_ANAMIC_GAIN, (0<<3)|0);

	// DEBUG
	//  loop transmit - receive
	//aregw(AV_ATX2ARXPGA, (7 << 3) | 7);


	// now power it up, with sample rate and option 1 (2x stereo audio paths in and out)
	aregw(AV_CODEC_MODE, APPL_RATE_8 | 0 | OPT_MODE);
	aregw(AV_CODEC_MODE, APPL_RATE_48 | CODECPDZ | OPT_MODE);

	// Supposedly one must wait, but I don't understand why, the hardware
	// turns it off once done

	// Wait for the anti-pop start to complete
	//while (aregr(AV_ANAMICL) & CNCL_OFFSET_START)
	//	;
}

/* initi mcbsp2 for rx/tx of audio in stereo i2s format */
void setup_mcbsp2(void) {
	// set clock for mcbsp2 - PRCM functional clock
	reg32s(CONTROL_GENERAL_BASE, CONTROL_DEVCONF0, CONTROL_MCBSP2_CLKS, 0);

	reg32s(CM_PER_BASE, CM_FCLKEN_PER, CM_PER_EN_MCBSP2, ~0);
	reg32s(CM_PER_BASE, CM_ICLKEN_PER, CM_PER_EN_MCBSP2, ~0);

	// reset transmitter & receiver */
	reg32w(MCBSP2_BASE, MCBSPLP_SPCR2_REG, MCBSP_FREE);
	reg32w(MCBSP2_BASE, MCBSPLP_SPCR1_REG, 0);

	// make sure gpio mode is off
	reg32w(MCBSP2_BASE, MCBSPLP_PCR_REG, 0);

	// set xmit format
	reg32w(MCBSP2_BASE, MCBSPLP_XCR1_REG,
	       (MCBSP_WDLEN_16 << MCBSPB_XWDLEN1) /* 16 bits in phase 1 */
	       | (0 << MCBSPB_XFRLEN1)	  /* frame length1 = 1 word */
	       );
	reg32w(MCBSP2_BASE, MCBSPLP_XCR2_REG,
	       MCBSP_XPHASE				 /* 2 phase */
	       | 1			     /* 1 bit delay for i2s */
	       | (0 << MCBSPB_XFRLEN2)			/* frrame length = 1 word */
	       | (MCBSP_WDLEN_16 << MCBSPB_XWDLEN2) /* 16 bit words in phase 2 */
	       );

	// set receive format
	reg32w(MCBSP2_BASE, MCBSPLP_RCR1_REG,
	       (MCBSP_WDLEN_16 << MCBSPB_RWDLEN1) /* 16 bits in phase 1 */
	       | (0 << MCBSPB_RFRLEN1)	  /* frame length1 = 1 word */
	       );
	reg32w(MCBSP2_BASE, MCBSPLP_RCR2_REG,
	       MCBSP_RPHASE				 /* 2 phase */
	       | 1			     /* 1 bit delay for i2s */
	       | (0 << MCBSPB_RFRLEN2)			/* frrame length = 1 word */
	       | (MCBSP_WDLEN_16 << MCBSPB_RWDLEN2) /* 16 bit words in phase 2 */
	       );

	// disable DMA for now ...
	reg32w(MCBSP2_BASE, MCBSPLP_XCCR_REG, 0);
	
	//reg32w(MCBSP2_BASE, MCBSPLP_THRSH2_REG, 0x280);	/* tx threshold */
	reg32w(MCBSP2_BASE, MCBSPLP_THRSH2_REG, 0);	/* tx threshold */

	reg32w(MCBSP2_BASE, MCBSPLP_THRSH1_REG, 2);	/* rx threshold */

	reg32w(MCBSP2_BASE, MCBSPLP_PCR_REG,
	       0
	       | MCBSP_CLKXP
	       | MCBSP_CLKRP
	       );


	// 'wait for 2 clock cycles' (96Mhz clock?)

	//ksleep(1000);

	// take it out of reset ...
	reg32w(MCBSP2_BASE, MCBSPLP_SPCR2_REG, MCBSP_FREE | MCBSP_XRST);
	reg32w(MCBSP2_BASE, MCBSPLP_SPCR1_REG, MCBSP_RRST);
}

void showsample(int ds) {
	static int x = 0;
	static int div = 0;

	int d = (-ds) * 128 / 32768 + 768/4;

	moveTo(rp, x, d);
	drawPixel(rp);

	div++;
	if (div >= 32) {
		div = 0;
		x++;
		if (x >= WIDTH) {
			x = 0;
			setColour(rp, 0x3e31a2);
			moveTo(rp, 0, 0);
			drawRect(rp, WIDTH, HEIGHT);
			setColour(rp, 0xffffff);
		}
	}
}

// A-G is note (G is lower octave).
// 0-9 duration of *next* note in half-notes
char mary[] =
	"BAGABB4BAA4ABD4DBAGABBBBAABA7G"
	"BAGABB4BAA4ABD4DBAGABBBBAABA7G";

// wavelengths of various basic notes
int ftable[] = {
	(int)(SRATE / 220.000),	 /* a3 */
	(int)(SRATE / 246.942),
	(int)(SRATE / 261.626),	 /* c3 */
	(int)(SRATE / 293.665),
	(int)(SRATE / 329.628),
	(int)(SRATE / 349.228),

	(int)(SRATE / 195.998),	 /* g2 */
};

#define MAX 32767
#define SUS ((unsigned int )(32767 * 0.8))

/* envelope details */
struct env {
	unsigned int target;
	unsigned int time;
};

/* adsr of our note */
struct env adsr[] = {
	{ MAX, ((unsigned int)(SRATE * 0.05)) },
	{ SUS, ((unsigned int)(SRATE * 0.05)) },
	{ SUS, ((unsigned int)(SRATE * 0.2)) },
	{ 0, ((unsigned int)(SRATE * 0.02)) }
};

/* play one note with wavelength wvlen to channels lr (bits [1:0]) for sustain length len */
void playnote(int wvlen, int lr, int len) {
	int i;
	int ampstart = 0;
	int st = 0;

	for (i=0;i<4;i++) {
		int at = adsr[i].target;
		int dur = adsr[i].time;
		int j;

		// scale 'sustain' for note length
		if (i == 2)
			dur = dur * len >> 8;

		for (j=0;j<dur;j++) {
			int amp = (at-ampstart) * j / dur + ampstart;
			int v = (st*2 - wvlen) * amp / wvlen;

			showsample(v);

			// send out 2 samples, left/right
			while ((reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG) & MCBSP_XRDY) == 0)
				;
			reg32w(MCBSP2_BASE, MCBSPLP_DXR_REG, lr & 1 ? v : 0);
			while ((reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG) & MCBSP_XRDY) == 0)
				;
			reg32w(MCBSP2_BASE, MCBSPLP_DXR_REG, lr & 2 ? v : 0);

			st += 1;
			if (st >= wvlen)
				st = 0;
		}
		ampstart = at;
	}
}

// play a little ditty directly to the port
void playmary(void) {
	char *np = mary;
	char n;
	int lr = 1;
	int len = 256;

	while ( (n = *np++) ) {
		if (n >= '0' && n <= '9') {
			len = (n - '0') * 128;
		} else {
			dputc(n);
			playnote(ftable[n - 'A'], 3, len);
			lr ^= 3;
			len = 256;
		}
	}
}

int main(void) {
	// need the cache on otherwise we're too slow
	mmu_simple_init();

	video_init(1280, 1024);
	rp = graphics_init(FBADDR, WIDTH, HEIGHT, BM_RGB16);
	omap_attach_framebuffer(0, rp->drawable.bitmap);

	moveTo(rp, 0, 0);
	setColour(rp, 0x3e31a2);
	drawRect(rp, WIDTH, HEIGHT);
	setColour(rp, 0xffffff);
	int x = 0;

	dprintf("setup mcbsp\n");
	setup_mcbsp2();

	//dump_audio();

	dprintf("setup audio\n");
	setup_audio();

	dump_audio();

	// our intro ditty
	playmary();

	// manually send a modulated sawtooth wave to the sound chip
	int i = 0;
	int frames = 0;
	int phase = 0;
	int pdir = 1;
	int side = 1;
	int phaselen = 0;
	while (1) {
		uint32_t v = (i*2-WVLEN) * 32767 / WVLEN + 32768;
		int timeout = 1000000;

		if (i > phase) v = 0;

		// handle read & display
		while (reg32r(MCBSP2_BASE, MCBSPLP_SPCR1_REG) & MCBSP_RRDY) {
			int16_t ds;
			int32_t d;

			ds = reg32r(MCBSP2_BASE, MCBSPLP_DRR_REG);
			d = ds * 128 / 32768 + 768/4;
			moveTo(rp, x, d);
			drawPixel(rp);

			ds = reg32r(MCBSP2_BASE, MCBSPLP_DRR_REG);
			d = ds * 128 / 32768 + 768*3/4;
			moveTo(rp, x, d);
			drawPixel(rp);
		}

		if ((frames&7) == 0)
			x++;
		if (x >= WIDTH) {
			x = 0;
			moveTo(rp, 0, 0);
			setColour(rp, 0x3e31a2);
			drawRect(rp, WIDTH, HEIGHT);
			setColour(rp, 0xffffff);
			moveTo(rp, 450, 20);
			drawString(rp, "Signal from line-in");
		}
		
		// send left
		while ((reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG) & MCBSP_XRDY) == 0)
			if (timeout-- == 0) {
				dprintf("timeout waiting for xready after %d frames\n", frames);
				dprintf(" reg = %08x\n", reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG));
				reg32s(MCBSP2_BASE, MCBSPLP_SPCR2_REG, 1, 0);
				ksleep(100);
				reg32s(MCBSP2_BASE, MCBSPLP_SPCR2_REG, 1, 1);
				timeout = 1000000;
				frames = 0;
			}

		reg32w(MCBSP2_BASE, MCBSPLP_DXR_REG, side > 0 ? v : 0);

		// send right
		while ((reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG) & MCBSP_XRDY) == 0)
			if (timeout-- == 0) {
				dprintf("timeout waiting for xready after %d frames\n", frames);
				dprintf(" reg = %08x\n", reg32r(MCBSP2_BASE, MCBSPLP_SPCR2_REG));
				reg32s(MCBSP2_BASE, MCBSPLP_SPCR2_REG, 1, 0);
				ksleep(100);
				reg32s(MCBSP2_BASE, MCBSPLP_SPCR2_REG, 1, 1);
				timeout = 1000000;
				frames = 0;
			}

		reg32w(MCBSP2_BASE, MCBSPLP_DXR_REG, side > 0 ? 0 : v);

		i++;

		frames++;

		if (i >= WVLEN) {
			i = 0;
		}

		phaselen++;
		if (phaselen > WVLEN * 4) {
			phaselen = 0;
			phase += pdir;
			if (phase < 0 || phase >= WVLEN) {
				pdir = -pdir;
				phase += pdir;
				if (pdir > 0)
					side = -side;
			}
			if (pdir > 0)
				LEDS_ON(LED1);
			else
				LEDS_OFF(LED1);

		}
	}
}
