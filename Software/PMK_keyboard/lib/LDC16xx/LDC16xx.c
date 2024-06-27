#include "LDC16xx.h"



// read data from given LDC register
void readRegister(uint8_t LDC_addr, uint8_t reg, uint16_t * data){

  uint8_t tmp[2] = {0};
  HAL_I2C_Mem_Read(&hi2c1, (LDC_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, tmp, 2, 1000);
  *data = (tmp[0] << 8) | tmp[1];

}

// write data to given LDC register
void writeRegister(uint8_t LDC_addr, uint8_t reg, uint16_t data){

  uint8_t tmp[2] = {0};
  tmp[0] = data >> 8;
  tmp[1] = data & 0x00FF;
  HAL_I2C_Mem_Write(&hi2c1, (LDC_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, tmp, 2, 1000);

}

// wrapper to make writing a register nice
void writeLDCConfig(uint8_t LDC_addr, LDC_configReg cfg){
  writeRegister(LDC_addr, cfg.reg, cfg.value);
}

// read data for a defined channel (0-3)
int8_t readChannel(uint8_t LDC_addr, uint8_t channel, uint32_t *data){

  uint8_t error = 0;

  if(channel > 3)
    return -1;

  uint16_t MSB, LSB;
  readRegister(LDC_addr, LDC16xx_DATA_MSB_CH0 + 2*channel, &MSB);
  readRegister(LDC_addr, LDC16xx_DATA_LSB_CH0 + 2*channel, &LSB);

  error = MSB >> 12;
  *data = (((uint32_t)(MSB & 0x0FFF)) << 12) | LSB >> 4;

  return error;

}

// load full configuration array into LDC
void loadConfig(uint8_t LDC_addr, LDC_configReg cfg[], uint8_t size){
  // upload default config to LDC
  for(uint8_t i=0; i<size; i++){
    writeLDCConfig(LDC_addr, cfg[i]);
    HAL_Delay(100);
  }
}

// clear and set bits in LDC registers, provided user masks
void clearAndSetRegisterBits(uint8_t LDC_addr, uint8_t reg, uint16_t clear_mask, uint16_t set_mask){
   // obtain current value of error config register
  uint16_t val;
  readRegister(LDC_addr, reg, &val);
  // clear desired bits
  val &= ~clear_mask;
  // set desired bits
  val |= set_mask;
  // write new value to the device
  writeRegister(LDC_addr, reg, val);
}

// reset the device using I2C command
void resetDevice(uint8_t LDC_addr){
  writeRegister(LDC_addr, LDC16xx_RESET_DEV, LDC16xx_BITS_RESET_DEV);
  // 10ms should be enough to start up device
  HAL_Delay(10);
}

void waitForCoilDataReady(uint8_t LDC_addr, uint8_t coil){
  uint16_t tmp = 0;
  while(!(tmp & 1 << (3 - coil))){
    readRegister(LDC_addr, LDC16xx_STATUS, &tmp);
  }
}