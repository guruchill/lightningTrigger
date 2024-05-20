#pragma once
#include "InputDebounce.h"
#include "BLECamera.h"


#define DEBOUNCE_DELAY 20
#define UP_BUTTON_PIN 7
#ifdef NRF52832
#define DOWN_BUTTON_PIN 15
#define SELECT_SWITCH_PIN 11
#define LIGHTNING_TRIGGER_PIN 16
#define ANALOG_SENSOR PIN_A5
#else
#define DOWN_BUTTON_PIN 15
#define SELECT_SWITCH_PIN 11
#define LIGHTNING_TRIGGER_PIN 16
#define ANALOG_SENSOR PIN_A5
#endif



static InputDebounce thresholdUpButton;
static InputDebounce thresholdDownButton;
static InputDebounce selectSwitch;
static InputDebounce LightningTrigger;

typedef void (*button_callback)(void);



class Input {
public:
    
    static inline BLECamera *_camera_ref = nullptr;
    static bool Init(BLECamera *newcam);

    static void process(unsigned long time);

    static void registerResetCallback(button_callback cb);
    static int triggerThreshold;
    static Adafruit_SSD1306 display;

    static void drawDisplay(int sensorLevel=0 );
    static String BannerText;


private:

    static void readStartup(void);

    static void upButton(uint8_t pinIn);
    static void releaseUpButton(uint8_t pinIn);
    
    static void downButton(uint8_t pinIn);
    static void releaseDownButton(uint8_t pinIn);
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
