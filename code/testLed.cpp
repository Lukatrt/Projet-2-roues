#include <wiringPi.h>

#define LED_PIN 17

int main()
{
  wiringPiSetup();
  pinMode(LED_PIN, OUTPUT);

  while(1)
  {
    digitalWrite(LED_PIN, HIGH); // Allumer la LED
    delay(1000);
    digitalWrite(LED_PIN, LOW); // Éteindre la LED
    delay(1000);
  }

  return 0;
}
