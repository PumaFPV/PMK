#include "Arduino.h"
#include "Wire.h"

#include "LDC16xx.h"


// read data from given LDC register
void readRegister(uint8_t ldcAddr, uint8_t reg, uint16_t *data)
{
  Wire.beginTransmission(ldcAddr);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(ldcAddr, 2);
  uint8_t tmp[2] = {0};
  tmp[0] = Wire.read();
  tmp[1] = Wire.read();

  *data = (tmp[0] << 8) | tmp[1];
}

// write data to given LDC register
void writeRegister(uint8_t ldcAddr, uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {0};
  tmp[0] = data >> 8;
  tmp[1] = data & 0x00FF;
  Wire.beginTransmission(ldcAddr);
  Wire.write(reg);
  Wire.write(tmp[0]);
  Wire.write(tmp[1]);
  Wire.endTransmission();

}



// wrapper to make writing a register nice
void writeLDCConfig(uint8_t ldcAddr, LDC_configReg cfg)
{
  writeRegister(ldcAddr, cfg.reg, cfg.value);
}



// read data for a defined channel (0-3)
int8_t readChannel(uint8_t ldcAddr, uint8_t channel, uint32_t *data)
{
  uint8_t error = 0;

  if(channel > 3)
    return -1;

  uint16_t MSB, LSB;
  readRegister(ldcAddr, LDC16xx_DATA_MSB_CH0 + 2*channel, &MSB);
  readRegister(ldcAddr, LDC16xx_DATA_LSB_CH0 + 2*channel, &LSB);

  error = MSB >> 12;
  *data = (((uint32_t)(MSB & 0x0FFF)) << 12) | LSB >> 4;

  return error;

}



// load full configuration array into LDC
void loadConfig(uint8_t ldcAddr, LDC_configReg cfg[], uint8_t size)
{
  // upload default config to LDC
  for(uint8_t i=0; i<size; i++){
    writeLDCConfig(ldcAddr, cfg[i]);
    delay(100);
  }
}



// clear and set bits in LDC registers, provided user masks
void clearAndSetRegisterBits(uint8_t ldcAddr, uint8_t reg, uint16_t clear_mask, uint16_t set_mask)
{
   // obtain current value of error config register
  uint16_t val;
  readRegister(ldcAddr, reg, &val);
  // clear desired bits
  val &= ~clear_mask;
  // set desired bits
  val |= set_mask;
  // write new value to the device
  writeRegister(ldcAddr, reg, val);
}



// reset the device using I2C command
void resetDevice(uint8_t ldcAddr)
{
  writeRegister(ldcAddr, LDC16xx_RESET_DEV, LDC16xx_BITS_RESET_DEV);
  // 10ms should be enough to start up device
  delay(10);
}



void waitForCoilDataReady(uint8_t ldcAddr, uint8_t coil)
{
  uint16_t tmp = 0;
  while(!(tmp & 1 << (3 - coil))){
    readRegister(ldcAddr, LDC16xx_STATUS, &tmp);
  }
}