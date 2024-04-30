#include "InputHandler.h"

bool Input::Init(BLECamera *newcam)
{
    _camera_ref = newcam;
    canReset = true;

    shutterButton.registerCallbacks(pressTrigger, releaseTrigger, NULL, resetCheck);
    focusButton.registerCallbacks(pressFocus, releaseFocus, NULL, NULL);
    selectSwitch.registerCallbacks(switch_on, switch_off, NULL, NULL);
    LightningTrigger.registerCallbacks (lightning_on, lightning_off, NULL, NULL);

    shutterButton.setup(SHUTTER_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    focusButton.setup(FOCUS_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    selectSwitch.setup(SELECT_SWITCH_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);
    LightningTrigger.setup(LIGHTNING_TRIGGER_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES, 1, InputDebounce::ST_NORMALLY_OPEN);

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
    shutterButton.process(time);
    focusButton.process(time);
    selectSwitch.process(time);
    LightningTrigger.process(time);

    //Get the analog input for the lightning sensor
    int sensorValue = analogRead(ANALOG_SENSOR);
    //Serial.println(sensorValue);
    if ( sensorValue > TriggerLevel)
    {
        Serial.println("Lightning Detected");
        lightning_on(0);
    }
    

}

void Input::pressTrigger(uint8_t pinIn)
{
    Serial.println("Tringger On");
    _camera_ref->pressTrigger();
}

void Input::releaseTrigger(uint8_t pinIn)
{
    Serial.println("Trigger Off");
    _camera_ref->releaseTrigger();
}

void Input::pressFocus(uint8_t pinIn)
{
    Serial.println("Focus On");
    _camera_ref->focus(true);
}

void Input::releaseFocus(uint8_t pinIn)
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
    _camera_ref->focus(true);
    _camera_ref->pressTrigger();
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