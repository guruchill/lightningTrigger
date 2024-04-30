#include <Arduino.h>
#include <SPI.h>
#include "BLECamera.h"
#include "BLEHandler.h"
#include "RemoteStatus.h"
#include "InputHandler.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define WIRE Wire

BLECamera camera;
BLEHandler handler;
Adafruit_SSD1306 display;

void resetTest(void) {
    Serial.println("Clearing bonds");
    handler.clearBonds();
}

void setup()
{
    // Setup the red LED
    pinMode(PIN_LED1, OUTPUT);

    // Configure the Neopixel Status LED
    RemoteStatus *rs = rs->access();
    rs->set(Status::BOOT);

    // Setup button handling
    Input::Init(&camera);

    Input::registerResetCallback(resetTest);

// Debug nation bro
#if CFG_DEBUG
    Serial.begin(115200);
    rs->set(Status::WAIT_FOR_SERIAL);
    while (!Serial)
        delay(10);
#endif

    //Setup the SSD1306
    display = Adafruit_SSD1306(128, 32, &WIRE);
    display.begin(SSD1306_SWITCHCAPVCC, 0x78); 
    handler.display = &display;
    camera.display = &display;
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Camera Remote");
    display.setCursor(0,0);
    display.display(); // actually display all of the above

    // Initialze BLE
    if (!handler.InitBLE(&camera))
    {
        rs->set(Status::ERROR);
    }
}

void loop()
{
    // Netflix & Chill
    yield();

    // Check for button presses
    Input::process(millis());
}