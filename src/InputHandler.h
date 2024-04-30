#pragma once
#include "InputDebounce.h"
#include "BLECamera.h"

#define DEBOUNCE_DELAY 20
#define SHUTTER_BUTTON_PIN 7
#define FOCUS_BUTTON_PIN 15
#define SELECT_SWITCH_PIN 11
#define LIGHTNING_TRIGGER_PIN 16


static InputDebounce shutterButton;
static InputDebounce focusButton;
static InputDebounce selectSwitch;
static InputDebounce LightningTrigger;

typedef void (*button_callback)(void);

class Input {
public:
    static inline BLECamera *_camera_ref = nullptr;
    static bool Init(BLECamera *newcam);

    static void process(unsigned long time);

    static void registerResetCallback(button_callback cb);

private:

    static void readStartup(void);

    static void pressTrigger(uint8_t pinIn);
    static void releaseTrigger(uint8_t pinIn);
    
    static void pressFocus(uint8_t pinIn);
    static void releaseFocus(uint8_t pinIn);
    static void pressedDurationCallback(uint8_t pinIn, unsigned long duration);

    //Reset logic
    static void resetCheck(uint8_t pinIn, unsigned long duration);
    static inline bool canReset;

    static void switch_on(uint8_t pinIn);
    static void switch_off(uint8_t pinIn);

    static void lightning_on(uint8_t pinIn);
    static void lightning_off(uint8_t pinIn);
    


    static inline button_callback _resetCallback = nullptr;
};
