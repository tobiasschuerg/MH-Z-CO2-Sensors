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

class MHZ {
 public:
  enum SensorType { MHZ14A, MHZ14B, MHZ16, MHZ1911A, MHZ19B, MHZ19C, MHZ19D, MHZ19E };

  enum StatusCode {
    STATUS_NO_RESPONSE = -2,
    STATUS_CHECKSUM_MISMATCH = -3,
    STATUS_INCOMPLETE = -4,
    STATUS_NOT_READY = -5,
    STATUS_PWM_NOT_CONFIGURED = -6,
    STATUS_SERIAL_NOT_CONFIGURED = -7
  };

  enum MeasuringRange { RANGE_2K = 2000, RANGE_5K = 5000, RANGE_10K = 10000, RANGE_50K = 50000 };

  MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, SensorType type, MeasuringRange range = RANGE_5K);
  MHZ(uint8_t rxpin, uint8_t txpin, SensorType type);
  MHZ(uint8_t pwmpin, SensorType type, MeasuringRange range = RANGE_5K);
  MHZ(Stream *serial, uint8_t pwmpin, SensorType type, MeasuringRange range = RANGE_5K);
  MHZ(Stream *serial, SensorType type);

  void setDebug(boolean enable, Stream *console = &Serial);

  boolean isPreHeating();
  boolean isReady();
  void setAutoCalibrate(boolean b);
  void calibrateZero();
  void setRange(int range);
  // void calibrateSpan(int range); //only for professional use... see implementation and Datasheet.

  int32_t readCO2UART();
  int32_t readCO2PWM();
  int getLastTemperature();
  void setTemperatureOffset(uint8_t offset);
  int32_t getLastCO2();
  void activateAsyncUARTReading();
  void setBypassCheck(boolean isBypassPreheatingCheck, boolean isBypassResponseTimeCheck);

 private:
  static const unsigned long MHZ14A_PREHEATING_TIME = 3L * 60L * 1000L;
  static const unsigned long MHZ14B_PREHEATING_TIME = 1L * 30L * 1000L;
  static const unsigned long MHZ16_PREHEATING_TIME = 1L * 10L * 1000L;
  static const unsigned long MHZ1911A_PREHEATING_TIME = 1L * 60L * 1000L;
  static const unsigned long MHZ19B_PREHEATING_TIME = 3L * 60L * 1000L;
  static const unsigned long MHZ19C_PREHEATING_TIME = 1L * 60L * 1000L;
  static const unsigned long MHZ19D_PREHEATING_TIME = 1L * 60L * 1000L;
  static const unsigned long MHZ19E_PREHEATING_TIME = 1L * 60L * 1000L;

  static const unsigned long MHZ14A_RESPONSE_TIME = 60L * 1000L;
  static const unsigned long MHZ14B_RESPONSE_TIME = 0;
  static const unsigned long MHZ16_RESPONSE_TIME = 30L * 1000L;
  static const unsigned long MHZ1911A_RESPONSE_TIME = 120L * 1000L;
  static const unsigned long MHZ19B_RESPONSE_TIME = 120L * 1000L;
  static const unsigned long MHZ19C_RESPONSE_TIME = 120L * 1000L;
  static const unsigned long MHZ19D_RESPONSE_TIME = 120L * 1000L;
  static const unsigned long MHZ19E_RESPONSE_TIME = 120L * 1000L;

  static const int UNUSED_PIN = -1;

  uint8_t _pwmpin = UNUSED_PIN;
  SensorType _type;
  uint8_t temperature;
  uint8_t _temperatureOffset = 44;
  MeasuringRange _range = RANGE_5K;
  boolean debug = false;
  boolean _isBypassPreheatingCheck = false;
  boolean _isBypassResponseTimeCheck = false;

  Stream *_serial;
  Stream *_console;
  byte getCheckSum(byte *packet);

  unsigned long lastRequest = 0;

  bool SerialConfigured = true;
};

unsigned long getTimeDiff(unsigned long start, unsigned long stop);

#endif
