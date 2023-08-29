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
#include <limits.h>

// types of sensors.
extern const int MHZ14A;
extern const int MHZ19B;
extern const int MHZ19C;

// status codes
extern const int STATUS_NO_RESPONSE;
extern const int STATUS_CHECKSUM_MISMATCH;
extern const int STATUS_INCOMPLETE;
extern const int STATUS_NOT_READY;

class MHZ {
 public:
  enum MeasuringRange { RANGE_2K = 2000, RANGE_5K = 5000 };

  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type, MeasuringRange range = RANGE_5K);
  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t type);
  MHZ(uint8_t pwmpin, uint8_t type, MeasuringRange range = RANGE_5K);
  MHZ(Stream *serial, uint8_t pwmpin, uint8_t type, MeasuringRange range = RANGE_5K);
  MHZ(Stream *serial, uint8_t type);

  void setDebug(boolean enable, Stream *console = &Serial);

  boolean isPreHeating();
  boolean isReady();
  void setAutoCalibrate(boolean b);
  void calibrateZero();
  void setRange(int range);
  // void calibrateSpan(int range); //only for professional use... see implementation and Dataheet.

  int readCO2UART();
  int readCO2PWM();
  int getLastTemperature();
  int getLastCO2();
  void activateAsyncUARTReading();

 private:
  static const unsigned long MHZ14A_PREHEATING_TIME = 3L * 60L * 1000L;
  static const unsigned long MHZ19B_PREHEATING_TIME = 3L * 60L * 1000L;
  static const unsigned long MHZ19C_PREHEATING_TIME = 1L * 60L * 1000L;
  static const unsigned long MHZ14A_RESPONSE_TIME = (unsigned long)60 * 1000;
  static const unsigned long MHZ19B_RESPONSE_TIME = (unsigned long)120 * 1000;
  static const unsigned long MHZ19C_RESPONSE_TIME = (unsigned long)120 * 1000;
  static const int UNUSED_PIN = -1;

  uint8_t _pwmpin = UNUSED_PIN;
  uint8_t _type, temperature;
  MeasuringRange _range = RANGE_5K;
  boolean debug = false;

  Stream *_serial;
  Stream *_console;
  byte getCheckSum(byte *packet);

  unsigned long lastRequest = 0;

  bool SerialConfigured = true;
};

unsigned long getTimeDiff(unsigned long start, unsigned long stop);

#endif
