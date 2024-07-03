#ifndef SPACEMOUSEHANDLE_H
#define SPACEMOUSEHANDLE_H

#include "Wire.h"
#include "LDC16xx.h"
#include "variables.h"


#define BOOTLOADER_ADDRESS 0x1FFFC400 // address of the bootloader ROM
#define LDC_HONE_DEADBAND 20 // the magnitude of all channel readings should be less than this before LDC honing begins
#define LDC_HONE_PERIOD 100 // this many samples should pass within LDC_HONE_DEADBAND before LDC honing executes
#define X_SCALE_FACTOR 120 // Scaling factors to bring the values up to us the full range of int16_t so other programs play nice
#define Y_SCALE_FACTOR 120
#define Z_SCALE_FACTOR 35
#define RX_SCALE_FACTOR 30
#define RY_SCALE_FACTOR 30
#define RZ_SCALE_FACTOR 50


LDC_configReg default_config[] = {
    { LDC16xx_CLOCK_DIVIDERS_CH0,   0x5002 }, // get weird behavior with other dividers like 1002, just stick with this for now
    { LDC16xx_CLOCK_DIVIDERS_CH1,   0x5002 },
    { LDC16xx_CLOCK_DIVIDERS_CH2,   0x5002 },
    { LDC16xx_CLOCK_DIVIDERS_CH3,   0x5002 },
    { LDC16xx_SETTLECOUNT_CH0,      0x0040 },
    { LDC16xx_SETTLECOUNT_CH1,      0x0040 },
    { LDC16xx_SETTLECOUNT_CH2,      0x0040 },
    { LDC16xx_SETTLECOUNT_CH3,      0x0040 },
    { LDC16xx_RCOUNT_CH0,           0x1fff },
    { LDC16xx_RCOUNT_CH1,           0x1fff },
    { LDC16xx_RCOUNT_CH2,           0x1fff },
    { LDC16xx_RCOUNT_CH3,           0x1fff },
    { LDC16xx_DRIVE_CURRENT_CH0,    0xD000 },
    { LDC16xx_DRIVE_CURRENT_CH1,    0xD000 },
    { LDC16xx_DRIVE_CURRENT_CH2,    0xD000 },
    { LDC16xx_DRIVE_CURRENT_CH3,    0xD000 },
    { LDC16xx_ERROR_CONFIG,         0x0001 },
    { LDC16xx_MUX_CONFIG,           LDC16xx_BITS_DEGLITCH_3_3Mhz | LDC16xx_BITS_AUTOSCAN_EN | LDC16xx_BITS_RR_SEQUENCE_CH0_CH1_CH2_CH3 },
    { LDC16xx_CONFIG,               LDC16xx_BITS_ACTIVE_CHAN_CH0 // select channel 0
                                  | LDC16xx_BITS_AUTO_AMP_DIS // if 0, IDRIVE constantly adjusts
                                  | LDC16xx_BITS_RP_OVERRIDE_EN // if 0, IDRIVE auto calibrates on sensor startup
                                  | LDC16xx_BITS_INTB_DIS // disable intb pin
                                  | LDC16xx_BITS_REF_CLK_SRC // 40MHz ext clock
                                  // | LDC16xx_BITS_SLEEP_MODE_EN // start in sleep mode
                                  }, 
};

LDC_configReg SLEEP_CONFIG_config = { LDC16xx_CONFIG, LDC16xx_BITS_ACTIVE_CHAN_CH0 | LDC16xx_BITS_AUTO_AMP_DIS | LDC16xx_BITS_INTB_DIS | LDC16xx_BITS_SLEEP_MODE_EN};
LDC_configReg AWAKE_CONFIG_config = { LDC16xx_CONFIG, LDC16xx_BITS_ACTIVE_CHAN_CH0 | LDC16xx_BITS_AUTO_AMP_DIS | LDC16xx_BITS_INTB_DIS};

#define LDC_CONFIG_SIZE sizeof(default_config)/sizeof(LDC_configReg)



// Channel data and cal data variables
uint32_t ldc1_ch0 = 0;
uint32_t ldc1_ch1 = 0;
uint32_t ldc1_ch2 = 0;
uint32_t ldc1_ch3 = 0;
uint32_t ldc2_ch0 = 0;
uint32_t ldc2_ch1 = 0;
int32_t ldc1_ch0_dif = 0;
int32_t ldc1_ch1_dif = 0;
int32_t ldc1_ch2_dif = 0;
int32_t ldc1_ch3_dif = 0;
int32_t ldc2_ch0_dif = 0;
int32_t ldc2_ch1_dif = 0;
uint32_t ldc1_ch0_cal = 0;
uint32_t ldc1_ch1_cal = 0;
uint32_t ldc1_ch2_cal = 0;
uint32_t ldc1_ch3_cal = 0;
uint32_t ldc2_ch0_cal = 0;
uint32_t ldc2_ch1_cal = 0;

typedef enum {
  HONED = 0,
  ACTIVE = 1, 
} honing_state_t;

// Honing counter
uint32_t ldc_honing_count = 0;
honing_state_t ldc_honing_state = HONED;
uint8_t in_deadband = 1;


void spacemouseSetup()
{
    resetDevice(LDC1612_ADDRESS);
    resetDevice(LDC1614_ADDRESS);
    loadConfig(LDC1612_ADDRESS, default_config, LDC_CONFIG_SIZE);
    LDC_configReg LDC1612_MUX_config = {LDC16xx_MUX_CONFIG, LDC16xx_BITS_DEGLITCH_3_3Mhz | LDC16xx_BITS_AUTOSCAN_EN | LDC16xx_BITS_RR_SEQUENCE_CH0_CH1};
    default_config[16] = LDC1612_MUX_config;
    loadConfig(LDC1614_ADDRESS, default_config, LDC_CONFIG_SIZE);

    readChannel(LDC1614_ADDRESS, 0, &ldc1_ch0_cal);
    readChannel(LDC1614_ADDRESS, 1, &ldc1_ch1_cal);
    readChannel(LDC1614_ADDRESS, 2, &ldc1_ch2_cal);
    readChannel(LDC1614_ADDRESS, 3, &ldc1_ch3_cal);

    readChannel(LDC1612_ADDRESS, 0, &ldc2_ch0_cal);
    readChannel(LDC1612_ADDRESS, 1, &ldc2_ch1_cal);
}


void spacemouseLoop()
{
     // Grab a new set of values
    readChannel(0x2a, 0, &ldc1_ch0);
    readChannel(0x2a, 1, &ldc1_ch1);
    readChannel(0x2a, 2, &ldc1_ch2);
    readChannel(0x2a, 3, &ldc1_ch3);

    readChannel(0x2b, 0, &ldc2_ch0);
    readChannel(0x2b, 1, &ldc2_ch1);

    // Compare to cal'd values
    ldc1_ch0_dif = ((int32_t)ldc1_ch0_cal - (int32_t)ldc1_ch0) / 125;
    ldc1_ch1_dif = ((int32_t)ldc1_ch1_cal - (int32_t)ldc1_ch1) / 125;
    ldc1_ch2_dif = ((int32_t)ldc1_ch2_cal - (int32_t)ldc1_ch2) / 125;
    ldc1_ch3_dif = ((int32_t)ldc1_ch3_cal - (int32_t)ldc1_ch3) / 125;
    ldc2_ch0_dif = ((int32_t)ldc2_ch0_cal - (int32_t)ldc2_ch0) / 125;
    ldc2_ch1_dif = ((int32_t)ldc2_ch1_cal - (int32_t)ldc2_ch1) / 125;

    // Perform honing (if necessary)
    switch (ldc_honing_state){
      case HONED:
        if(
          abs(ldc1_ch0_dif) > LDC_HONE_DEADBAND ||
          abs(ldc1_ch1_dif) > LDC_HONE_DEADBAND ||
          abs(ldc1_ch2_dif) > LDC_HONE_DEADBAND ||
          abs(ldc1_ch3_dif) > LDC_HONE_DEADBAND ||
          abs(ldc2_ch0_dif) > LDC_HONE_DEADBAND ||
          abs(ldc2_ch1_dif) > LDC_HONE_DEADBAND
        ){
          ldc_honing_state = ACTIVE;
        }
        break;
      case ACTIVE:
        if(
          abs(ldc1_ch0_dif) < LDC_HONE_DEADBAND &&
          abs(ldc1_ch1_dif) < LDC_HONE_DEADBAND &&
          abs(ldc1_ch2_dif) < LDC_HONE_DEADBAND &&
          abs(ldc1_ch3_dif) < LDC_HONE_DEADBAND &&
          abs(ldc2_ch0_dif) < LDC_HONE_DEADBAND &&
          abs(ldc2_ch1_dif) < LDC_HONE_DEADBAND
        ){
          ldc_honing_count++;
          in_deadband = 1;
        }else{
          ldc_honing_count = 0;
          in_deadband = 0;
        }
        if(ldc_honing_count > LDC_HONE_PERIOD){
          ldc1_ch0_cal = ldc1_ch0;
          ldc1_ch1_cal = ldc1_ch1;
          ldc1_ch2_cal = ldc1_ch2;
          ldc1_ch3_cal = ldc1_ch3;
          ldc2_ch0_cal = ldc2_ch0;
          ldc2_ch1_cal = ldc2_ch1;
          ldc_honing_count = 0;
          ldc_honing_state = HONED;
        }
      default:
        break;
    }   


    // Get sums and differences
    int32_t cm1 = ldc1_ch0_dif+ldc1_ch1_dif;
    int32_t dm1 = ldc1_ch0_dif-ldc1_ch1_dif;
    int32_t cm2 = ldc1_ch2_dif+ldc1_ch3_dif;
    int32_t dm2 = ldc1_ch2_dif-ldc1_ch3_dif;
    int32_t cm3 = ldc2_ch0_dif+ldc2_ch1_dif;
    int32_t dm3 = ldc2_ch0_dif-ldc2_ch1_dif;

    // Compute tranformation
    int32_t z = cm1 + cm2 + cm3;
    int32_t y = - dm1 + dm3;
    int32_t x = - (dm2 - dm1 / 2 - dm3 / 2);
    int32_t rz = dm1 + dm2 + dm3;
    int32_t rx = cm1 / 2 - cm2 + cm3 / 2;
    int32_t ry =  - cm1 + cm3;

    if(in_deadband){
      z = 0;
      y = 0;
      x = 0;
      rz = 0;
      rx = 0;
      ry = 0;
    }

    x = X_SCALE_FACTOR * x;
    y = Y_SCALE_FACTOR * y;
    z = Z_SCALE_FACTOR * z;
    rx = RX_SCALE_FACTOR * rx;
    ry = RY_SCALE_FACTOR * ry;
    rz = RZ_SCALE_FACTOR * rz;
}

#endif