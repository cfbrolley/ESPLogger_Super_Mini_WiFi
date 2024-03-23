#ifndef Buzzer_h
#define Buzzer_h
#include <arduino.h>

class Buzzer
{
  public:
    Buzzer(int pin);
    void begin();
    void startup();
    void error();
    void success();
    void running();
    void ended();

  private:
    int _pin;
};

#endif