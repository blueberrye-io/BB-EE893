
//#define TWI_FREQ 5000L

#include "Wire.h"


byte EE893_read_byte(byte control_byte, byte *result)
{
  byte control, dev_addr, i2c_addr, checksum;

  control = control_byte;
  dev_addr = 0x00;

  // Send Control Byte
  i2c_addr = ((control) | (dev_addr << 1)) >> 1;


  //Wert lesen:
  Wire.requestFrom(i2c_addr, 2);
  if (Wire.available()) {
    *result = Wire.read(); // data byte 
    checksum = Wire.read();

    if (checksum != ((*result + control) % 0x100)) {
       return 1;
     }
  }
  else {
    return 2;
  }
  return 0;
}


byte EE893_read_co2(byte *hi, byte *lo, boolean slow)
{
  byte result, result_byte;

  if (slow) {
    result = EE893_read_byte(0xE1, lo);
    if (result != 0) {
      return result;
    }
    result = EE893_read_byte(0xF1, hi);
    if (result != 0) {
      return result;
    }
  } else {
    result = EE893_read_byte(0xC1, lo);
    if (result != 0) {
      return result + 2;
    }
    result = EE893_read_byte(0xD1, hi);
    if (result != 0) {
      return result + 2;
    }
  }
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin(); // wake up I2C bus
  TWBR = 200;  // set the TWBR register (bypass wire library) reduce the speed about 3 times
  TWSR |= bit (TWPS0);
  TWSR |= bit (TWPS1);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte hi, lo;
  Serial.println("\n\nNext Measurement");  

  Serial.print("Returncode ");
  Serial.println(EE893_read_co2(&hi , &lo, false));
  delay(10000);

  //Serial.print(hi, HEX); Serial.print(" "); Serial.println(lo, HEX);
  Serial.print("CO2 [ppm] fast mode "); Serial.println(hi * 256 + lo);


////
  Serial.print("Returncode ");
  Serial.println(EE893_read_co2(&hi , &lo, true));
  delay(20000);

  //Serial.print(hi, HEX); Serial.print(" "); Serial.println(lo, HEX);
  Serial.print("CO2 [ppm] accurate mode "); Serial.println(hi * 256 + lo);
  
}
