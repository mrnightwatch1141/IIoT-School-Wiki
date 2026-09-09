/*
Angelo De Florio
IIoT School Wiki

smart-tank.ino
*/
#include <DallasTemperature.h>
#include <OneWire.h>
#include <IRremote.hpp>

// Costanti componenti
// Buzzer
const int BUZZER_PIN = 8;
// RGB
const int RED_LED    = 9;
const int GREEN_LED  = 10;
const int BLUE_LED   = 11;
// Temp sensor
const byte TEMP_PIN = 2;
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
// Button
const int BUTTON_PIN = 13;
// IR
const int IR_RECEIVE_PIN = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  pinMode(BUZZER_PIN , OUTPUT);
  pinMode(RED_LED    , OUTPUT);
  pinMode(GREEN_LED  , OUTPUT);
  pinMode(BLUE_LED   , OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // Startup Test
  digitalWrite(RED_LED   , HIGH);
  digitalWrite(GREEN_LED , LOW);
  digitalWrite(BLUE_LED  , LOW);
  tone(BUZZER_PIN, 500);
  delay(200);
  digitalWrite(RED_LED   , LOW);
  digitalWrite(GREEN_LED , HIGH);
  digitalWrite(BLUE_LED  , LOW);
  tone(BUZZER_PIN, 1000);
  delay(200);
  digitalWrite(RED_LED   , LOW);
  digitalWrite(GREEN_LED , LOW);
  digitalWrite(BLUE_LED  , HIGH);
  tone(BUZZER_PIN, 1500);
  delay(200);
  digitalWrite(RED_LED   , LOW);
  digitalWrite(GREEN_LED , LOW);
  digitalWrite(BLUE_LED  , LOW);
  noTone(BUZZER_PIN);

  delay(500);
}

void checkTemp() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  // Test temperatura
  if (temp < 30) {
    digitalWrite(RED_LED   , LOW);
    digitalWrite(GREEN_LED , LOW);
    digitalWrite(BLUE_LED  , HIGH);

    tone(BUZZER_PIN, 1000);
    delay(250);
    noTone(BUZZER_PIN);
    delay(150);
    tone(BUZZER_PIN, 1000);
    delay(250);
    noTone(BUZZER_PIN);

    digitalWrite(RED_LED   , LOW);
    digitalWrite(GREEN_LED , LOW);
    digitalWrite(BLUE_LED  , LOW);
  } else {
    digitalWrite(RED_LED   , HIGH);
    digitalWrite(GREEN_LED , LOW);
    digitalWrite(BLUE_LED  , LOW);

    tone(BUZZER_PIN, 500);
    delay(250);
    noTone(BUZZER_PIN);
    delay(150);
    tone(BUZZER_PIN, 500);
    delay(250);
    noTone(BUZZER_PIN);

    digitalWrite(RED_LED   , LOW);
    digitalWrite(GREEN_LED , LOW);
    digitalWrite(BLUE_LED  , LOW);
  }
  Serial.println(sensors.getDeviceCount());
  Serial.println(temp);
}

void loop() {
  // put your main code here, to run repeatedly:

  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    checkTemp();
  }

  if (IrReceiver.decode()) {
    Serial.print("Protocollo: ");
    Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
    Serial.print("Indirizzo: 0x");
    Serial.println(IrReceiver.decodedIRData.address, HEX);
    Serial.print("Comando: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    Serial.print("Codice completo: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.println();
    if (IrReceiver.decodedIRData.decodedRawData == 0xC) {
      checkTemp();
    }
    IrReceiver.resume();
  }


  String input;
  int level;
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("LEVEL:")) {
      level = input.substring(6).toInt();

      // Normale
      if (level > 10 && level < 90) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(BLUE_LED, LOW);

        tone(BUZZER_PIN, 150);
        delay(250);
        noTone(BUZZER_PIN);
        delay(150);
        tone(BUZZER_PIN, 250);
        delay(250);
        noTone(BUZZER_PIN);
      }
      // Trabocco
      else if (input >= 100) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(BLUE_LED, HIGH);
        tone(BUZZER_PIN, 1500);
        delay(1000);
        noTone(BUZZER_PIN);
      }
      // Insufficiente
      else {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        tone(BUZZER_PIN, 500);
        delay(1000);
        noTone(BUZZER_PIN);
      }
    }
  }
}
