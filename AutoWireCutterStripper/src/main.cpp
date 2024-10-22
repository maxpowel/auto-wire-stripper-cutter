#include <pin_configuration.h>
#include "Guillotine/Guillotine.hpp"
#include "Ui/Ui.hpp"
#include "config.h"


Guillotine guillotine(LINMOT_STEPPERS_STEP_PIN, LINMOT_STEPPERS_DIR_PIN, EXTRUDER_STEPPER_STEP_PIN, EXTRUDER_STEPPER_DIR_PIN, &Serial2, LINMOT_DIAG1_PIN, LINMOT_DIAG2_PIN);
Ui ui(&guillotine, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BTN_PIN, SCREEN_WIDTH, SCREEN_HEIGHT);

#ifdef ENABLE_WIFI
#include "wifi.h"
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing");
    if(init_config() !=0) {
        Serial.println("Cannot initialize config");
        return;
    }
    struct config *config = get_config();
    guillotine.begin();
    //guillotine.set_steps_mm_extruder(config->steps_mm_extruder);
    //guillotine.set_steps_mm_linear(config->steps_mm_linear);

    guillotine.set_steps_mm_extruder(135);
    guillotine.set_steps_mm_linear(404);

    ui.begin();

    #ifdef ENABLE_WIFI
    wifi_begin(&ui, &guillotine);
    #endif
    Serial.println("Ready");
    //guillotine.moveWire(10);
    //guillotine.moveWireSteps(10000);
    //guillotine.moveBladeSteps(-5000);
    //guillotine.moveBlade(10);
    //guillotine.start(10, 10, 10, 5, 5);
    
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