

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "MHZ.h"

#define CO2_IN D2

#define MH_Z19_RX D7
#define MH_Z19_TX D6


MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

unsigned long startTime = millis();


void setup() {
  Serial.begin(9600);
  pinMode(CO2_IN, INPUT);
  delay(100);
  Serial.println("MHZ 19B");
}


void loop() {
  Serial.print("Time from start: ");
  Serial.print((millis() - startTime) / 1000);
  Serial.println(" s");
  int ppm_uart = co2.readCO2UART();
  int ppm_pwm = co2.readCO2PWM();

  Serial.println("\n------------------------------");
  delay(5000);
}
