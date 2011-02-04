/*  omap3-scd.h: OMAP3 system companion device registers (aka TPS65950)

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
#ifndef _OMAP3_SCD_H
#define _OMAP3_SCD_H

// system companion device i2c addresses
#define SCDA_ID1 0x48
#define SCDA_ID2 0x49
#define SCDA_ID3 0x4a
#define SCDA_ID4 0x4b

// aliases to addresses
#define SCDA_AUDIO_VOICE 0x49
#define SCDA_LED 0x4a

// Registers in 0x4a
#define SCD_LEDEN 0xee
#define SCD_PWMAON 0xef
#define SCD_PWMAOFF 0xf0
#define SCD_PWMBON 0xf1
#define SCD_PWMBOFF 0xf2

// SCD_LEDEN bits
#define SCD_PWM_LENGTHB (1<<7)
#define SCD_PWM_LENGTHA (1<<6)
#define SCD_LEDBPWM (1<<5)
#define SCD_LEDAPWM (1<<4)
#define SCD_LEDBEXT (1<<3)
#define SCD_LEDAEXT (1<<2)
#define SCD_LEDBON (1<<1)
#define SCD_LEDAON (1<<0)

// AUDIO_VOICE registers
#define AV_CODEC_MODE 0x00000001
#define AV_OPTION 0x00000002
#define AV_MICBIAS_CTL 0x00000004
#define AV_ANAMICL 0x00000005
#define AV_ANAMICR 0x00000006
#define AV_AVADC_CTL 0x00000007
#define AV_ADCMICSEL 0x00000008
#define AV_DIGMIXING 0x00000009
#define AV_ATXL1PGA 0x0000000A
#define AV_ATXR1PGA 0x0000000B
#define AV_AVTXL2PGA 0x0000000C
#define AV_AVTXR2PGA 0x0000000D
#define AV_AUDIO_IF 0x0000000E
#define AV_VOICE_IF 0x0000000F
#define AV_ARXR1PGA 0x00000010
#define AV_ARXL1PGA 0x00000011
#define AV_ARXR2PGA 0x00000012
#define AV_ARXL2PGA 0x00000013
#define AV_VRXPGA 0x00000014
#define AV_VSTPGA 0x00000015
#define AV_VRX2ARXPGA 0x00000016
#define AV_AVDAC_CTL 0x00000017
#define AV_ARX2VTXPGA 0x00000018
#define AV_ARXL1_APGA_CTL 0x00000019
#define AV_ARXR1_APGA_CTL 0x0000001A
#define AV_ARXL2_APGA_CTL 0x0000001B
#define AV_ARXR2_APGA_CTL 0x0000001C
#define AV_ATX2ARXPGA 0x0000001D
#define AV_BT_IF 0x0000001E
#define AV_BTPGA 0x0000001F
#define AV_BTSTPGA 0x00000020
#define AV_EAR_CTL 0x00000021
#define AV_HS_SEL 0x00000022
#define AV_HS_GAIN_SET 0x00000023
#define AV_HS_POPN_SET 0x00000024
#define AV_PREDL_CTL 0x00000025
#define AV_PREDR_CTL 0x00000026
#define AV_PRECKL_CTL 0x00000027
#define AV_PRECKR_CTL 0x00000028
#define AV_HFL_CTL 0x00000029
#define AV_HFR_CTL 0x0000002A
#define AV_ALC_CTL 0x0000002B
#define AV_ALC_SET1 0x0000002C
#define AV_ALC_SET2 0x0000002D
#define AV_BOOST_CTL 0x0000002E
#define AV_SOFTVOL_CTL 0x0000002F
#define AV_DTMF_FREQSEL 0x00000030
#define AV_DTMF_TONEXT1H 0x00000031
#define AV_DTMF_TONEXT1L 0x00000032
#define AV_DTMF_TONEXT2H 0x00000033
#define AV_DTMF_TONEXT2L 0x00000034
#define AV_DTMF_TONOFF 0x00000035
#define AV_DTMF_WANONOFF 0x00000036
#define AV_CODEC_RX_SCRAMBLE_H 0x00000037
#define AV_CODEC_RX_SCRAMBLE_M 0x00000038
#define AV_CODEC_RX_SCRAMBLE_L 0x00000039
#define AV_APLL_CTL 0x0000003A
#define AV_DTMF_CTL 0x0000003B
#define AV_DTMF_PGA_CTL2 0x0000003C
#define AV_DTMF_PGA_CTL1 0x0000003D
#define AV_MISC_SET_1 0x0000003E
#define AV_PCMBTMUX 0x0000003F
#define AV_RX_PATH_SEL 0x00000043
#define AV_VDL_APGA_CTL 0x00000044
#define AV_VIBRA_CTL 0x00000045
#define AV_VIBRA_SET 0x00000046
#define AV_ANAMIC_GAIN 0x00000048
#define AV_MISC_SET_2 0x00000049

// CODEC_MODE
#define APPL_RATE_8 (0<<4)
#define APPL_RATE_11_025 (1<<4)
#define APPL_RATE_12 (2<<4)
#define APPL_RATE_16 (4<<4)
#define APPL_RATE_22_05 (5<<4)
#define APPL_RATE_34 (6<<4)
#define APPL_RATE_32 (8<<4)
#define APPL_RATE_44_1 (9<<4)
#define APPL_RATE_48 (0xa<<4)
#define APPL_RATE_96 (0xe<<4)
#define CODECPDZ 2
#define OPT_MODE 1
// OPTION
#define ARXR2_EN (1<<7)
#define ARXL2_EN (1<<6)
#define ARXR1_EN (1<<5)
#define ARXL1_VRX_EN (1<<4)
#define ATXR2_VTXR_EN (1<<3)
#define ATXL2_VTXL_EN (1<<2)
#define ATXR1_EN (1<<1)
#define ATXL1_EN (1<<0)
// RX_PATH_SET
#define RXL2_SEL (1<<5)
#define RXR2_SEL (1<<4)
#define B_RXL1_SEL 2
#define B_RXR1_SEL 0
// APLL_CTL
#define APLL_EN (1<<4)
#define APLL_INFREQ_26 6
#define APLL_INFREQ_19 5
#define APLL_INFREQ_38_4 0xf
// AVDAC_CTL
#define VDAC_EN (1<<4)
#define ADACL2_EN (1<<3)
#define ADACR2_EN (1<<2)
#define ADACL1_EN (1<<1)
#define ADACR1_EN (1<<0)
// AVADC_CTL
#define ADCL_EN (1<<3)
#define AVADC_CLK_PRIORITY (1<<2)
#define ADCR_EN (1<<1)
// ARX[LR]1_AGPA_CTL
#define B_ARX1_GAIN_SET 3
#define ARX1_GAIN_SET_MAX 0xf
#define ARX1_FM_EN (1<<2)
#define ARX1_DA_EN (1<<1)
#define ARX1_PDZ (1<<0)
// SOFTVOL_CTL
#define B_SOFTVOL_SET 5
#define SOFTVOL_EN (1<<0)
// AUDIO_IF reg
#define AIF_SLAVE_EN (1<<7)
#define DATA_WIDTH_16 (0<<5)
#define DATA_WIDTH_A16 (2<<5)
#define DATA_WIDTH_A24 (3<<5)
#define AIF_FORMAT_CODEC (0<<3)
#define AIF_FORMAT_RJUST (1<<3)
#define AIF_FORMAT_LJUST (2<<3)
#define AIF_FORMAT_TDM (3<<3)
#define AIF_TRI_EN (1<<2)
#define CLK256FS_EN (1<<1)
#define AIF_EN (1<<0)
// HS_SEL
#define HSR_INV_EN (1<<7)
#define HS_OUTLOW_EN (1<<6)
#define HSOR_AR2_EN (1<<5)
#define HSOR_AR1_EN (1<<4)
#define HSOR_VOICE_EN (1<<3)
#define HSOL_AL2_EN (1<<2)
#define HSOL_AL1_EN (1<<1)
#define HSOL_VOICE_EN (1<<0)
// ARX[LR][12]PGA
#define B_ARXPGA_FGAIN 0
#define B_ARXPGA_CGAIN 6
// HS_GAIN_SET
#define HS_GAIN_OFF 0
#define HS_GAIN_6DB 1
#define HS_GAIN_0DB 2
#define HS_GAIN_N6DB 3
#define B_HSR_GAIN 2
#define B_HSL_GAIN 0
// ANAMICL
#define CNCL_OFFSET_START (1<<7)
#define OFFSET_CNCL_SEL_RX1 (0<<5)
#define OFFSET_CNCL_SEL_RX2 (1<<5)
#define OFFSET_CNCL_SEL_RXV (2<<5) 
#define OFFSET_CNCL_SEL_RXALL (3<<5)
#define MICAMPL_EN (1<<4)
#define CKMIC_EN (1<<3)
#define AUXL_EN (1<<2)
#define HSMIC_EN (1<<1)
#define MAINMIC_EN (1<<0)
// ANAMICR
#define MICAMPR_EN (1<<4)
#define AUXR_EN (1<<2)

// MISC_SET_1
#define CLK64_EN (1<<7)
#define SCRAMBLE_EN (1<<6)
#define FMLOOP_EN (1<<5)
#define SMOOTH_ANAVOL_EN (1<<1)
#define DIGMIC_LR_SWAP_EN (1<<0)
// HS_POPN_SET
#define VMID_EN (1<<6)
#define EXTMUTE (1<<5)
// for 26Mhz sysclock
#define RAMP_DELAY_20ms (0<<2)
#define RAMP_DELAY_40ms (1<<2)
#define RAMP_DELAY_81ms (2<<2)
#define RAMP_DELAY_161ms (3<<2)
#define RAMP_DELAY_323ms (4<<2)
#define RAMP_DELAY_645ms (5<<2)
#define RAMP_DELAY_1291ms (6<<2)
#define RAMP_DELAY_2581ms (7<<2)
#define RAMP_EN 1

#endif
