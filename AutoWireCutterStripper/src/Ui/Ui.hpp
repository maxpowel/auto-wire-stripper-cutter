// Ui.hpp
#ifndef UI_HPP
#define UI_HPP

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include <Guillotine/Guillotine.hpp>

#define NUM_OF_COMPS 6

// A component is a cell with a value on the OLED display.
struct Component {
    int x, y;  // Position
    int w, h;  // Size
    int value;  // Current value of the cell.
    boolean highlighted;  // Whether the cell is the currently highlighted cell.
    boolean selected;  // Whether the cell is currently the selected one, where its value will be controlled by the encoder.
    boolean btn;  // If it is a button or not.
};

class Ui {
  private:
    Guillotine *_guillotine;
    int _ENCODER_BTN_PIN;
    int _encoder_value;
    Adafruit_SH1106G display;
    // Ui status
    Component comps[NUM_OF_COMPS];
    int encoderPos;  // Current position/value of the encoder.
    int encoderLastPos;  // For OLED drawing.
    int encoderLastPosMain;  // For main loop.

    boolean encBtnState = false;
    boolean encBtnPrevState = false;  // For OLED drawing.
    boolean encBtnPrevStateMain = false;  // For main loop.

    
    void drawWire();
    void drawText(String text, int x, int y);
    void drawComponent(Component comp);
    void handleAllComponents();
    void handleOLEDDisplay();
  public:
    // Constructor
    Ui(Guillotine *, int ENCODER_PIN_A, int ENCODER_PIN_B, int ENCODER_BTN_PIN, int SCREEN_WIDTH, int SCREEN_HEIGHT);
    int encoderValue();

    void run();
    void begin();

};

#endif
