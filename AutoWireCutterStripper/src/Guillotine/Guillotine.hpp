// Guillotine.hpp
#ifndef GUILLOTINE_HPP
#define GUILLOTINE_HPP

#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper

class Guillotine {
  private:
    AccelStepper linMotStepper;
    AccelStepper extruderStepper;
    HardwareSerial *serial;
    TMC2209Stepper TMCdriver;

  public:
    // Constructor
    Guillotine(int LINMOT_STEP, int LINMOT_DIR, int EXTRUDER_STEP, int EXTRUDER_DIR, HardwareSerial * SerialPort);

    void run();
    void begin();
    // Métodos para mover la cuchilla y el alambre
    void moveBladeSteps(int steps);
    void moveBlade(float mm);
    void moveWire(float mm);
    void moveWireSteps(int steps);

    // Métodos para cortar, pelar y retraer
    void cut();
    void strip();
    void retract();
};

#endif
