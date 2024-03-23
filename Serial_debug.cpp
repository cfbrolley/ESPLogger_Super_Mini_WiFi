#include <arduino.h>
#include "Serial_Debug.h"

Serial_Debug::Serial_Debug(int baud) {
_baud = baud;
}

void Serial_Debug::begin() {
  Serial.begin(_baud);
}

void Serial_Debug::debugdata(unsigned long _timer, float _pressure, float _altitude, float _correctedalt, float _ax, float _ay, float _az, float _gx, float _gy, float _gz) {
    #define COMMA Serial.print(", ");
    Serial.println();
    Serial.print(_timer); COMMA;
    Serial.print(_pressure); COMMA;
    Serial.print(_altitude); COMMA;
    Serial.print(_correctedalt); COMMA;
    Serial.print(_ax); COMMA;
    Serial.print(_ay); COMMA;
    Serial.print(_az); COMMA;
    Serial.print(_gx); COMMA;
    Serial.print(_gy); COMMA;
    Serial.print(_gz); COMMA;
}

void Serial_Debug::debugBMP(int _BMPcode, float _altioffset) {
  switch (_BMPcode) {
  case 1:
      Serial.println("BMP error!");
      Serial.println("Data bus!");
      break;
  case 2:
      Serial.println("BMP error!");
      Serial.println("Chip version!");
      break;
  case 3:
      Serial.println("BMP OK!");
      Serial.println("starting altitude: ");
      Serial.print(_altioffset);
      Serial.print("m");
      Serial.println();
      break;
  default:
      Serial.println("BMP error!");
      Serial.println("unknown!");
      break;
  }    
}

void Serial_Debug::debugIMU() {
      Serial.println("IMU error!");
}

void Serial_Debug::debugSD() {
      Serial.println("SD error!");
}