/* MHZ library

    By Tobias Schürg
*/

#include "MHZ.h"

MHZ::MHZ(uint8_t rxpin, uint8_t txpin, uint8_t pwmpin, uint8_t type)
    : co2Serial(rxpin, txpin) {
  _rxpin = rxpin;
  _txpin = txpin;
  _pwmpin = pwmpin;
  _type = type;

  co2Serial.begin(9600);
}

void MHZ::setDebug(boolean enable) {
  debug = enable;
  if (debug) {
    Serial.println("MHZ: debug mode ENABLED");
  } else {
    Serial.println("MHZ: debug mode DISABLED");
  }
}

int retryCount = 0;

int MHZ::readCO2UART() {
  if (debug) Serial.println("-- read CO2 uart ---");
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9];  // for answer

  if (debug) Serial.print("  >> Sending CO2 request");
  co2Serial.write(cmd, 9);  // request PPM CO2

  // clear the buffer
  memset(response, 0, 9);

  int waited = 0;
  while (co2Serial.available() == 0) {
    if (debug) Serial.print(".");
    delay(100);  // wait a short moment to avoid false reading
    if (waited++ > 10) {
      if (debug) Serial.println("No response after 10 seconds");
      co2Serial.flush();
      return STATUS_NO_RESPONSE;
    }
  }
  if (debug) Serial.println();

  // The serial stream can get out of sync. The response starts with 0xff, try
  // to resync.
  // TODO: I think this might be wrong any only happens during initialization?
  boolean skip = false;
  while (co2Serial.available() > 0 && (unsigned char)co2Serial.peek() != 0xFF) {
    if (!skip) {
      Serial.print("MHZ: - skipping unexpected readings:");
      skip = true;
    }
    Serial.print(" ");
    Serial.print(co2Serial.peek(), HEX);
    co2Serial.read();
  }
  if (skip) Serial.println();

  if (co2Serial.available() > 0) {
    int count = co2Serial.readBytes(response, 9);
    if (count < 9) {
      co2Serial.flush();
      return STATUS_INCOMPLETE;
    }
  } else {
    co2Serial.flush();
    return STATUS_INCOMPLETE;
  }

  if (debug) {
    // print out the response in hexa
    Serial.print("  << ");
    for (int i = 0; i < 9; i++) {
      Serial.print(response[i], HEX);
      Serial.print("  ");
    }
    Serial.println("");
  }

  // checksum
  byte check = getCheckSum(response);
  if (response[8] != check) {
    Serial.println("MHZ: Checksum not OK!");
    Serial.print("MHZ: Received: ");
    Serial.println(response[8], HEX);
    Serial.print("MHZ: Should be: ");
    Serial.println(check, HEX);
    temperature = STATUS_CHECKSUM_MISMATCH;
    co2Serial.flush();
    return STATUS_CHECKSUM_MISMATCH;
  }

  int ppm_uart = 256 * (int)response[2] + response[3];

  temperature = response[4] - 44;  // - 40;

  byte status = response[5];
  if (debug) {
    Serial.print(" # PPM UART: ");
    Serial.println(ppm_uart);
    Serial.print(" # Temperature? ");
    Serial.println(temperature);
  }

  // Is always 0 for version 14a  and 19b
  // Version 19a?: status != 0x40
  if (debug || status != 0) {
    Serial.print(" ! Status maybe not OK ! ");
    Serial.println(status, HEX);
  } else if (debug) {
    Serial.print(" Status  OK: ");
    Serial.println(status, HEX);
  }

  co2Serial.flush();
  return ppm_uart;
}

uint8_t MHZ::getLastTemperature() { return temperature; }

byte MHZ::getCheckSum(byte *packet) {
  if (debug) Serial.println("  getCheckSum()");
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
  if (debug) Serial.print("-- reading CO2 from pwm ");
  unsigned long th, tl, ppm_pwm = 0;
  do {
    if (debug) Serial.print(".");
    th = pulseIn(_pwmpin, HIGH, 1004000) / 1000;
    tl = 1004 - th;
    ppm_pwm = 5000 * (th - 2) / (th + tl - 4);
  } while (th == 0);
  if (debug) {
    Serial.print("\n # PPM PWM: ");
    Serial.println(ppm_pwm);
  }
  return ppm_pwm;
}
