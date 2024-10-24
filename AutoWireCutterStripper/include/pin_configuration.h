
// Seteppers configuration
#ifndef PIN_CONFIG
#define PIN_CONFIG
//Linear motion
const int LINMOT_STEPPERS_STEP_PIN = 25;
const int LINMOT_STEPPERS_DIR_PIN = 26;
const int LINMOT_DIAG1_PIN = 33;
const int LINMOT_DIAG2_PIN = 35;

// The stepper that moves the wire in the extruder.
const int EXTRUDER_STEPPER_STEP_PIN = 23;  
const int EXTRUDER_STEPPER_DIR_PIN = 27;;


// Encoder
const int ENCODER_PIN_A = 18;
const int ENCODER_PIN_B = 19;
const int ENCODER_BTN_PIN = 5;

// OLED display
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

#define ENABLE_WIFI true
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"

#endif
