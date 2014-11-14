#include "ThreeDCompass.h"

bool ThreeDCompass::haveHMC5883L = false;

bool ThreeDCompass::detectHMC5883L ()
{
  // read identification registers
  Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
  Wire.write(10); //select Identification register A
  Wire.endTransmission();
  Wire.requestFrom(HMC5883L_ADDR, 3);
  if(3 == Wire.available()) {
    char a = Wire.read();
    char b = Wire.read();
    char c = Wire.read();
    if(a == 'H' && b == '4' && c == '3')
      return true;
  }

  return false;
}


void ThreeDCompass::init()
{
  //Initialize Serial and I2C communications
  #if __COMPASS_H__DEBUG
    Serial.println("Init Compass - GY271");
  #endif
  Wire.begin();
  // lower I2C clock http://www.gammon.com.au/forum/?id=10896
  TWBR = 78;  // 25 kHz 
  TWSR |= _BV (TWPS0);  // change prescaler  
}

void ThreeDCompass::update(int* x, int* y, int *z)
{
  bool detect = detectHMC5883L();

  if(!haveHMC5883L) 
  {
    if(detect) 
    {
      haveHMC5883L = true;
      #if __COMPASS_H__DEBUG
        Serial.println("We have HMC5883L, moving on");
      #endif
      // Put the HMC5883 IC into the correct operating mode
      Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
      Wire.write(0x02); //select mode register
      Wire.write(0x00); //continuous measurement mode
      Wire.endTransmission();
    }
    else
    {  
#if __COMPASS_H__DEBUG
      Serial.println("No HMC5883L detected!");
#endif
      return;
    }
  }
  else
  {
    if(!detect) {
      haveHMC5883L = false;
      Serial.println("Lost connection to HMC5883L!");
      return;
    }
  }

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(HMC5883L_ADDR, 6);
  if(6<=Wire.available()){
    *x = Wire.read()<<8; //X msb
    *x |= Wire.read(); //X lsb
    *z = Wire.read()<<8; //Z msb
    *z |= Wire.read(); //Z lsb
    *y = Wire.read()<<8; //Y msb
    *y |= Wire.read(); //Y lsb
  }
  
  //Print out values of each axis
#if __COMPASS_H__DEBUG
  Serial.print("x: ");
  Serial.print(*x);
  Serial.print("  y: ");
  Serial.print(*y);
  Serial.print("  z: ");
  Serial.println(*z);
#endif
  
}
  

ThreeDCompass compass;
