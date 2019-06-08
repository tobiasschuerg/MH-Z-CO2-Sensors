#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <MHZ.h>

// pin for pwm reading
#define CO2_IN 10

// pin for uart reading
#define MH_Z19_RX D4  // D7
#define MH_Z19_TX D0  // D6

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

void setup() {
  Serial.begin(9600);
  pinMode(CO2_IN, INPUT);
  delay(100);
  Serial.println("MHZ 19B");

  // enable debug to get addition information
  // co2.setDebug(true);

  if (co2.isPreHeating()) {
    Serial.print("Preheating");
    while (co2.isPreHeating()) {
      Serial.print(".");
      delay(5000);
    }
    Serial.println();
  }
}

void loop() {
  Serial.print("\n----- Time from start: ");
  Serial.print(millis() / 1000);
  Serial.println(" s");

  int ppm_uart = co2.readCO2UART();
  Serial.print("PPMuart: ");

  if (ppm_uart > 0) {
    Serial.print(ppm_uart);
  } else {
    Serial.print("n/a");
  }

  int ppm_pwm = co2.readCO2PWM();
  Serial.print(", PPMpwm: ");
  Serial.print(ppm_pwm);

  int temperature = co2.getLastTemperature();
  Serial.print(", Temperature: ");

  if (temperature > 0) {
    Serial.println(temperature);
  } else {
    Serial.println("n/a");
  }

  Serial.println("\n------------------------------");
  delay(5000);
}
