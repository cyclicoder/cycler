
//Accelerometer header code
#include <MPU9250_asukiaaa.h>
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 26
#define SCL_PIN 25
#endif

long nowTime;
long lastTime;
long intervalTime;
MPU9250 mySensor;
float accelRoot1=0;
float accelRoot2=0;
float accelRoot3=0;
float accelRoot4=0;
float averageAccelRoot=0;
float initSensorAccX=0;
float initSensorAccY=0;
float initSensorAccZ=0;

//Radio transmitter header code
// -*- mode: C++ -*-
// Implements a simplex (one-way) transmitter with an TX-C1 module
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
RH_ASK driver; // ESP8266: do not use pin 11

//Arduino setup function
void setup() {
  //keep track of time
  nowTime=0;
  lastTime=0;
  intervalTime=0;

  //start serial wire connection for accelerometer
  while(!Serial);
  Serial.begin(9600);
  Serial.println("started");
  #ifdef _ESP32_HAL_I2C_H_
    // for esp32
    Wire.begin(SDA_PIN, SCL_PIN); //sda, scl
  #else
    Wire.begin();
  #endif
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginMag();

 if (!driver.init())
   Serial.println("init failed");

  //initial sensor acceleration values due to orientation.
  //this will keep running until it is about to get sensor data
   while(mySensor.accelSqrt()==0)
  {
    mySensor.accelUpdate();
    //get initial accelerometer values to use for partial gravity correct
    initSensorAccX=mySensor.accelX();
    initSensorAccY=mySensor.accelY();
    initSensorAccZ=mySensor.accelZ();
    delay(100);
  }
  Serial.println(String(mySensor.accelX())+String(initSensorAccY)+String(4));


   
}

void loop() {

  //update most recent stored speed vectors
  //calculate vector norm, with calibration for effect of gravity based on initial (assumed) upright orientation
  accelRoot1=sqrt((mySensor.accelX()-initSensorAccX)*(mySensor.accelX()-initSensorAccX)+(mySensor.accelY()-initSensorAccY)*(mySensor.accelY()-initSensorAccY)+(mySensor.accelZ()-initSensorAccZ)*(mySensor.accelZ()-initSensorAccZ));
  accelRoot2=accelRoot1;
  accelRoot3=accelRoot2;
  accelRoot4=accelRoot3;
  //use running average of last four readings as value to send via radio
  averageAccelRoot=(accelRoot1+accelRoot2+accelRoot3+accelRoot4)/4;
  Serial.println("averageAccelRoot: " + String(averageAccelRoot));

  lastTime=nowTime;
  nowTime = millis();
  intervalTime = nowTime-lastTime;
  
  mySensor.accelUpdate();
  Serial.println("print accel values");
  Serial.println("accelX: " + String(mySensor.accelX()));
  Serial.println("accelY: " + String(mySensor.accelY()));
  Serial.println("accelZ: " + String(mySensor.accelZ()));
  //Serial.println("accelSqrt: " + String(mySensor.accelSqrt()));
  
  
  //RADIO CODE
  //onyl send message if acceleration vector above cutoff, to save on radio transmission power.
  if(averageAccelRoot>0.4)
    {
    char result[8];
    dtostrf(mySensor.accelSqrt(), 6, 2, result);
    driver.send((uint8_t *)result, strlen(result));
    driver.waitPacketSent();
    delay(100);
    }
}
