
//Radio receiver header code 
// -*- mode: C++ -*-
#include <RH_ASK.h>
#include<Keyboard.h>
#include <SPI.h> // Not actualy used but needed to compile
RH_ASK driver;

//initialize variables used for coordinating key press times
long goTillTime = 0;   
long lastUpdateTime=0;
float flt_speed=0;

void setup()
{
    //start serial communication with radio receiver
    Serial.begin(9600);
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    unsigned long currentTime = millis();
    //create a buffer of bytes to store received radio message
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    //float variable for storing speed with decimal
    flt_speed=0; //set speed to zero each cycle, to be updated with reception of speed via radio
    
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        // Message with a good checksum received, dump it.
        driver.printBuffer("Got:", buf, buflen);
        String str_try =String((char*)buf);
        
        //2 and 6 are the indices in str_try of the received char set that give the float
        Serial.println(str_try.substring(2,6));    
        flt_speed = str_try.substring(2,6).toFloat(); //if no valid float returned, function returns 0; won't throw egregious error
        lastUpdateTime=millis(); //keeps track of last recorded radio receipt
     }

    //manage length of keyboard presses with speed value obtained. Default threshold of 1.1.
    if ( flt_speed > 1.1)
    {
      Keyboard.press(KEY_UP_ARROW); //for TrackMania
      //Keyboard.press('w');//for Redout
      goTillTime=currentTime+500; //extends length of key press for 0.5 seconds past most recent received transmission over threshold
    }
    if(goTillTime<currentTime)
    {
      //release held keys if no signal over threshold in last 0.5 seconds.
      Keyboard.releaseAll();
    }    
}
