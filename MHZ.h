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


#define STATUS_NO_RESPONSE -2
#define STATUS_CHECKSUM_MISMATCH -3
#define STATUS_INCOMPLETE -4


#include <SoftwareSerial.h>


class MHZ {
  public:
    MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type);

    void setDebug(boolean enable);

    int readCO2UART();
    int readCO2PWM();
    uint8_t getLastTemperature();

  private:
    uint8_t _rxpin, _txpin, _pwmpin, _type, temperature;
    boolean debug = false;

    SoftwareSerial co2Serial;
    byte getCheckSum(byte *packet);
};


#endif

