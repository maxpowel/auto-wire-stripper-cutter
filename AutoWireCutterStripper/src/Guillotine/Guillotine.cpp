// Guillotine.cpp
#include "Guillotine.hpp"

#define STEPS_MM_EXTRUDER 135
#define STEPS_MM_LINEAR 404

#define DRIVER_ADDRESS   0b00   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f           // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)

// Constructor
Guillotine::Guillotine(int LINMOT_STEP, int LINMOT_DIR, int EXTRUDER_STEP, int EXTRUDER_DIR, HardwareSerial * SerialPort)
  : linMotStepper(AccelStepper::DRIVER, LINMOT_STEP, LINMOT_DIR), extruderStepper(AccelStepper::DRIVER, EXTRUDER_STEP, EXTRUDER_DIR), serial(SerialPort), TMCdriver(SerialPort, R_SENSE, DRIVER_ADDRESS) {

}

void Guillotine::begin() {
    serial->begin(115200);
    TMCdriver.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
    TMCdriver.toff(5);                 // Enables driver in software
    TMCdriver.rms_current(600);        // Set motor RMS current
    //TMCdriver.microsteps(256);         // Set microsteps
    TMCdriver.microsteps(16);         // Set microsteps
    TMCdriver.en_spreadCycle(false);   // false = StealthChop / true = SpreadCycl
    TMCdriver.pwm_autoscale(true);     // Needed for stealthChop
    TMCdriver.VACTUAL(0);
    //TMCdriver.VACTUAL(5000);

    extruderStepper.setMaxSpeed(10000);    // Velocidad máxima en pasos por segundo
    extruderStepper.setAcceleration(500); // Aceleración en pasos por segundo al cuadrado
     linMotStepper.setMaxSpeed(10000);    // Velocidad máxima en pasos por segundo
    linMotStepper.setAcceleration(500); // Aceleración en pasos por segundo al cuadrado
}

void Guillotine::run() {
    if (extruderStepper.distanceToGo() != 0) {
        extruderStepper.run();
    }
    
    if (linMotStepper.distanceToGo() != 0) {
        linMotStepper.run();
    }
    
}


// Mover la cuchilla (blade)
void Guillotine::moveBlade(float mm) {
    linMotStepper.move(mm * STEPS_MM_LINEAR);
}
void Guillotine::moveBladeSteps(int steps) {
    linMotStepper.move(steps);
}

void Guillotine::moveWire(float mm) {
   extruderStepper.move(mm * STEPS_MM_EXTRUDER);
}

void Guillotine::moveWireSteps(int steps) {
   extruderStepper.move(steps);
}

// Método para cortar
void Guillotine::cut() {
    //serial.println("Cutting wire...");
    moveBlade(500);  // Mueve la cuchilla hacia adelante
    delay(1000);     // Espera un momento
    retract();       // Retrae la cuchilla
}

// Método para pelar el cable
void Guillotine::strip() {
    //serial.println("Stripping wire...");
    moveWire(200);   // Avanza el cable
    cut();           // Corta el extremo del cable
}

// Método para retraer la cuchilla
void Guillotine::retract() {
    //serial.println("Retracting blade...");
    moveBlade(-500);  // Mueve la cuchilla hacia atrás
}
