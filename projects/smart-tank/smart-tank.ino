/*
Angelo De Florio
IIoT School Wiki

==== smart-tank.ino ====
Programma principale dell'Arduino UNO R2 WiFi.
Il prototipo Arduino monta un totale di 5 componenti su di una Breadboard:
- LED RGB (KY-016):
  Indicherà lo stato del recipiente in base al livello dell'acqua
  (es. LEVEL:50, LEVEL:25, LEVEL:100);
- Ricevitore IR (KY-005):
  Se riceve un impulso IR mostrerà la temperatura che rileverà il sensore
  della temperatura montato su Arduino. Se la temperatura è < di 30 allora
  il LED sarà BLU altrimenti ROSSO;
- Bottone (KY-004):
  Se premuto mostrerà la temperatura che rileverà il sensore
  della temperatura montato su Arduino;
- Sensore della temperatura (KY-001):
  Rileva la temperatura in base alla posizione della scheda Arduino.
- Buzzer Passivo (KY-006):
  Componente privo di generatore interno che necessita un'onda quadra
  per emettere una frequenza. Impiegato nel segnalare il livello dell'acqua
  nel recipiente.
*/

// Librerie
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
const int IR_RECEIVE_PIN = 7;

/*
setup()
Funzione che esegure una sola volta del codice.
In questo caso inizializza i componenti ed esegue un test
dei LED e del Buzzer per segnalare l'accensione.
*/
void setup() {
  // put your setup code here, to run once:

  // Inizializzazione Seriale e componenti
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

/*
checkTemp()
Funzione che rileva la temperatura dal sensore e
abilita il LED RGB di BLU o ROSSO in base al
valore rilevato
*/
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

/*
loop()
Funzione che ripete continuamente una serie di istruzioni.
Rimane sempre in ascolto per eventuali input provenienti dal
sensore IR, bottone o dalla porta seriale
*/
void loop() {
  // put your main code here, to run repeatedly:

  // Leggi lo stato del bottone
  int buttonState = digitalRead(BUTTON_PIN);
  // Se è stato premuto...
  if (buttonState == LOW) {
    checkTemp(); // controllo la temperatura rilevata
  }

  // Se il ricevitore decodifica...
  if (IrReceiver.decode()) {
    /*
    Restituisce le informazioni sull'impulso ricevuto:
    - protocollo (String);
    - indirizzo  (esadecimale);
    - codice     (esadecimale).
    */
    Serial.print("Protocollo: ");
    Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
    Serial.print("Indirizzo: 0x");
    Serial.println(IrReceiver.decodedIRData.address, HEX);
    Serial.print("Comando: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    Serial.print("Codice completo: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.println();
    // Controllo da sistemare in base al telecomando
    if (IrReceiver.decodedIRData.decodedRawData == 0xC) {
      checkTemp();
    }
    IrReceiver.resume();
  }

  /*
  Ricezione della stringa dal server Node-RED:
  il server Node-RED riceve i dati formattati in JSON dal Gateway
  e in base al campo distance invia una stringa LEVEL che l'Arduino
  interpreterà.
  */
  String input;
  int level;
  if (Serial.available()) {
    // Leggo l'input fino all'andata a capo
    input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("LEVEL:")) {
      // Prendo solo le cifre
      level = input.substring(6).toInt();

      // Livello normale: compreso tra 10 e 90
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
        
      // Trabocco: livello maggiore o uguale a 100
      else if (input >= 100) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(BLUE_LED, HIGH);
        tone(BUZZER_PIN, 1500);
        delay(1000);
        noTone(BUZZER_PIN);
      }
        
      // Insufficiente: livello minore di 10
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
