/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BMEP280 Breakout 
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required 
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <U8g2lib.h>

#define tempSensorAddr 0x76
#define displayAddr   0x3C
#define LED PC13

Adafruit_BMP280 bme; // I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  u8g2.begin();
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  pinMode(LED, OUTPUT);
}
  
void loop() {
    float temp, pressure;
    char ctemp[10];
    temp = bme.readTemperature();
    pressure = bme.readPressure()/100;
    if (temp > 26) then {
      digitalWrite(LED,HIGH);
    }else{
      digitalWrite(LED,LOW);
    }
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure());
    Serial.println(" Pa");

    Serial.print("Approx altitude = ");
    Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    
    Serial.println();

    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB14_tr); // choose a suitable font
    u8g2.drawStr(0,30,"T:");  // write something to the internal memory
    u8g2.drawStr(30,30,dtostrf(temp,1,2,ctemp));
    u8g2.drawStr(90,30,"C");
    u8g2.drawStr(0,60,"P:");  // write something to the internal memory
    u8g2.drawStr(30,60,dtostrf(pressure,1,2,ctemp));
    u8g2.drawStr(90,60,"mB");   
    u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);  
    delay(2000);
}
