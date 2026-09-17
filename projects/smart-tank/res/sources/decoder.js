/*
Angelo De Florio
==== decoder.js ====
Script che elabora i dati inviati dal sensore a ultrasuoni (EM310-UDL),
calcola il livello dell'acqua e prepara i dati per due servizi principali:
- InfluxDB --> Database che funge da spina dorsale per la visualizzazione grafica;
- Arduino  --> Circuito Arduino che indica attraverso un Buzzer e un LED RGB
               lo stato del contenitore.

Costanti:
EMPTY_DIST  = Distanza misurata dal sensore
              quando il serbatoio è completamente vuoto
TANK_HEIGHT = L'altezza utile del contenitore
FULL_DIST   = Calcolata come differenza, rappresenta
              la distanza minima dal sensore quando il
              serbatoio è pieno
*/

// Costanti serbatoio
const EMPTY_DIST  = 322; // mm
const TANK_HEIGHT = 160; // mm
const FULL_DIST   = EMPTY_DIST - TANK_HEIGHT;

/*
Ricezione del payload ed estrazione del campo "distance"
convertendolo in un numero
*/
let distance = Number(msg.payload.distance);
// Debug del valore ricevuto
node.warn("Distanza rilevata: " + distance);

/*
Se il campo "distance" ricevuto dal JSON è valido
e se è maggiore di 0 allora...
*/
if (Number.isFinite(distance) && distance > 0) {
    /*
    Calcola l'altezza dell'acqua sottraendo la distanza attuale del
    sensore dalla distanza di serbatoio vuoto.
    Le funzioni Math.max e Math.min servono a limitare il valore dentro
    un intervallo preciso. Serve a evitare che il calcolo dia risultati
    fisicamente impossibili a causa di letture anomale
    del sensore a ultrasuoni.
    */
    let water_lvl  = Math.max(0, Math.min(TANK_HEIGHT, EMPTY_DIST - distance));
    // Converte il livello in una percentuale intera di riempimento rispetto all'altezza totale
    let percentage = Math.round((water_lvl / TANK_HEIGHT) * 100);

    /*
    Aggiorna l'interfaccia grafica del nodo in Node-RED
    mostrando un puntino verde con il testo contenente
    la distanza e la percentuale corrente
    */
    node.status(
        {
            fill: "green",
            shape: "dot",
            text: `${distance} mm - ${percentage}%`
        }
    );

    /*
    InfluxDB:
    Prepara una struttura JSON da inviare al backend
    di InfluxDB, inserendo i dati delle misurazioni
    */
    const metrics = {
        measurement: "water_tank",
        payload: {
            distance: distance,
            water_lvl: water_lvl,
            percentage: percentage
        },
        tags: {
            location: "main_tank",
            sensor: "Milesight EM310-UDL-868M"
        },
        timestamp: new Date()
    };


    /*
    Arduino:
    Crea una stringa formattata con il livello in percentuale
    intera da inviare via seriale ad Arduino
    */
    let arduinoCmd = {};
    arduinoCmd.payload = `LEVEL:${percentage}\n`;

    /*
    Restituisce un array di oggetti che corrispondono
    alle uscite per i due nodi
    */
    return [metrics, arduinoCmd];
}

return null;