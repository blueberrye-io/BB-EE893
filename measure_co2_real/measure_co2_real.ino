/*
  
 */
#include "Wire.h"



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int  value_co2;

int led_ok = 6;
int led_ng = 7;


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
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  Wire.begin(); // wake up I2C bus
  TWBR = 200;  // set the TWBR register (bypass wire library) reduce the speed about 3 times
  TWSR |= bit (TWPS0);
  TWSR |= bit (TWPS1);

  pinMode(led_ok, OUTPUT);
  pinMode(led_ng, OUTPUT);

  digitalWrite(led_ok, LOW);
  digitalWrite(led_ng, LOW);
  

  delay(2000);
  //Serial.println("CO2 test ready.");
}

void loop() {
  byte hi, lo, result;
  
  serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {
    result = EE893_read_co2(&hi , &lo, false);
    value_co2 = 256*hi + lo;

    if(result == 0) {
      Serial.println(value_co2);
      digitalWrite(led_ok, HIGH);
      digitalWrite(led_ng, LOW);
    }
    else {
      digitalWrite(led_ok, HIGH);
      digitalWrite(led_ng, LOW); 
    }
    
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  //Serial.println("test");
  //delay(500);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;

      // the function returns without appending the 
      // newline to the inputstring
      return;
    }
    // add it to the inputString:
    inputString += inChar;
    
  }
}
