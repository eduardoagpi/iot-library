#ifndef Digit_interuptor
#define Digit_interuptor

#include <Arduino.h>

class DigitalInterruptor {
    private:
        bool currentStatus;
        int pin;

        void init(int pin, bool initialValue) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
            currentStatus = false;

            writeStatus(initialValue);
        }

        void writeStatus(bool newValue) {
            if (newValue == currentStatus) return;
            if (newValue) {
                digitalWrite(pin, HIGH);
                currentStatus = true;
            } else {
                digitalWrite(pin, LOW);
                currentStatus = false;
            }
        }

    public:
        DigitalInterruptor(int pin, bool initialValue = false) {
            init(pin, initialValue);
        }

        void turnOn() {
            writeStatus(true);
        }

        void turnOff() {
            writeStatus(false);
        }

        boolean isTurnedOn() {
            return currentStatus;
        }

};

#endif