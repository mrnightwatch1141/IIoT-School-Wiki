/*
Angelo De Florio
IIoT School Wiki

==== auto-energy-saver.in ====
Programma principale per l'Arduino UNO R3 (NO WiFi):
Il prototipo Arduino monta 2 componenti su di una Breadboard:
- Sensore di tracciamento ad infrarossi (KY-033):
  Traccerà il movimento ravvicinato della persona e ne segnalerà la presenza
  con l'accensione del LED integrato;
- Fotoresistor (KY-018):
  Rileva il livello di luminosità nella stanza.
*/

// Costanti componenti
// IR Tracker
const byte TRACKING_PIN               = 8;
// Fotoresistor
const byte PHOTORESISTOR_PIN          = A0;

/*
Costante delay:
per evitare un sovraccarico generale
a causa dell'immediatezza della misurazione
*/
const unsigned long INTERVALLO_INVIO  = 250; // ms

/*
Variabili globale per l'ultimo invio delle misurazioni:
memorizza i millisecondi trascorsi tra un invio e l'altro.
*/
unsigned long ultimoInvio = 0;

/*
setup()
Funzione che esegure una sola volta del codice.
Inizializzazione dei componenti.
*/
void setup() {
  // Inizializzazione Seriale e componenti
  Serial.begin(9600);
  /*
  Inizializzazione IR Tracker con INPUT_PULLUP:
  INPUT_PULLUP --> internamente, connette un resistore tra il PIN
                   e i 5V mantenendo il PIN a 5V finchè non viene
                   rilevato un valore che lo colleghi a MASSA.
                   Senza di esso, il PIN "fluttuerebbe" agendo come
                   un'antenna che capta tutti i segnali nelle vicinanze
                   restituendo misurazioni errate.
  */
  pinMode(TRACKING_PIN, INPUT_PULLUP);
  // Inizializzazione LED integrato nell'IR Tracker
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Inizializzazione Fotoresistor
  pinMode(PHOTORESISTOR_PIN, INPUT);
}

/*
loop()
Funzione che ripete continuamente una serie di istruzioni.
Rimane sempre in ascolto per eventuali input provenienti
dall'IR Tracker o dal Fotoresistor.
*/
void loop() {
  // put your main code here, to run repeatedly:

  // Dichiarazione variabili
  int valoreLuce, trackingState, ostacolo;
  // Millisecondi attuali
  unsigned long now = millis();

  // Se è trascorso abbastanza tempo
  if (now - ultimoInvio >= INTERVALLO_INVIO) {
    // Registra i millisecondi attuali
    ultimoInvio = now;

    // Leggi il valore della luce
    valoreLuce = analogRead(PHOTORESISTOR_PIN);

    // Leggi lo stato dell'IR Tracker
    trackingState = digitalRead(TRACKING_PIN);
    // Se è LOW...
    if (trackingState == LOW) {
      // Ha rilevato l'ostacolo, quindi l'oggetto/persona vicina
      ostacolo = 1;
    } else {
      // Altrimenti la strada del sensore è libera
      ostacolo = 0;
    }

    // Invio di un JSON via seriale con i valori misurati
    Serial.print("{\"motion\":");
    Serial.print(ostacolo);
    Serial.print(",\"light\":");
    Serial.print(valoreLuce);
    Serial.println("}");
  }
}