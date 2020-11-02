#include <Arduino.h>

#define LONG_CLICK_TIME_DEFAULT 3000
#define UNPRESSED_VALUE_DEFAULT LOW
#define UNREAD -1

class Button {

    private:
        uint8_t pin;
        int lastReadValue = UNREAD;
        int unpressedValue;

        long longClickTime = LONG_CLICK_TIME_DEFAULT;
        long lastTimePressed = -1;

        void (*onTouchDown)(void);
        void (*onTouchUp)(void);
        void (*onShortClick)(void);
        void (*onLongClick)(void);

        void init(uint8_t pin, int inputType, byte unpressedValue, long longClickTime) {
            this->pin = pin;
            pinMode(pin, inputType);
            this->unpressedValue = unpressedValue;
            this->lastReadValue = unpressedValue;
            this->longClickTime = longClickTime;
        }

        void buttonReleased() {
            this->callTouchUp();
            if (lastTimePressed > 0) {
                if ((millis() - lastTimePressed ) > longClickTime) {
                    callLongClick();
                } else {
                    callShortClick();
                }
            }
            lastTimePressed = -1;
        }

        void buttonPressed() {
            lastTimePressed = millis();
            this->callTouchDown();
        }

        void callTouchDown() {
            if (onTouchDown != NULL) {
                onTouchDown();
            }
        }

        void callTouchUp() {
            if (onTouchUp != NULL) {
                onTouchUp();
            }
        }

        void callShortClick() {
            if (onShortClick != NULL) {
                onShortClick();
            }
        }

        void callLongClick() {
            if (onLongClick != NULL) {
                onLongClick();
            }
        }

    public:
        Button(uint8_t pin) {
            init(pin, INPUT, UNPRESSED_VALUE_DEFAULT, LONG_CLICK_TIME_DEFAULT);
        }

        Button(uint8_t pin, int inputType, int unpressedValue) {
            init(pin, inputType, unpressedValue, LONG_CLICK_TIME_DEFAULT);
        }

        void setLongClickTime(long longClickTime) {
            longClickTime = longClickTime;
        }

        void setClickAction(void (*f)(void)) {
            onShortClick = f;
        }

        void setLongClickAction(void (*f)(void)) {
            onLongClick = f;
        }

        void setUpAction(void (*f)(void)) {
            onTouchUp = f;
        }

        void setDownAction(void (*f)(void)) {
            onTouchDown = f;
        }

        void tick() {
            int newValue = digitalRead(pin);
            if (lastReadValue != UNREAD) {
                if (newValue != lastReadValue) {
                    if (newValue == unpressedValue) {
                        buttonReleased();
                    } else {
                        buttonPressed();
                    }
                }
            }
            lastReadValue = newValue;
        }
};