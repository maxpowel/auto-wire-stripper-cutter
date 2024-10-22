// Guillotine.hpp
#ifndef GUILLOTINE_HPP
#define GUILLOTINE_HPP

#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper

enum State {
    IDLE,
    BLADE_CUT,
    BLADE_UP,
    PRE_WIRE_MOVE,
    BLADE_STRIP,
    LENGTH_WIRE_MOVE,
    POST_WIRE_MOVE,

};


class Guillotine {
  private:
    AccelStepper linMotStepper;
    AccelStepper extruderStepper;
    HardwareSerial *serial;
    TMC2209Stepper TMCdriver;
    State currentState = IDLE;
    State previousState = IDLE;
    int cablesPending = 0;
    // Count how many strips as been done in the iteration
    int totalStrips = 0;

    // Job variables
    float pre_wire;
    float length_wire;
    float pos_wire;
    float gauge;
    int _diag1_pin;
    int _diag2_pin;
    int _diag[2];
    int _diag_threshold;
    bool homing;
    int home_ticks;
    bool valid_home;
    //Config
    int steps_mm_extruder;
    int steps_mm_linear;
    
     // State methods
    void state_blade_cut(State fromState);
    void state_blade_up(State fromState);
    void state_pre_wire_move(State fromState);
    void state_length_wire_move(State fromState);
    void state_post_wire_move(State fromState);
    void state_blade_strip(State fromState);

  public:
    // Constructor
    Guillotine(int LINMOT_STEP, int LINMOT_DIR, int EXTRUDER_STEP, int EXTRUDER_DIR, HardwareSerial * SerialPort, int DIAG1_PIN, int DIAG2_PIN);
    void start(float pre, float length, float pos, int repetitions, float cable_gauge);
    void run();
    void begin();
    // Métodos para mover la cuchilla y el alambre
    void moveBladeSteps(int steps);
    void moveBlade(float mm);
    void moveWire(float mm);
    void moveWireSteps(int steps);
    int *diag();
    void set_diag_treshold(uint8_t threshold);
    uint8_t get_diag_treshold();
    void home();

    void set_steps_mm_extruder(int value);
    void set_steps_mm_linear(int value);
    int get_steps_mm_extruder();
    int get_steps_mm_linear();


   
};

#endif
