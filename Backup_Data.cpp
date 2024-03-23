#include <arduino.h>
#include <EEPROM.h>
#include "Backup_Data.h"

Backup_Data::Backup_Data(int ALTITUDE_ADDRESS, int MAX_ACCEL_ADDRESS, int TTA_ADDRESS) //When creating instance, pass the EEPROM memory addresses to the library and it'll handle the rest
{
  _ALTITUDE_ADDRESS = ALTITUDE_ADDRESS;
  _MAX_ACCEL_ADDRESS = MAX_ACCEL_ADDRESS;
  _TTA_ADDRESS = TTA_ADDRESS;
}

void Backup_Data::begin(){}

void Backup_Data::save(float _alti, float _accel, float _TTA) //actually not sure if this is correct using private variables, but it compiles, sooo.....
  {
   EEPROM.put(_ALTITUDE_ADDRESS, _alti);
   EEPROM.put(_MAX_ACCEL_ADDRESS, _accel);
   EEPROM.put(_TTA_ADDRESS, _TTA);
  }

void Backup_Data::read()
  {
   EEPROM.get(_ALTITUDE_ADDRESS, _alti);
   EEPROM.get(_MAX_ACCEL_ADDRESS, _accel);
   EEPROM.get(_TTA_ADDRESS, _TTA);
   Serial.println();
   Serial.println();
   Serial.println("Saved flight summary:");
   Serial.println();
   Serial.print("Max altittude recorded: ");
   Serial.print(_alti);
   Serial.println(" meters");
   Serial.print("Max acceleration recorded: ");
   Serial.print(_accel);
   Serial.println(" meters per second");
   Serial.print("Time to apogee: ");
   Serial.print(_TTA / 1000);
   Serial.println(" seconds");
   Serial.println();
  }