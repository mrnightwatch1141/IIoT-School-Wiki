/*
Angelo De Florio
indoor_motion_parser.js
*/
// Costanti
const timeout = 0.1; // minuti
const motion  = Number(JSON.parse(msg.payload).pir);
const light   = Number(JSON.parse(msg.payload).light_level);
node.warn(motion);

/*
Funzione createDownlink(hex):
Funzionamento:
    Permette di creare un payload Downlink
    in JSON per inviare determinati
    comandi come l'accensione e lo spegnimento
*/
function createDownlink(hex) {
    let out = {};
    out.topic = "energysaver/downlink/24e124148c400670";
    out.payload = {
        "confirmed": false,
        "fPort": 85,
        "data": Buffer.from(hex, "hex").toString("base64")
    };
    return out;
}

// Variabile globale Node-RED per memorizzare l'ultimo movimento
let lastMotion = global.get("lastMotion") || Date.now();
// Variabile che contiene il valore esadecimale del payload
let downlinkHex;
/*
Se la persona è presente
o la luce è accesa...
*/
if (motion === 1 || light === 1) {
    // ...allora procedo a verificare lo stato della presa
    // Valore esadecimale di accensione
    downlinkHex = "080100ff"; 
    // Movimento attuale
    global.set("lastMotion", Date.now());
    
    // Invio lo stato del nodo a Node-RED
    node.status({
        fill: "green",
        shape: "ring",
        text: "Rilevato movimeto! Presa: " + (global.get("socketState") === 1 ? "ON" : "OFF")
    });

    // Se la presa è spenta...
    if (global.get("socketState") !== 1) {
        // ...effettuo l'accensione
        node.warn("Accensione...");
        // aggiorno lo stato globale della presa
        global.set("socketState", 1);
        // chiamo la funzione per creare il payload JSON
        return createDownlink(downlinkHex);
    }

    // Esco dalla funzione
    return null;
}
// ...altrimenti
// calcolo il tempo trascorso in minuti
let elapsed = (Date.now() - lastMotion) / 60000;
// Aggiorno lo stato del nodo in Node-RED segnalando l'assenza
node.status({
    fill: "yellow",
    shape: "ring",
    text: "Assente da " + elapsed.toFixed(3) + " minuti. Presa: " + (global.get("socketState") === 1 ? "ON" : "OFF")
});

// Se il tempo è scaduto e la presa è accesa...
if (elapsed >= timeout && global.get("socketState") !== 0) {
    // ...allora effettuo lo spegnimento
    node.warn("Tempo scaduto! Spegnimento presa in corso...");

    // valore esadecimale di accensione
    downlinkHex = "080000ff";
    // aggiorno lo stato globale della presa
    global.set("socketState", 0);
    // chiamo la funzione per creare il payload JSON
    return createDownlink(downlinkHex);
}

// Esco dalla funzione
return null;