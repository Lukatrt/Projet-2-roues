#include <iostream>
#include <unistd.h>
#include <pigpio.h>

#define LED_PIN 17

int main()
{
  if (gpioInitialise() < 0)
  {
    std::cerr << "Erreur d'initialisation de la bibliothèque pigpio" << std::endl;
    return 1;
  }

  gpioSetMode(LED_PIN, PI_OUTPUT);

  while(1)
  {
    gpioWrite(LED_PIN, 1); // Allumer la LED
    usleep(1000000);
    gpioWrite(LED_PIN, 0); // Éteindre la LED
    usleep(1000000);
  }

  gpioTerminate();

  return 0;
}
