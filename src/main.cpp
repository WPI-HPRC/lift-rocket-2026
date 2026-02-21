#include <Arduino.h>
#include "Context.h"
#include "State.h"
#include "States.h"
#include "SPI.h"
#include <Wire.h>

#include "boilerplate/Sensors/Impl/GPS/teseo_liv3f_class.h"

#include "boilerplate/Sensors/SensorManager/SensorManager.h"

#include "config.h"

#include "logging.h"

#define GPS_SCL PF_14
#define GPS_SDA PF_15

#define GPS_RESET PB_8
#define GPS_INT PB_9

TwoWire dev_i2c(GPS_SDA, GPS_SCL);

TeseoLIV3F *gps;

void setup() {

    Serial.begin(115200);
    while(!SerialUSB.available()){delay(10);}

    // pinMode(GPS_RESET, OUTPUT);
    // pinMode(GPS_INT, OUTPUT);

    Serial.printf("STARTING\n\n");
    //Wire.begin(static_cast<uint32_t>(GPS_SDA), static_cast<uint32_t>(GPS_SCL));
    gps = new TeseoLIV3F(&dev_i2c, GPS_RESET, GPS_INT);
    GNSS_StatusTypeDef status = gps->init();
    Serial.printf("STATUS = %d\n", status);

}

void loop() {

    Serial.print("TRYING TO UPDATE\n");
    GNSS_StatusTypeDef status = gps->update();
    Serial.print("GOT UPDATE\n");
    if (status != GNSS_OK) {
        Serial.printf("STATUS = %d\n", status);
    }

    Serial.print("WakeUp Status: ");
    gps->getWakeupStatus() == 0 ? Serial.print("0") : Serial.print("1");
    Serial.print("\r\n>");
}

/*
void waitResponse()
{
   //ask the device if the message has arrived
   int result = gps->getMessageDone();
   //if it arrived
   if (result)
   {
      Serial.println("OK");
      //if it was a version message
      if (waitType==1)
      {
         //print the version infos
         getPSTMVERInfo();
         //wait for user interaction
         waitType=0;
         status = waitForRequest;
      }
      //if it was a configuration message
      else if (waitType==2)
      {
         //get the data
         data = gps->getData();
         //if the command was a success
         if (data.result == GNSS_OP_OK)
         {
            //delay in order to prevent i2c issues
            delay(500);
            Serial.print("Saving NMEA msg configuration...\t");
            Serial.print("\r\n>");
            //send the message to save the new parameters and start waiting
            gps->sendCommand((char *)"$PSTMSAVEPAR");
            gps->askMessage((char *)"$PSTMSAVEPAR");
            waitType=3;
            stime = millis();
         }
         //if it failed reset
         else
         {
            waitType=0;
            status = waitForRequest;
         }
      }
      //if it was a save message
      else if (waitType==3)
      {
         //get the data
         data = gps->getData();
         //if the command was a success
         if (data.result == GNSS_OP_OK)
         {
            //reset the sensor
            Serial.print(" Resetting...\t");
            gps->init();
            Serial.print("\r\n>");
         }
         //wait for user interaction
         waitType=0;
         status = waitForRequest;
      }
   }
   //if the timer expired (10s)
   else if (millis() > (stime + 10000))
   {
      //wait for user interaction
      Serial.print("Timed out\r\n>");
      waitType=0;
      status = waitForRequest;
   }
}
   */