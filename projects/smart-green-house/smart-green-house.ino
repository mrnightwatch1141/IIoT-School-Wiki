/*
Angelo De Florio
IIoT School Wiki
==== smart-green-house.ino ====
L'Arduino agisce come client che interroga
(servendosi del WiFi) periodicamente Node-RED
per ricevere i dati aggiornati dei sensori
Il prototipo Arduino monta un totale di 3 componenti su di una Breadboard:
- LED RGB (KY-016):
  Indica l'umidità del terreno e se c'è una fuga d'acqua
- Buzzer Passivo (KY-006):
  Componente privo di generatore interno che necessita un'onda quadra
  per emettere una frequenza. Impiegato nel segnalare se il terreno è
  troppo secco o se c'è una fuga d'acqua.
- Rotay Encoders (KY-040):
  Potenziometro che consente di modificare/resettare la soglia di segnalazione.int
*/

// Librerie
#include <WiFiNINA.h>    // Funzioni WiFi generali
#include <HttpClient.h>  // Client HTTP
#include <ArduinoJson.h> // JSON
#include <string.h>

// Costanti
// WiFi
const char* SSID = "SSID_UTENTE";
const char* PASSWORD = "PASSWORD_UTENTE";
// Server Node-RED
char serverAddr[] = "192.168.1.13";
int port = 1880;

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddr, port);

// Componenti
// LED RGB
const int RED_LED   = 13;
const int GREEN_LED = 12;
const int BLUE_LED  = 11;
// BUZZER
const int BUZZER    = 8;
// Trimmer Encoder
const int CLK_PIN   = 2;
const int DT_PIN    = 3;
const int SW_PIN    = 4;

// Variabili globali
volatile int  threshold     = 30;
unsigned long lastClickTime = 0;

/*
setup()
Funzione che esegure una sola volta del codice.
In questo caso inizializza i componenti LED e Buzzer.
*/
void setup() {
  // Inizializzazione Seriale
  Serial.begin(9600);

  // Inizializzazione LED RGB
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Inizializzazione Buzzer
  pinMode(BUZZER, OUTPUT);

  // Inizializzazione poteziometro
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN,  INPUT);
  pinMode(SW_PIN,  INPUT_PULLUP);

  digitalWrite(RED_LED   , LOW);
  digitalWrite(GREEN_LED , LOW);
  digitalWrite(BLUE_LED  , LOW);
  digitalWrite(BUZZER    , LOW);

  // Interrupt per impostare la soglia
  /*
  https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
  Gli interrupts aiutano a risolvere i problemi di timing con Arduino
  Sintassi consigliata:
  attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)
  mode: FALLING = quando il PIN passa da HIGH a LOW
  */
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readTrimmerEncoder, FALLING);

  while (WiFi.begin(SSID, PASSWORD) != WL_CONNECTED) {
    delay(1000);
  }
}

/*
Funzione che legge lo stato del
Trimmer, modifica la soglia o la
resetta
*/
void readTrimmerEncoder() {
  int dtState;
  // Leggo lo stato attuale del Trimmer
  dtState = digitalRead(CLK_PIN);
  // Senso orario
  if (dtState == HIGH) {
    threshold++;
  } else {
    // Senso antiorario
    threshold--;
  }

  // Limite soglia
  if (threshold < 0)   threshold = 0;
  if (threshold > 100) threshold = 100;
}

/*
loop()
Funzione che ripete continuamente una serie di istruzioni.
Rimane sempre in ascolto sull'endpoint arduino-status (sola
lettura). Effettua una richiesta GET per prelevare l'ultimo stato 
dei sensori.
*/
void loop() {
  // put your main code here, to run repeatedly:
  // Dichiarazione variabili
  int http_status, statusCode;
  float remoteValue;
  String leakStatus;

  // Dichiarazione oggetti
  String response;
  StaticJsonDocument<200> doc;
  DeserializationError error; // oggetto che indica l'errore di deserializzazione

  // Reset della soglia
  if (
    digitalRead(SW_PIN) == LOW &&
    millis() - lastClickTime > 300 // Cooldown
  ) {
    threshold = 30;
    // DEBUG
    Serial.println("Soglia riportata a 30");
    lastClickTime = millis();
  }

  // Prelevo lo stato dell'endpoit
  http_status = client.get("/arduino-status");
  if (http_status == 0) {
    // Codice di stato
    statusCode = client.responseStatusCode();
    // Corpo della risposta
    response   = client.responseBody();

    // DEBUG
    Serial.println(response);

    // Se è Successful
    if (statusCode == 200) {
      // Inizializzazione oggetto
      error = deserializeJson(doc, response);

      // Non ci sono errori
      if (!error) {
        /*
        Prelevo il tipo di sensore dal JSON
        e lo inserisco in un puntatore mutabile
        ad una stringa immutabile
        */
        const char* sensorType = doc["sensor"];
        // Prelevo il valore decimale dell'umidità
        remoteValue = doc["value"];

        // DEBUG
        Serial.print("Sensore: ");
        Serial.println(sensorType);
        Serial.print("Valore remoto: ");
        Serial.println(remoteValue);
        Serial.print("Soglia impostata: ");
        Serial.println(threshold);

        if (
          // Se il puntatore non è vuoto/NULL (best practice)
          sensorType != nullptr && 
          // Se è il sensore dell'umidità del terreno
          strcmp(sensorType, "Ursalink EM500-SWL | Terreno") == 0 && 
          // Se il valore è minore della soglia
          remoteValue < threshold
          ) {
          // Segnala l'umidità bassa
          digitalWrite(RED_LED   , HIGH);
          digitalWrite(GREEN_LED , LOW);
          digitalWrite(BLUE_LED  , LOW);
          tone(BUZZER, 1000);
          delay(2000);
          noTone(BUZZER);
        } else if( // Altrimenti controllo...
          sensorType != nullptr &&
          // Se si tratta della fuga d'acqua...
          strcmp(sensorType, "Milesight EM300-SDL | Leakage Status") == 0
        ) {
          /*
          Prelevo lo stato della fuga d'acqua
          effettuando un casting esplicito ed eliminando
          delle ambiguità che potrebbe mandare
          il compilatore in errore.

          In questo modo, indico ad ArduinoJson di estrarre il valore
          come stringa C e la classe String di Arduino accetterà
          un const char* tramite il suo operatore di assegnazione
          */
          leakStatus = doc["value"].as<const char*>();
          // Se il valore è normale...
          if (leakStatus == "normal") {
            // Segnala l'assenza della fuga d'acqua
            digitalWrite(RED_LED   , LOW);
            digitalWrite(GREEN_LED , HIGH);
            digitalWrite(BLUE_LED  , LOW);
            tone(BUZZER, 500);
            delay(2000);
            noTone(BUZZER);
          } else { // Altrimenti
            // Ne segnalo la presenza
            digitalWrite(RED_LED   , HIGH);
            digitalWrite(GREEN_LED , LOW);
            digitalWrite(BLUE_LED  , LOW);
            tone(BUZZER, 1000);
            delay(2000);
            noTone(BUZZER);
          }
        } else { // Se una o NESSUNA delle condizioni non dovessero essere soddisfatte
          // La stato è normale
          digitalWrite(RED_LED   , LOW);
          digitalWrite(GREEN_LED , HIGH);
          digitalWrite(BLUE_LED  , LOW);
          noTone(BUZZER);
        }
      }
    }
  } else {
    client.stop();
    digitalWrite(RED_LED   , LOW);
    digitalWrite(GREEN_LED , LOW);
    digitalWrite(BLUE_LED  , LOW);
    noTone(BUZZER);
  }

  // Interrogo l'endpoint ogni 5 secondi
  delay(5000);
}
