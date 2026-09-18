/*
Angelo De Florio
socket_power.js
*/
let out;
if (msg.payload) {
    out = {};
    out.topic = "energysaver/downlink/24e124148c400670";
    out.payload = {
        "confirmed" : false,
        "fPort"     : 85,
        "data"      : Buffer.from("080100ff", "hex").toString("base64")
    };
    return out;
}

out = {};
out.topic = "energysaver/downlink/24e124148c400670";
out.payload = {
    "confirmed" : false,
    "fPort"     : 85,
    "data"      : Buffer.from("080000ff", "hex").toString("base64")
};
return out;