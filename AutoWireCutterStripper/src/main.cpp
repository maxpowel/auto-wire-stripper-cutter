#include <pin_configuration.h>
#include "Guillotine/Guillotine.hpp"
#include "Ui/Ui.hpp"


Guillotine guillotine(LINMOT_STEPPERS_STEP_PIN, LINMOT_STEPPERS_DIR_PIN, EXTRUDER_STEPPER_STEP_PIN, EXTRUDER_STEPPER_DIR_PIN, &Serial2);
Ui ui(&guillotine, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BTN_PIN, SCREEN_WIDTH, SCREEN_HEIGHT);


#ifdef ENABLE_WIFI
#include "wifi.h"
#endif





void setup() {
    Serial.begin(115200);

    guillotine.begin();
    ui.begin();

    #ifdef ENABLE_WIFI
    wifi_begin(&ui, &guillotine);
    #endif
    
    //guillotine.moveWire(10);
    //guillotine.moveWireSteps(10000);
    //guillotine.moveBladeSteps(-5000);
    //guillotine.moveBlade(10);
    guillotine.start(10, 10, 10, 5, 5);
}




void loop() {
    // Execute motor moves
    guillotine.run();
    // Check UI changes
    ui.run();
    // Server
    #ifdef ENABLE_WIFI
    server.handleClient();
    #endif
}