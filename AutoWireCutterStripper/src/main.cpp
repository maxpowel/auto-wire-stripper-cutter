#include <pin_configuration.h>
#include "Guillotine/Guillotine.hpp"
#include "Ui/Ui.hpp"

Guillotine guillotine(LINMOT_STEPPERS_STEP_PIN, LINMOT_STEPPERS_DIR_PIN, EXTRUDER_STEPPER_STEP_PIN, EXTRUDER_STEPPER_DIR_PIN, &Serial2);
Ui ui(&guillotine, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BTN_PIN, SCREEN_WIDTH, SCREEN_HEIGHT);

/*
void runAutoCuttingStripping() {
    if (CALIBRATION_MODE) {
        guillotine.moveWire(comps[STRIPPING_LENGTH1_INDEX].value);
        guillotine.cut();
    }
    else {
        guillotine.cut();
        delay(DELAY_BETWEEN_CUTS);

        for (int i = 0; i < comps[QUANTITY_INDEX].value; i++) {
            guillotine.moveWire(comps[STRIPPING_LENGTH1_INDEX].value);
            guillotine.strip();
            guillotine.moveWire(comps[WIRE_LENGTH_INDEX].value);
            guillotine.strip();
            guillotine.moveWire(comps[STRIPPING_LENGTH2_INDEX].value);
            guillotine.cut();
            delay(DELAY_BETWEEN_CUTS);
        }
    }

    comps[START_BTN_INDEX].selected = false;
    encBtnState = false;
}
*/



void setup() {
    Serial.begin(115200);

    guillotine.begin();
    ui.begin();
    //guillotine.moveWire(10);
    //guillotine.moveWireSteps(10000);
    //guillotine.moveBladeSteps(-5000);
    //guillotine.moveBlade(10);
}




void loop() {
    // Execute motor moves
    guillotine.run();
    // Check UI changes
    ui.run();

}