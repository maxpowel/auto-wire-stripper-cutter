#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "Ui/Ui.hpp"
#include <pin_configuration.h>

WebServer server(80);
Ui *_ui = NULL;
Guillotine *_guillotine = NULL;



void setCrossOrigin(){
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

void handleMove() {
  if (server.method() == 6) {
    // Options method
    server.send(200, "application/json", "{\"ok\":\"ok\"}");
    return;
  }

  if (server.hasArg("plain") == false) {
    
    // Si no hay cuerpo en la solicitud, respondemos con un error
    server.send(400, "application/json", "{\"error\":\"No JSON body received\"}");
    return;
  }

  String body = server.arg("plain");
  Serial.println(body);
  StaticJsonDocument<200> doc;

  // Intenta deserializar el JSON recibido
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  // Leer datos del JSON
  const int lin = doc["lin"];
  const int wire = doc["wire"];
  const int linStep = doc["linStep"];
  const int wireStep = doc["wireStep"];

   // Construir una respuesta JSON
  StaticJsonDocument<200> response;
  

  if (lin) {
    response["status"] = "ok";
    response["action"] = "lin";
    _guillotine->moveBlade(lin);
  } else if (wire) {
    response["status"] = "ok";
    response["action"] = "wire";
    _guillotine->moveWire(wire);
  } else if (linStep) {
    response["status"] = "ok";
    response["action"] = "linStep";
    _guillotine->moveBladeSteps(linStep);
  } else if (wireStep) {
    response["status"] = "ok";
    response["action"] = "wireStep";
    _guillotine->moveWireSteps(wireStep);
  } else {
    response["status"] = "fail";
    response["action"] = NULL;
  }
  
  String jsonResponse;
  serializeJson(response, jsonResponse);

  // Enviar la respuesta JSON de vuelta
  server.send(200, "application/json", jsonResponse);
}

void handleRun() {
  if (server.method() == 6) {
    // Options method
    server.send(200, "application/json", "{\"ok\":\"ok\"}");
    return;
  }

  if (server.hasArg("plain") == false) {
    
    // Si no hay cuerpo en la solicitud, respondemos con un error
    server.send(400, "application/json", "{\"error\":\"No JSON body received\"}");
    return;
  }

  String body = server.arg("plain");
  StaticJsonDocument<200> doc;

  // Intenta deserializar el JSON recibido
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  // Leer datos del JSON
  const float pre = doc["pre"];
  const float length = doc["length"];
  const float pos = doc["pos"];
  const int repetitions = doc["repetitions"];
  const int gauge = doc["gauge"];

  _guillotine->start(pre, length, pos, repetitions, gauge);
   // Construir una respuesta JSON
  StaticJsonDocument<200> response;
  response["status"] = "ok";

  String jsonResponse;
  serializeJson(response, jsonResponse);

  // Enviar la respuesta JSON de vuelta
  server.send(200, "application/json", jsonResponse);
}

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Error al abrir el archivo HTML");
    return;
  }

  String html = file.readString();
  file.close();
  server.send(200, "text/html", html);
}

void handleStatus() {
  server.send(200, "application/json", "{\"version\":\"beta-1\"}");
}


WebServer* wifi_begin(Ui *ui, Guillotine *guillotine) {
    _ui = ui;
    _guillotine = guillotine;
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    Serial.println("Connected");
    Serial.println(WiFi.localIP());

    if (!LittleFS.begin()) {
        Serial.println("Error al mounting LittleFS");
        LittleFS.format();
        //return;
    }

    server.enableCORS();
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    //server.on("/settings", HTTP_OPTIONS, sendCrossOriginHeader);
    ui->comps[2].value = 43;
    server.on("/move", handleMove);
    server.on("/run", handleRun);
    server.begin();
    return &server;
}
