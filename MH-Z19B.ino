

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "MHZ.h"

#define CO2_IN D2

#define MH_Z19_RX D7
#define MH_Z19_TX D6


MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

void setup() {
  Serial.begin(9600);
  pinMode(CO2_IN, INPUT);
  delay(100);
  Serial.println("MHZ 19B");

  co2.setDebug(false);
}


void loop() {
  // Serial.print("\n----- Time from start: ");
  // Serial.print(millis() / 1000);
  // Serial.println(" s");
  int ppm_uart = co2.readCO2UART();
  // int ppm_pwm = co2.readCO2PWM();
  int temperature = co2.getLastTemperature();
  Serial.print("PPMuart: ");
  Serial.print(ppm_uart);
  // Serial.print(", PPMpwm: ");
  // Serial.print(ppm_pwm);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // Serial.println("\n------------------------------");
  // delay(3000);
}
