/*
Angelo De Florio
recover_sensor_value.js

Funzione che recupera l'ultimo
valore salvato dai sensori
*/
let val = global.get("sensor_value") || 0;
let sensorName = global.get("sensor_name") || "Unknown";

// Risposta JSON da inviare all'Arduino
msg.payload = {
    sensor : sensorName,
    value  : val
};

return msg;