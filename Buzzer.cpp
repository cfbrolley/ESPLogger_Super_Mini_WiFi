#include <arduino.h>
#include "Buzzer.h"

Buzzer::Buzzer(int pin)
{
  _pin = pin;
}

void Buzzer::begin()
{
  pinMode(_pin, OUTPUT);
}

void Buzzer::startup()
{
  tone(_pin, 4500, 160);
  delay(260);
  tone(_pin, 4500, 100);
  delay(150);
  tone(_pin, 4500, 100);
  delay(150);
  tone(_pin, 5800, 500);
  delay(1100); 
}

void Buzzer::error()
{
  tone(_pin, 5600, 200);
  delay(250);
  tone(_pin, 3800, 500);
}

void Buzzer::success()
{
  tone(_pin, 3700, 220);
  delay(200);
  tone(_pin, 5700, 120);
}

void Buzzer::running()
{
 tone(_pin, 5700, 300);
}

void Buzzer::ended()
{
 tone(_pin, 4700, 300);
 delay(500);
 tone(_pin, 4700, 300);
 delay(3000);
}