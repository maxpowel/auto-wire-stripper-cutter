// Ui.cpp
#include "Ui.hpp"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <EC11.hpp>

#define DISPLAY_I2C_ADDRESS 0x3c

// Define the encoder here because we have to access it from the interruptions
static EC11 encoder;
static int _ENCODER_PIN_A = 0;
static int _ENCODER_PIN_B = 0;

// Display constants
const int TEXT_SIZE = 2;
const int TEXT_OFFSET = 3;
// TODO corregir esta ñapa
#define SCREEN_WIDTH_TEMPORAL 128
// Check what is still used

// Values for drawing the wire at the top of the OLED screen.
const int WIRE_STRAND_LENGTH = 30;
const int WIRE_RETRACTION = -1;
const int WIRE_STRAND_Y_POS = 7;
const int WIRE_INSULATION_WIDTH = SCREEN_WIDTH_TEMPORAL - (WIRE_STRAND_LENGTH * 2);
const int WIRE_INSULATION_HEIGHT = 14;

// These are just references to the corresponding component index in the comps array;
const int STRIPPING_LENGTH1_INDEX = 0;
const int WIRE_LENGTH_INDEX = 1;
const int STRIPPING_LENGTH2_INDEX = 2;
const int QUANTITY_INDEX = 3;
const int STRIPPING_DEPTH_INDEX = 4;
const int START_BTN_INDEX = 5;

const int CUTTING_STEPS = 17750;  // Steps to move blade to fully cut the wire.
const int STRIPPING_MULTIPLIER = 300;  // The chosen stripping depth value on the screen is multiplied by this value.
const int WIRE_MOVEMENT_MULTI = 408;  // How much to move wire per unit on OLED, turn on CALIBRATION_MODE to find this value.
const int DELAY_BETWEEN_CUTS = 100;  // Delay in ms between each cut in the quantity.

///

void pinDidChange() {
  encoder.checkPins(digitalRead(_ENCODER_PIN_A), digitalRead(_ENCODER_PIN_B));
}


// Constructor
Ui::Ui(Guillotine *guillotine, int ENCODER_PIN_A, int ENCODER_PIN_B, int ENCODER_BTN_PIN, int SCREEN_WIDTH, int SCREEN_HEIGHT): display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
    _ENCODER_PIN_A = ENCODER_PIN_A;
    _ENCODER_PIN_B = ENCODER_PIN_B;
    _ENCODER_BTN_PIN = ENCODER_BTN_PIN;
    _encoder_value = 0;
    _guillotine = guillotine;

    // Components
    comps[0] = {0, 20, 40, 20, 0, 0, 0, 0};
    comps[1] = {44, 20, 40, 20, 0, 0, 0, 0};
    comps[2] = {88, 20, 40, 20, 0, 0, 0, 0};
    comps[3] = {0, 44, 40, 20, 0, 0, 0, 0};
    comps[4] = {44, 44, 40, 20, 0, 0, 0, 0};
    comps[5] = {88, 44, 40, 20, 0, 0, 0, 1};
}

void Ui::begin() {
    
    pinMode(_ENCODER_BTN_PIN, INPUT_PULLUP);

    // We can use internal pull-up with the encoder pins, assuming pin C is simply grounded.
    pinMode(_ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(_ENCODER_PIN_B, INPUT_PULLUP);
    
    attachInterrupt(_ENCODER_PIN_A, pinDidChange, CHANGE);
    attachInterrupt(_ENCODER_PIN_B, pinDidChange, CHANGE);

    display.begin(DISPLAY_I2C_ADDRESS, true);

}

void Ui::refresh() {
    handleOLEDDisplay();
}

void Ui::run() {
    EC11Event e;
    if (encoder.read(&e)) {

        // OK, got an event waiting to be handled.
        
        if (e.type == EC11Event::StepCW) {
            _encoder_value += e.count;
        } else {
            _encoder_value -= e.count;
        }
    }

    boolean encBtnState = digitalRead(_ENCODER_BTN_PIN);
    

    // Only update OLED screen if encoder is moved or pressed.
    if ((_encoder_value != encoderLastPosMain) || (encBtnState != encBtnPrevStateMain)) {
        handleOLEDDisplay();
    }

    /*if (comps[START_BTN_INDEX].selected) {
        runAutoCuttingStripping();
    }*/

    encoderLastPosMain = _encoder_value;
    encBtnPrevStateMain = encBtnState;
}

int Ui::encoderValue() {
    return _encoder_value;
}




void Ui::drawWire() {
    display.drawLine(0, WIRE_STRAND_Y_POS, WIRE_STRAND_LENGTH, WIRE_STRAND_Y_POS, SH110X_WHITE);
    display.fillRect(WIRE_STRAND_LENGTH, 0, WIRE_INSULATION_WIDTH, WIRE_INSULATION_HEIGHT, SH110X_WHITE);
    display.drawLine(WIRE_STRAND_LENGTH + WIRE_INSULATION_WIDTH, WIRE_STRAND_Y_POS, SCREEN_WIDTH_TEMPORAL, WIRE_STRAND_Y_POS, SH110X_WHITE);
}


void Ui::drawText(String text, int x, int y) {
    display.setCursor(x, y);
    display.println(text);
}


void Ui::drawComponent(Component comp) {
    if (comp.highlighted) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
        display.fillRect(comp.x, comp.y, comp.w, comp.h, SH110X_WHITE);

        if (comp.selected) {
            display.drawRect(comp.x - 1, comp.y - 1, comp.w + 2, comp.h + 2, SH110X_WHITE);
        }

    }
    else {
        display.setTextColor(SH110X_WHITE, SH110X_BLACK);
        display.drawRect(comp.x, comp.y, comp.w, comp.h, SH110X_WHITE);
    }

    if (comp.btn) {
        display.setTextSize(1);
        drawText("Start", comp.x + TEXT_OFFSET, comp.y + TEXT_OFFSET);
    }
    else {
        display.setTextSize(TEXT_SIZE);
        drawText(String(comp.value), comp.x + TEXT_OFFSET, comp.y + TEXT_OFFSET);
    }
}


void Ui::handleAllComponents() {
    for (int i = 0; i < NUM_OF_COMPS; i++) {
        Component& comp = comps[i];
        if (encoderPos == i) {
            comp.highlighted = true;

            if (encBtnState) {
                if (!comp.selected && !comp.btn) {
                    _encoder_value = comp.value;
                }

                comp.selected = true;

                int newEncPos = _encoder_value;
                comp.value = newEncPos;
            }
            else {
                comp.selected = false;                
            }

        }
        else {
            comp.highlighted = false;
            comp.selected = false;
        }

        drawComponent(comp);
    }
}

void Ui::handleOLEDDisplay() {
    display.clearDisplay();

    drawWire();
    // TODO move to interrupt
    boolean btnState = digitalRead(_ENCODER_BTN_PIN);

    // Handling whether encoder is changing cell or value of the cell.
    if (!btnState && (btnState != encBtnPrevState)) {
         encBtnState = !encBtnState;
        if (encBtnState) {
            encoderLastPos = encoderPos;
        }
        else {
            _encoder_value = encoderLastPos;
        }

    }

    encBtnPrevState = btnState;

    if (!encBtnState) {
        encoderPos = _encoder_value;
        if(encoderPos >= NUM_OF_COMPS) {
            encoderPos = 0;
            _encoder_value = 0;
        } else if(encoderPos < 0) {
            encoderPos = NUM_OF_COMPS-1;
            _encoder_value = NUM_OF_COMPS-1;
        }

    }

    handleAllComponents();

    display.display();
}