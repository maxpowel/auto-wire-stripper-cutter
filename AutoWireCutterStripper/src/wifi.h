#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
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
  const float length = doc["len"];
  const float pos = doc["pos"];
  const int repetitions = doc["repetitions"];
  const float gauge = doc["gauge"];

  _ui->comps[0].value = pre;
  _ui->comps[1].value = length;
  _ui->comps[2].value = pos;


  _ui->comps[3].value = repetitions;
  _ui->comps[4].value = gauge;
  _ui->refresh();

  _guillotine->start(pre, length, pos, repetitions, gauge);
   // Construir una respuesta JSON
  StaticJsonDocument<200> response;
  response["status"] = "ok";
  response["values"] = doc;

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

void handleHome() {
  StaticJsonDocument<200> response;
  _guillotine->home();
  response["success"] = "ok";
  String jsonResponse;
  serializeJson(response, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

void handleStatus() {
  
  if (server.method() == 6) {
    // Options method
    server.send(200, "application/json", "{\"ok\":\"ok\"}");
    return;
  }

  if (server.hasArg("plain") == false) {
    // GET
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    array.add(_ui->comps[0].value);
    array.add(_ui->comps[1].value);
    array.add(_ui->comps[2].value);

    StaticJsonDocument<200> response;
    response["version"] = "beta-1";
    response["comps"] = array;
    response["repetitions"] = _ui->comps[3].value;
    response["gauge"] = _ui->comps[4].value;
    response["threshold"] = _guillotine->get_diag_treshold();
    String jsonResponse;
    serializeJson(response, jsonResponse);

    server.send(200, "application/json", jsonResponse);
  } else {
    //POST
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Leer datos del JSON
    const float pre = doc["pre"];
    const float length = doc["len"];
    const float pos = doc["pos"];
    const int repetitions = doc["repetitions"];
    const int gauge = doc["gauge"];

    _ui->comps[0].value = pre;
    _ui->comps[1].value = length;
    _ui->comps[2].value = pos;


    _ui->comps[3].value = repetitions;
    _ui->comps[4].value = gauge;
    _ui->refresh();

    //_guillotine->start(pre, length, pos, repetitions, gauge);
    // Construir una respuesta JSON
    StaticJsonDocument<200> response;
    response["status"] = "ok";
    response["values"] = doc;

    String jsonResponse;
    serializeJson(response, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  }
}

void handleConfig() {
  
  if (server.method() == 6) {
    // Options method
    server.send(200, "application/json", "{\"ok\":\"ok\"}");
    return;
  }

  if (server.hasArg("plain") == false) {
    // GET
    StaticJsonDocument<200> response;
    response["steps_mm_extruder"] = _guillotine->get_steps_mm_extruder();
    response["steps_mm_linear"] = _guillotine->get_steps_mm_linear();
    String jsonResponse;
    serializeJson(response, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  } else {
    //POST
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Leer datos del JSON
    const int steps_mm_extruder = doc["steps_mm_extruder"];
    const int steps_mm_linear = doc["steps_mm_linear"];
    _guillotine->set_steps_mm_linear(steps_mm_linear);
    _guillotine->set_steps_mm_extruder(steps_mm_extruder);
    struct config *c = get_config();
    c->steps_mm_extruder = steps_mm_extruder;
    c->steps_mm_linear = steps_mm_linear;
    writeConfig();

    // Construir una respuesta JSON
    StaticJsonDocument<200> response;
    response["status"] = "ok";
    String jsonResponse;
    serializeJson(response, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  }
}

void handleDiag() {

  if (server.method() == 6) {
    // Options method
    server.send(200, "application/json", "{\"ok\":\"ok\"}");
    return;
  }

  if (server.hasArg("plain") == false) {
    // GET
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    int *d = _guillotine->diag();
    array.add(d[0]);
    array.add(d[1]);
    StaticJsonDocument<200> response;
    response["diag"] = array;
    response["threshold"] = _guillotine->get_diag_treshold();
    String jsonResponse;
    serializeJson(response, jsonResponse);
    // Enviar la respuesta JSON de vuelta
    server.send(200, "application/json", jsonResponse);
  } else {
    //POST
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    // Leer datos del JSON
    const uint8_t threshold = doc["threshold"];
    _guillotine->set_diag_treshold(threshold);
    // Construir una respuesta JSON
    StaticJsonDocument<200> response;
    response["status"] = "ok";
    response["threshold"] = _guillotine->get_diag_treshold();
    response["threshold2"] = threshold;
    

    String jsonResponse;
    serializeJson(response, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  }
  
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
    server.on("/config", handleConfig);
    //server.on("/settings", HTTP_OPTIONS, sendCrossOriginHeader);
    //ui->comps[2].value = 43;
    server.on("/move", handleMove);
    server.on("/home", handleHome);
    server.on("/run", handleRun);
    server.on("/diag", handleDiag);
    server.begin();
    return &server;
}
