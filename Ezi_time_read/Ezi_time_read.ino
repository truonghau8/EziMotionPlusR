#include "EziMotionPlusR.h"

EziMotionPlusR myServo(Serial1, 115200);

void setup()
{
    myServo.initServo();
    Serial.begin(9600);
}

int32_t tmp = 0;

void loop()
{
    static uint32_t last_time = millis();
    Serial.println(millis() - last_time);
    last_time = millis();
    if(myServo.GetActualPos(0, &tmp) == CommIsNormal)
    {
        Serial.print("ok");
    };
}