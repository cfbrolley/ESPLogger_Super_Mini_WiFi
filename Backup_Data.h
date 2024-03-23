#ifndef Backup_Data_h
#define Backup_Data_h
#include <arduino.h>
#include <EEPROM.h>

class Backup_Data
{
  public:
    Backup_Data(int ALTITUDE_ADDRESS, int MAX_ACCEL_ADDRESS, int TTA_ADDRESS);
    void begin();
    void save(float _alti, float _accel, float _TTA);
    void read();

  private:
    float _alti;
    float _accel;
    float _TTA;
    int _ALTITUDE_ADDRESS;
    int _MAX_ACCEL_ADDRESS;
    int _TTA_ADDRESS;
};

#endif