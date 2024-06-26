#include "InputHandler.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define TriggerLevel 800
#define ThresholdSkip 50 
#define MaxThreshold 1000


int Input::triggerThreshold = 0;
Adafruit_SSD1306 Input::display;
String Input::BannerText = "CameraRemote";

bool Input::Init(BLECamera *newcam)
{
    _camera_ref = newcam;
    canReset = true;

    thresholdUpButton.registerCallbacks(upButton, releaseUpButton, NULL, resetCheck);
    thresholdDownButton.registerCallbacks(downButton, releaseDownButton, NULL, NULL);
    selectSwitch.registerCallbacks(switch_on, switch_off, NULL, NULL);
    LightningTrigger.registerCallbacks (lightning_on, lightning_off, NULL, NULL);

    thresholdUpButton.setup(UP_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    thresholdDownButton.setup(DOWN_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    selectSwitch.setup(SELECT_SWITCH_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    LightningTrigger.setup(LIGHTNING_TRIGGER_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);

    triggerThreshold = TriggerLevel;

    Input::readStartup();
}

void Input::readStartup(void)
{
    if (digitalRead(SELECT_SWITCH_PIN) == HIGH)
    {
        Input::switch_on(0);
    }
    else
    {
        Input::switch_off(0);
    }
}

void Input::process(unsigned long time)
{
    thresholdUpButton.process(time);
    thresholdDownButton.process(time);
    selectSwitch.process(time);
    LightningTrigger.process(time);

    //Get the analog input for the lightning sensor
    unsigned long  preTriggerTime = millis();
    int sensorValue = analogRead(ANALOG_SENSOR);

    if ( sensorValue > triggerThreshold )
    {
       
        Serial.println("Lightning Detected");
        unsigned long  preShutterTime = millis();
        lightning_on(0);
        unsigned long  postShutterTime = millis();
        Serial.printf("Time before ADC check %d\n", preTriggerTime);
        Serial.printf("Time before shutter fire %d\n", preShutterTime);
        Serial.printf("Time after shutter fire %d\n", postShutterTime);
        Serial.printf("ADC time %d\n", preShutterTime - preTriggerTime);
        Serial.printf("Time to fire %d\n", postShutterTime - preShutterTime);
        
        Serial.println("");
        
    }

    drawDisplay(sensorValue);
    

}

void Input::upButton(uint8_t pinIn)
{
    Serial.println("Threshold Up");
    triggerThreshold = triggerThreshold + ThresholdSkip;
    if (triggerThreshold > MaxThreshold)
    {
        triggerThreshold = MaxThreshold;
    }
  
}

void Input::releaseUpButton(uint8_t pinIn)
{


}

void Input::downButton(uint8_t pinIn)
{
    Serial.println("Threshold Down");
    triggerThreshold = triggerThreshold - ThresholdSkip;
    if (triggerThreshold < 0 )
    {
        triggerThreshold = 0;
    }
   drawDisplay();
}

void Input::releaseDownButton(uint8_t pinIn)
{
    Serial.println("Focus Off");
    _camera_ref->focus(false);
}

void Input::lightning_off(uint8_t pinIn)
{
   Serial.println("Lightning Off");
   _camera_ref->focus(false);
}

void Input::lightning_on(uint8_t pinIn)
{
    Serial.println("Lightning On");
    unsigned long  preTriggerTime = millis();
    _camera_ref->focus(true);
    _camera_ref->pressTrigger();
    unsigned long  postTriggerTime = millis();
    Serial.printf("*** Time to trigger %d\n", postTriggerTime - preTriggerTime); 
    _camera_ref->releaseTrigger();
    _camera_ref->focus(false);
}

void Input::resetCheck(uint8_t pinIn, unsigned long duration)
{
    if (duration > 10000 && millis() < 25000 && canReset)
    {
        // Clear bonds
        if (_resetCallback)
        {
            _resetCallback();
        }
    }
    canReset = false;
}

void Input::registerResetCallback(button_callback cb)
{
    _resetCallback = cb;
}

// Switch specific

void Input::switch_on(uint8_t pinIn)
{
    Serial.println("Switch On");
    digitalWrite(3, HIGH);
    _camera_ref->setMode(MANUAL_FOCUS);
}

void Input::switch_off(uint8_t pinIn)
{
    Serial.println("Switch Off");
    digitalWrite(3, LOW);
    _camera_ref->setMode(AUTO_FOCUS);
}

void Input::drawDisplay(int sensorLevel)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println(BannerText);
    display.printf("Sensitivity %d\n", triggerThreshold);
    
    int currentLevel = (int)((float)sensorLevel/1000*128);
    int triggerLevel = (int)((float)triggerThreshold/1000*128);
    //display.printf("Sensor Level %d\n", currentLevel);
    display.fillRect(0,30,currentLevel,10,SSD1306_WHITE);
    display.fillTriangle(triggerLevel, 30, triggerLevel-5, 25, triggerLevel+5, 25, SSD1306_WHITE);


    display.display();
}