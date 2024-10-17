// Guillotine.cpp
#include "Guillotine.hpp"

#define STEPS_MM_EXTRUDER 135
#define STEPS_MM_LINEAR 404

#define DRIVER_ADDRESS   0b00   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f           // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)


// States description
// Cut: bladeCut, bladeUp
// Precut: preWireMove -> bladeStrip -> (retract?) -> bladeUp
// Wire lenght mode: lengthWireMove ->
// Postcut: bladeStrip -> bladeUp -> postWireMove
// Back to cut

// Idle is always followed by bladeCut
// bladeCut is always followed by bladeUp when: motor position is cut position
// bladeUp is followed by:
//     preWireMove: 
//     lengthWireMove
//     postWireMove

// preWireMove is always followed by bladeStrip: when wire move is done
// bladeStrip is always followed by bladeUp
//


// Constructor
Guillotine::Guillotine(int LINMOT_STEP, int LINMOT_DIR, int EXTRUDER_STEP, int EXTRUDER_DIR, HardwareSerial * SerialPort)
  : linMotStepper(AccelStepper::DRIVER, LINMOT_STEP, LINMOT_DIR), extruderStepper(AccelStepper::DRIVER, EXTRUDER_STEP, EXTRUDER_DIR), serial(SerialPort), TMCdriver(SerialPort, R_SENSE, DRIVER_ADDRESS) {
    

}


void Guillotine::begin() {
    serial->begin(115200);
    TMCdriver.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
    TMCdriver.toff(5);                 // Enables driver in software
    TMCdriver.rms_current(900);        // Set motor RMS current
    //TMCdriver.microsteps(256);         // Set microsteps
    TMCdriver.microsteps(16);         // Set microsteps

    //TMCdriver.en_spreadCycle(true);  // To make the motors work warm
    TMCdriver.en_spreadCycle(false);   // false = StealthChop / true = SpreadCycl
    TMCdriver.pwm_autoscale(true);     // Needed for stealthChop
    TMCdriver.VACTUAL(0);
    TMCdriver.TCOOLTHRS(0xFFFFF);  // Configura la temperatura de umbral para protección
    //TMCdriver.TPOWERDOWN(128);// Tiempo de espera antes de reducir la corriente
    //TMCdriver.VACTUAL(5000);

    extruderStepper.setMaxSpeed(10000);    // Velocidad máxima en pasos por segundo
    extruderStepper.setAcceleration(500); // Aceleración en pasos por segundo al cuadrado
    linMotStepper.setMaxSpeed(10000);    // Velocidad máxima en pasos por segundo
    linMotStepper.setAcceleration(500); // Aceleración en pasos por segundo al cuadrado
}

void Guillotine::start(float pre, float length, float pos, int repetitions, int gauge) {
    Serial.println("Process started");
    pre_wire = pre;
    length_wire = length;
    pos_wire = pos;
    cablesPending = repetitions;
    gauge = gauge;
}

//
void Guillotine::state_blade_cut(State fromState) {
    Serial.println("Cutting");
    // TODO cambiar -2 por el gague
    linMotStepper.moveTo(-2);
    // Update the status
    previousState = fromState;
    currentState = BLADE_CUT;
}


void Guillotine::state_pre_wire_move(State fromState) {
    Serial.println("Pre move");
    // TODO cambiar -2 por el valor real
    extruderStepper.move(10);
    // Update the status
    previousState = fromState;
    currentState = PRE_WIRE_MOVE;
}

void Guillotine::state_length_wire_move(State fromState) {
    Serial.println("Moving wire length");
    // TODO cambiar -2 por el valor real
    extruderStepper.move(10);
    // Update the status
    previousState = fromState;
    currentState = LENGTH_WIRE_MOVE;
}

void Guillotine::state_post_wire_move(State fromState) {
    Serial.println("Post move");
    // TODO cambiar -2 por el valor real
    extruderStepper.move(10);
    // Update the status
    previousState = fromState;
    currentState = POST_WIRE_MOVE;
}



void Guillotine::state_blade_up(State fromState) {
    Serial.println("Blade up");
    // TODO cambiar 2 por el gague
    linMotStepper.moveTo(2);
    // Update the status
    previousState = fromState;
    currentState = BLADE_UP;
}

void Guillotine::state_blade_strip(State fromState) {
    Serial.println("Striping");
    // TODO cambiar -1 por el gague
    linMotStepper.moveTo(-1);
    totalStrips++;
    // Update the status
    previousState = fromState;
    currentState = BLADE_STRIP;
}


//
void Guillotine::run() {

    // Hacer machine.run()
    if (extruderStepper.distanceToGo() != 0) {
        extruderStepper.run();
    }
    
    if (linMotStepper.distanceToGo() != 0) {
        linMotStepper.run();
    }
    
    // State machine when running
    switch (currentState) {
            case IDLE:
                if (cablesPending > 0) {
                    // Starting the process
                    state_blade_cut(IDLE);
                }
                break;

            case BLADE_CUT:
                // Change the state only when the cut is finished
                if (linMotStepper.distanceToGo() == 0) {
                    if (previousState == IDLE) {
                        state_blade_up(BLADE_CUT);
                    } else if(previousState == POST_WIRE_MOVE) {
                        cablesPending--;
                        Serial.println("Cables pending");
                        Serial.println(cablesPending);
                        if (cablesPending > 0) {
                            state_blade_up(BLADE_CUT);
                        } else {
                            // ALL DONE, reset state
                            Serial.println("JOB DONE");
                            currentState = IDLE;
                            previousState = IDLE;
                        }
                    } else {
                        Serial.println("INVALID STATE AT BLADE CUT");
                    }
                    
                }
                break;

            case BLADE_UP:
                // Change the state only when the up move is finished
                if (linMotStepper.distanceToGo() == 0) {
                    if (previousState == BLADE_CUT) {
                        state_pre_wire_move(BLADE_UP);
                    } else if(totalStrips == 1) {
                        state_length_wire_move(BLADE_UP);
                    } else if (totalStrips == 2) {
                        state_post_wire_move(BLADE_UP);
                    } else {
                        Serial.println("INVALID STATE AT BLADE UP");
                    }
                    
                }
                break;
                
            case PRE_WIRE_MOVE:
                if (extruderStepper.distanceToGo() == 0) {
                    state_blade_strip(PRE_WIRE_MOVE);
                }
                break;

            case BLADE_STRIP:
                if (linMotStepper.distanceToGo() == 0) {
                    state_blade_up(BLADE_STRIP);
                }
                break;

            case LENGTH_WIRE_MOVE:
                if (extruderStepper.distanceToGo() == 0) {
                    state_blade_strip(LENGTH_WIRE_MOVE);
                }
                break;

            case POST_WIRE_MOVE:
                if (extruderStepper.distanceToGo() == 0) {
                    state_blade_cut(POST_WIRE_MOVE);
                    totalStrips = 0;
                }
                
                break;

            default:
                Serial.println("Estado desconocido");
                break;
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


void Guillotine::home() {
   //TODO hacer el homing
}
