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

#include <SoftwareSerial.h>

// types of sensors.
extern const int MHZ14A;
extern const int MHZ19B;

// status codes
extern const int STATUS_NO_RESPONSE;
extern const int STATUS_CHECKSUM_MISMATCH;
extern const int STATUS_INCOMPLETE;
extern const int STATUS_NOT_READY;

class MHZ {
 public:
  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type);
  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t type);
  MHZ(uint8_t pwmpin, uint8_t type);
  MHZ(Stream * serial, uint8_t pwmpin, uint8_t type);
  MHZ(Stream * serial, uint8_t type);

  void setDebug(boolean enable);

  boolean isPreHeating();
  boolean isReady();

  int readCO2UART();
  int readCO2PWM();
  int getLastTemperature();

 private:
  uint8_t _pwmpin, _type, temperature;
  boolean debug = false;

  Stream * _serial;
  byte getCheckSum(byte *packet);
};

#endif
