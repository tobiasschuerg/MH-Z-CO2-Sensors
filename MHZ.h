/* MHZ library

    By Tobias Schürg
*/
#ifndef MHZ_H
#define MHZ_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


// Define types of sensors.
#define MHZ14A 14
#define MHZ19B 19


#include <SoftwareSerial.h>


class MHZ {
  public:
    MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type);

    int readCO2UART();
    int readCO2PWM();

  private:
    uint8_t _rxpin, _txpin, _pwmpin, _type;
    SoftwareSerial co2Serial;
    byte getCheckSum(byte *packet);

};


#endif

