/*
Angelo De Florio
extract_data.js

Funzione che estrae i dati JSON di ChirpStack
*/
let payload = msg.payload;
let devEUI;
// Estrazione del devEUI
if (payload.deviceInfo) {
    devEUI = payload.deviceInfo.devEui;
} else {
    devEUI = "Sconosciuto";
}
let data = payload.object;

let sensorType     =  "Unknown";
let extractedValue =  null;
let metricName     =  "";

// Controllo per il sensore Ursalink EM500-SWL (Umidità del terreno)
if (data && data.moisture !== undefined) {
    sensorType     = "Ursalink EM500-SWL | Terreno";
    extractedValue = data.moisture;
    metricName     = "% Umidità suolo";
}
// Controllo per il sensore Milesight AM319-HCHO (HCHO)
else if (data && data.hcho !== undefined) {
    sensorType     = "Milesight AM319-HCHO | HCHO";
    extractedValue = data.hcho;
    metricName     = "mg/m^3 HCHO";
}
// Controllo per il sensore Milesight EM300-SDL (Leak)
else if (data && data.leakage_status !== undefined) {
    sensorType     = "Milesight EM300-SDL | Leakage Status";
    extractedValue = data.leakage_status;
}

// Creazione del payload JSON con tutti i dati
msg.payload = {
    devEUI : devEUI,
    sensor : sensorType,
    value  : extractedValue,
    unit   : metricName,
    raw    : data
};

return msg;