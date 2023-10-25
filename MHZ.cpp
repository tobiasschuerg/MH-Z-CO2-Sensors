/* MHZ library

    By Tobias Schürg
*/

#include "MHZ.h"

const int MHZ14A = 114;
const int MHZ14B = 214;
const int MHZ19B = 119;
const int MHZ19C = 219;

const int STATUS_NO_RESPONSE = -2;
const int STATUS_CHECKSUM_MISMATCH = -3;
const int STATUS_INCOMPLETE = -4;
const int STATUS_NOT_READY = -5;
const int STATUS_PWM_NOT_CONFIGURED = -6;
const int STATUS_SERIAL_NOT_CONFIGURED = -7;

uint8_t sPwmPin = 5;
int sRange = MHZ::RANGE_5K;
unsigned long sHighStartsMillis, sLowStartsMillis, sTl, sTh, sLastPwmPpm = 0;
Stream* sConsole;

unsigned long getTimeDiff(unsigned long start, unsigned long stop) {
  if (stop < start) return (ULONG_MAX - start) + stop;
  return stop - start;
}

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

void IRAM_ATTR pulseInInterruptHandler() {
  unsigned long now = millis();
  int state = digitalRead(sPwmPin);

  if (state == true) {  // rising edge
    sTl = getTimeDiff(sLowStartsMillis, now);
    sHighStartsMillis = now;

    if (sTh > 1004 || sTh < 1) {
      sLastPwmPpm = 0;
      return;
    }

    sLastPwmPpm = ((sTh - 2) * sRange) / (sTh + sTl - 4);
    if (sConsole != NULL) {
      sConsole->print("PWM PPM:");
      sConsole->println(sLastPwmPpm);
    }
  } else {  // End of pulse
    sTh = getTimeDiff(sHighStartsMillis, now);
    sLowStartsMillis = now;
  }
}

MHZ::MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type, MeasuringRange range) {
  SoftwareSerial* ss = new SoftwareSerial(rxpin, txpin);
  _pwmpin = pwmpin;
  sPwmPin = pwmpin;
  _type = type;
  _range = range;
  sRange = range;

  ss->begin(9600);
  _serial = ss;
}

MHZ::MHZ(uint8_t rxpin, uint8_t txpin, uint8_t type) {
  SoftwareSerial* ss = new SoftwareSerial(rxpin, txpin);
  _type = type;

  ss->begin(9600);
  _serial = ss;
}

MHZ::MHZ(uint8_t pwmpin, uint8_t type, MeasuringRange range) {
  _pwmpin = pwmpin;
  sPwmPin = pwmpin;
  sRange = range;
  _type = type;
  _range = range;
}

MHZ::MHZ(Stream* serial, uint8_t pwmpin, uint8_t type, MeasuringRange range) {
  _serial = serial;
  _pwmpin = pwmpin;
  sPwmPin = pwmpin;
  sRange = range;
  _type = type;
  _range = range;
}

MHZ::MHZ(Stream* serial, uint8_t type) {
  _serial = serial;
  _type = type;
}

void MHZ::activateAsyncUARTReading() {
  attachInterrupt(digitalPinToInterrupt(sPwmPin), pulseInInterruptHandler, CHANGE);
}

/**
 * Enables or disables the debug mode (more logging).
 */
void MHZ::setDebug(boolean enable, Stream* console) {
  debug = enable;
  _console = console;
  sConsole = console;
  if (debug) {
    _console->println(F("MHZ: debug mode ENABLED"));
  } else {
    _console->println(F("MHZ: debug mode DISABLED"));
  }
}

boolean MHZ::isPreHeating() {
  if (_isBypassPreheatingCheck) {
    return false;
  } else if (_type == MHZ14A) {
    return millis() < (MHZ14A_PREHEATING_TIME);
  } else if (_type == MHZ14B) {
    return millis() < (MHZ14B_PREHEATING_TIME);
  } else if (_type == MHZ19B) {
    return millis() < (MHZ19B_PREHEATING_TIME);
  } else if (_type == MHZ19C) {
    return millis() < (MHZ19C_PREHEATING_TIME);
  } else {
    _console->println(F("MHZ::isPreHeating() => UNKNOWN SENSOR"));
    return false;
  }
}

boolean MHZ::isReady() {
  if (isPreHeating()) {
    return false;
  } else if (_isBypassResponseTimeCheck) {
    return true;
  } else if (_type == MHZ14A) {
    return getTimeDiff(lastRequest, millis()) > MHZ14A_RESPONSE_TIME;
  } else if (_type == MHZ14B) {
    return getTimeDiff(lastRequest, millis()) > MHZ14B_RESPONSE_TIME;
  } else if (_type == MHZ19B) {
    return getTimeDiff(lastRequest, millis()) > MHZ19B_RESPONSE_TIME;
  } else if (_type == MHZ19C) {
    return getTimeDiff(lastRequest, millis()) > MHZ19C_RESPONSE_TIME;
  } else {
    _console->print(F("MHZ::isReady() => UNKNOWN SENSOR \""));
    _console->print(_type);
    _console->println(F("\""));
    return true;
  }
}

int MHZ::readCO2UART() {
  if (_serial == NULL) {
    if (debug) _console->println(F("-- serial is not configured"));
    return STATUS_SERIAL_NOT_CONFIGURED;
  }
  if (!isReady()) return STATUS_NOT_READY;

  // Clearing the uart reading buffer to avoid:
  // - processing the unwanted data the sensor sends during startup
  // - reading an old response already in the reading buffer
  if (debug) _console->print(F("MHZ: - clearing uart reading buffer "));
  while (_serial->available() > 0) {
    if (debug) {
      _console->print(" ");
      _console->print(_serial->peek(), HEX);
    }
    _serial->read();
  }
  if (debug) _console->println();

  if (debug) _console->println(F("-- read CO2 uart ---"));
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9];  // for answer

  if (debug) _console->print(F("  >> Sending CO2 request"));
  _serial->write(cmd, 9);  // request PPM CO2
  lastRequest = millis();

  // clear the buffer
  memset(response, 0, 9);

  int waited = 0;
  while (_serial->available() == 0) {
    if (debug) _console->print(".");
    delay(100);  // wait a short moment to avoid false reading
    if (waited++ > 10) {
      if (debug) _console->println(F("No response after 10 seconds"));
      _serial->flush();
      return STATUS_NO_RESPONSE;
    }
  }
  if (debug) _console->println();

  // The serial stream can get out of sync. The response starts with 0xff, try
  // to resync.
  // TODO: I think this might be wrong any only happens during initialization?
  boolean skip = false;
  while (_serial->available() > 0 && (unsigned char)_serial->peek() != 0xFF) {
    if (!skip) {
      _console->print(F("MHZ: - skipping unexpected readings:"));
      skip = true;
    }
    _console->print(" ");
    _console->print(_serial->peek(), HEX);
    _serial->read();
  }
  if (skip) _console->println();

  if (_serial->available() > 0) {
    int count = _serial->readBytes(response, 9);
    if (count < 9) {
      _serial->flush();
      return STATUS_INCOMPLETE;
    }
  } else {
    _serial->flush();
    return STATUS_INCOMPLETE;
  }

  if (debug) {
    // print out the response in hexa
    _console->print(F("  << "));
    for (int i = 0; i < 9; i++) {
      _console->print(response[i], HEX);
      _console->print(F("  "));
    }
    _console->println(F(""));
  }

  // checksum
  byte check = getCheckSum(response);
  if (response[8] != check) {
    _console->println(F("MHZ: Checksum not OK!"));
    _console->print(F("MHZ: Received: "));
    _console->println(response[8], HEX);
    _console->print(F("MHZ: Should be: "));
    _console->println(check, HEX);
    temperature = STATUS_CHECKSUM_MISMATCH;
    _serial->flush();
    return STATUS_CHECKSUM_MISMATCH;
  }

  int ppm_uart = 256 * (int)response[2] + response[3];

  temperature = response[4] - _temperatureOffset;

  byte status = response[5];
  if (debug) {
    _console->print(F(" # PPM UART: "));
    _console->println(ppm_uart);
    _console->print(F(" # Temperature? "));
    _console->println(temperature);
  }

  // Is always 0 for version 14a  and 19b
  // Version 19a?: status != 0x40
  if (debug && status != 0) {
    _console->print(F(" ! Status maybe not OK ! "));
    _console->println(status, HEX);
  } else if (debug) {
    _console->print(F(" Status  OK: "));
    _console->println(status, HEX);
  }

  _serial->flush();
  return ppm_uart;
}

int MHZ::getLastTemperature() {
  if (_serial == NULL) {
    if (debug) _console->println(F("-- serial is not configured"));
    return STATUS_SERIAL_NOT_CONFIGURED;
  }
  if (isPreHeating()) return STATUS_NOT_READY;
  return temperature;
}

void MHZ::setTemperatureOffset(uint8_t offset) { _temperatureOffset = offset; }

void MHZ::setBypassCheck(boolean isBypassPreheatingCheck, boolean isBypassResponseTimeCheck) {
  _isBypassPreheatingCheck = isBypassPreheatingCheck;
  _isBypassResponseTimeCheck = isBypassResponseTimeCheck;
}

int MHZ::getLastCO2() { return sLastPwmPpm; }

byte MHZ::getCheckSum(byte* packet) {
  if (_serial == NULL) {
    if (debug) _console->println(F("-- serial is not configured"));
    return STATUS_SERIAL_NOT_CONFIGURED;
  }
  if (debug) _console->println(F("  getCheckSum()"));
  byte i;
  unsigned char checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}

int MHZ::readCO2PWM() {
  if (_pwmpin == UNUSED_PIN) {
    if (debug) _console->println(F("-- pwm is not configured "));
    return STATUS_PWM_NOT_CONFIGURED;
  }
  // if (!isReady()) return STATUS_NOT_READY; not needed?
  if (debug) _console->print(F("-- reading CO2 from pwm "));
  unsigned long th, tl, ppm_pwm = 0, start = millis();
  do {
    if (debug) _console->print(".");
    th = pulseIn(_pwmpin, HIGH, 1004000) / 1000;
    tl = 1004 - th;
    ppm_pwm = _range * (th - 2) / (th + tl - 4);
    if (getTimeDiff(start, millis()) > 90L * 1000) {  // Timeout after 90 seconds
      _console->print("Unable to read value. Timeout.");
      break;
    }
  } while (th == 0);
  if (debug) {
    _console->print(F("\n # PPM PWM: "));
    _console->println(ppm_pwm);
  }
  return ppm_pwm;
}

void MHZ::setAutoCalibrate(boolean b)  // only available for MHZ-19B with firmware < 1.6, MHZ-19C and MHZ 14a
{
  uint8_t cmd_enableAutoCal[9] = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6};
  uint8_t cmd_disableAutoCal[9] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86};
  if (b) {
    _serial->write(cmd_enableAutoCal, 9);
  } else {
    _serial->write(cmd_disableAutoCal, 9);
  }
}

void MHZ::setRange(int range)  // only available for MHZ-19B < 1.6 and MH-Z 14a
{
  uint8_t cmd_2K[9] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x07, 0xD0, 0x8F};
  uint8_t cmd_5K[9] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x13, 0x88, 0xCB};
  uint8_t cmd_10K[9] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x27, 0x10, 0x2F};

  switch (range) {
    case 1:
      _serial->write(cmd_2K, 9);
      break;
    case 2:
      _serial->write(cmd_5K, 9);
      break;
    case 3:
      _serial->write(cmd_10K, 9);
  }
}

void MHZ::calibrateZero() {
  uint8_t cmd[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
  _serial->write(cmd, 9);
}

/***** calibrateSpan() function for professional use. requires a constant atmosphere with 2K, 5k or 10k ppm CO2 and
calibrateZero at first.

void MHZ::calibrateSpan(int range)
{
    char cmd_2K[9] = {0xFF, 0x01, 0x88, 0x07, 0xD0, 0x00, 0x00, 0x00, 0xA0};
    char cmd_5K[9] = {oxFF, 0x01, 0x88, 0x13, 0x88, 0x00, 0x00, 0x00, 0xDC};
    char cmd_10K[9]= {0xFF, 0x01, 0x88, 0x27, 0x10, 0x00, 0x00, 0x00, 0x40};

    switch(range)
    {
        case MHZ::RANGE_2K:
            _serial->write(cmd_2K,9);
            break;
        case MHZ::RANGE_5K:
            _serial->write(cmd_5K,9);
            break;
        case 10000:
             _serial->write(cmd_10k,9);
      }

  }
  ****/
