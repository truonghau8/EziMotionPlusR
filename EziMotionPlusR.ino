#include <Arduino.h>
#include "EziMotionPlusR.h"
#include "SoftwareSerial.h"

SoftwareSerial mySerial(2, 3);

EziMotionPlusR myServo(mySerial, 9600);

void setup()
{
    Serial.begin(9600);
    myServo.initServo();
    // pinMode(13, OUTPUT);
    // myServo.ServoEnable(0, 1);
    // if(myServo.MoveSingleAxisAbsPos(0, -20000, 100000) == CommIsNormal)
    // {
    //     digitalWrite(13, 1);
    //     delay(500);
    //     //myServo.ServoEnable(0,1);
    // }

    // delay(500);
    // myServo.MoveSingleAxisIncPos(0, 10000, 100000);
    // delay(500);
    // myServo.MoveSingleAxisIncPos(0, 10000, 100000);
    // int32_t test = 0;
    // if (myServo.ClearPosition(0) == CommIsNormal)
    // {
    //     Serial.println(test);
    // };

    if (myServo.ServoEnable(0, 1) == CommIsNormal)
    {
        int32_t tmp;
        //myServo.ClearPosition(0);
        delay(500);
        if (myServo.MoveSingleAxisIncPos(0, 10000, 100000) == CommIsNormal)
        {
            myServo.GetCommandPos(0, &tmp);
            Serial.println(tmp);
            delay(5000);

            if (myServo.EmergencyStop(0) == CommIsNormal)
            {
                myServo.ServoEnable(0, 0);
            };
        }
    }
}
void loop()
{
}
