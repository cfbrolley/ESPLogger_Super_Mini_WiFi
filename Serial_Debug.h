#ifndef Serial_Debug_h
#define Serial_Debug_h
#include <arduino.h>

class SerialDebug
{
  public:
    SerialDebug(int baud);
    void begin(int _baud);
    void debugdata(unsigned long _timer, float _pressure, float _altitude, float _correctedalt, float _ax, float _ay, float _az, float _gx, float _gy, float _gz);
    void debugsensor();

  private:
    int _baud;
    unsigned long _timer;
    float _pressure, _altitude, _correctedalt, _ax, _ay, _az, _gx, _gy, _gz;
};

#endif