/*
Angelo De Florio
remote_power.js

Funzione che serve per accendere/spegnere
la Smart Socket da remoto, tramite interfaccia
seriale
*/
// Costanti
const motion  = Number(JSON.parse(msg.payload.motion));
const light   = Number(JSON.parse(msg.payload.light));
/*
Cooldown in millisecondi:
L'implementazione del cooldown è fondamentale per
assicurarsi che la Smart Socket non riceva
comandi di accensione/spegnimento ripetutamente
*/
const COOLDOWN = 5000; // 5 secondi

// Stato precedente del sensore
const previousMotion = context.get("previousMotion") || 0;

// Istante dell'ultimo comando
const lastAction = context.get("lastAction") || 0;
const now = Date.now();

/*
Funzione createDownlink(hex):
Funzionamento:
    Permette di creare un payload Downlink
    in JSON per inviare determinati
    comandi come l'accensione e lo spegnimento

Parametri:
    - hex:   valore esadecimale del payload
    - topic: specifica l'argomento MQTT su cui pubblicare il messaggio
*/
function createDownlink(hex, topic) {
    let out = {};
    out.topic = topic;
    out.payload = {
        "confirmed": false,
        "fPort": 85,
        "data": Buffer.from(hex, "hex").toString("base64")
    };
    return out;
}

// Rilevamento automatico ambiente in base ai lumens
if (light < 150 && light > 100) {
    node.status(
        {
            fill:"brown",
            shape:"ring",
            text:"Stanza: soggiorno " + light + " lumens"
        }
    );
} else if (light < 300 && light > 200) {
    node.status(
        {
            fill:"blue",
            shape:"ring",
            text: "Stanza: bagno " + light + " lumens"
        }
    );
} else {
    node.status(
        {
            fill:"red",
            shape:"ring",
            text: "Stanza: non definita " + light + " lumens"
        }
    )
}

/*
Memorizza lo stato attuale del sensore memorizzando il contesto.
Il contesto in Node-RED serve a memorizzare dati e condividere
informazioni tra i nodi al di fuori del normale flusso dei messaggi
*/
context.set("previousMotion", motion);

// Esegui il comando solo sul passaggio 0 -> 1
const newMotionEvent = (motion === 1 && previousMotion === 0);

if (!newMotionEvent) {
    return null;
}

// Controllo cooldown
if (now - lastAction < COOLDOWN) {
    node.warn("Comando ignorato: cooldown attivo");
    return null;
}

// Memorizza il momento dell'azione
context.set("lastAction", now);

// Variabile che riceve lo stato globale della presa
let socketState = global.get("socketState") || 0;
let downlinkHex;

if (socketState !== 1) {
    downlinkHex = "080100ff";
    node.warn("Accensione remota della presa...");
    global.set("socketState", 1);
} else {
    downlinkHex = "080000ff";
    node.warn("Spegnimento remoto della presa...");
    global.set("socketState", 0);
}

// Creazione del payload downlink
return createDownlink(
    downlinkHex,
    "energysaver/downlink/24e124148c400670"
);