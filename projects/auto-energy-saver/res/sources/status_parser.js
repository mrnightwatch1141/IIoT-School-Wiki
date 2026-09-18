/*
Angelo De Florio
status_parser.js

Interpreta lo stato della presa ricevendo
un payload periodico dal Network Server
tramite broker MQTT
*/
let payload;

// Provo ad effettuare il parsing del payload
try {
    payload = JSON.parse(msg.payload);
} catch (err) {
    node.error("Payload JSON non valido", msg);
return null;

}

/*
Aggiorno lo stato della presa con
una variabile globale del flusso
Node-RED
*/
let state = Number(payload.socket_status);
node.warn("Stato presa: " + state);
if (state === 1 || state === 0) {
    global.set("socketState", state);
}
return null;