#include <arduino.h>
#include "Serial_Debug.h"

SerialDebug::SerialDebug()
{
_baud = baud
}

void SerialDebug::begin()
{
  Serial.begin(_baud);
}

void SerialDebug::debugdata(unsigned long _timer, float _pressure, float _altitude, float _correctedalt, float _ax, float _ay, float _az, float _gx, float _gy, float _gz) {
    #define COMMA Serial.print(", ");
    Serial.println();
    Serial.print(timer); COMMA;
    Serial.print(pressure); COMMA;
    Serial.print(altitude); COMMA;
    Serial.print(correctedalt); COMMA;
    Serial.print(ax); COMMA;
    Serial.print(ay); COMMA;
    Serial.print(az); COMMA;
    Serial.print(gx); COMMA;
    Serial.print(gy); COMMA;
    Serial.print(gz); COMMA;
}

void SerialDebug::debugBMP() {
      Serial.println("BMP error!");
      Serial.println("Data bus!");
}

void SerialDebug::debugIMU() {
      Serial.println("IMU error!");
}

void SerialDebug::debugSD() {
      Serial.println("SD error!");
}