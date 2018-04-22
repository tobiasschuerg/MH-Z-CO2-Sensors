
#include <SoftwareSerial.h>

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SoftwareSerial co2Serial(7, 6); // define MH-Z19 RX TX
unsigned long startTime = millis();

void setup() {
  Serial.begin(9600);
  co2Serial.begin(9600);
  pinMode(9, INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("CO2 heating up");

  lcd.setCursor(4, 1);
  lcd.print("seconds left");

  for (int seconds = 180; seconds > 0; seconds--) {
    lcd.setCursor(0, 1);
    lcd.print("   "); // clear timer
    lcd.setCursor(0, 1);
    lcd.print(seconds);
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2 Air Quality");
}

void loop() {
  Serial.println("------------------------------");
  Serial.print("Time from start: ");
  Serial.print((millis() - startTime) / 1000);
  Serial.println(" s");
  int ppm_uart = readCO2UART();
  int ppm_pwm = readCO2PWM();

  // TODO: just clear second line?
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2 Air Quality");

  lcd.setCursor(0, 1);
  lcd.print("PPM: ");

  lcd.setCursor(5, 1);
  lcd.print(ppm_pwm);

  lcd.setCursor(10, 1);
  lcd.print("|");

  lcd.setCursor(12, 1);
  lcd.print(ppm_uart);

  delay(5000);
}

int readCO2UART() {
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9]; // for answer

  Serial.println("Sending CO2 request...");
  co2Serial.write(cmd, 9); //request PPM CO2

  // clear the buffer
  memset(response, 0, 9);
  int i = 0;
  while (co2Serial.available() == 0) {
    //    Serial.print("Waiting for response ");
    //    Serial.print(i);
    //    Serial.println(" s");
    delay(1000);
    i++;
  }
  if (co2Serial.available() > 0) {
    co2Serial.readBytes(response, 9);
  }
  // print out the response in hexa
  for (int i = 0; i < 9; i++) {
    Serial.print(String(response[i], HEX));
    Serial.print("   ");
  }
  Serial.println("");

  // checksum
  byte check = getCheckSum(response);
  if (response[8] != check) {
    Serial.println("Checksum not OK!");
    Serial.print("Received: ");
    Serial.println(response[8]);
    Serial.print("Should be: ");
    Serial.println(check);
  }

  // ppm
  int ppm_uart = 256 * (int)response[2] + response[3];
  Serial.print("PPM UART: ");
  Serial.println(ppm_uart);

  // temp
  byte temp = response[4] - 40;
  Serial.print("Temperature? ");
  Serial.println(temp);

  // status
  byte status = response[5];
  Serial.print("Status? ");
  Serial.println(status);
  if (status == 0x40) {
    Serial.println("Status OK");
  }

  return ppm_uart;
}

byte getCheckSum(char *packet) {
  byte i;
  unsigned char checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}

int readCO2PWM() {
  unsigned long th, tl, ppm_pwm = 0;
  do {
    th = pulseIn(9, HIGH, 1004000) / 1000;
    tl = 1004 - th;
    ppm_pwm = 5000 * (th - 2) / (th + tl - 4);
  } while (th == 0);
  Serial.print("PPM PWM: ");
  Serial.println(ppm_pwm);
  return ppm_pwm;
}
