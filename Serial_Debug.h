#ifndef Serial_Debug_h
#define Serial_Debug_h
#include <arduino.h>

class Serial_Debug
{
  public:
    Serial_Debug(int baud); //pass baud rate for serial monitor
    void begin();
    void debugdata(unsigned long _timer, float _pressure, float _altitude, float _correctedalt, float _ax, float _ay, float _az, float _gx, float _gy, float _gz); //pass all logger variables
    void debugBMP(int _BMPcode, float  _altioffset); //allows passing of an identifier to display different error messages
    void debugIMU();
    void debugSD();

  private:
    int _baud;
    int _BMPcode;
    unsigned long _timer;
    float _pressure, _altitude, _altioffset, _correctedalt, _ax, _ay, _az, _gx, _gy, _gz;
};

#endif