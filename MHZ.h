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
extern const int MHZ19C;
extern const int MHZ_2K;
extern const int MHZ_5k;
extern const int MHZ_10K;
// status codes
extern const int STATUS_NO_RESPONSE;
extern const int STATUS_CHECKSUM_MISMATCH;
extern const int STATUS_INCOMPLETE;
extern const int STATUS_NOT_READY;

enum Ranges {RANGE_2K = 2000, RANGE_5K = 5000};

class MHZ {
 public:
  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type, Ranges range = RANGE_5K);
  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t type);
  MHZ(uint8_t pwmpin, uint8_t type, Ranges range = RANGE_5K);
  MHZ(Stream * serial, uint8_t pwmpin, uint8_t type, Ranges range = RANGE_5K);

  MHZ(Stream * serial, uint8_t type);

  void setDebug(boolean enable);

  boolean isPreHeating();
  boolean isReady();
  void setAutoCalibrate(boolean b);
  void calibrateZero();
  void setRange(int range);
 // void calibrateSpan(int range); //only for professional use... see implementation and Dataheet.

  int readCO2UART();
  int readCO2PWM();
  int getLastTemperature();

 private:
  uint8_t _pwmpin, _type, temperature;
  Ranges _range = RANGE_5K;
  boolean debug = false;

  Stream * _serial;
  byte getCheckSum(byte *packet);

  unsigned long lastRequest = 0;

  bool SerialConfigured = true;
  bool PwmConfigured = true;
};

#endif
