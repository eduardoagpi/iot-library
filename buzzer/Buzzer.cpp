
#ifndef mqtt_manager
#define mqtt_manager

#include <Arduino.h>
#include "Tone32.h"

enum note_time {
    WHOLE,
    HALF,
    QUARTER,
    QUARTER_TRIPLET,
    EIGHT,
    EIGHT_TRIPLET,
    SIXTEENTH,
    SIXTEENTH_TRIPLET
};

struct note {
    long pitch;
    note_time duration;
};

class AsyncBuzzerPlayer
{
    private:
        unsigned int pin;
        note *notes;
        int length;
        int measureDuration;
        int currentNote = 0;
        bool enabled = false;
        bool sounding = false;

        unsigned long changeNoteUntil = 0;

        long wholeDuration;
        long half_duration;
        long quarter_duration;
        long quarter_triplet_duration;
        long eight_duration;
        long eight_triplet_duration;
        long sixteenth_duration;
        long sixteenth_triplet_duration;

        void initDurations() {
            half_duration = wholeDuration / 2;
            quarter_duration = wholeDuration / 4;
            quarter_triplet_duration = half_duration / 3;
            eight_duration = wholeDuration / 8;
            eight_triplet_duration = quarter_duration / 3;
            sixteenth_duration = wholeDuration / 16;
            sixteenth_triplet_duration = eight_triplet_duration / 3;
        }

        long getDuration(note_time noteDuration) {
            switch(noteDuration) {
                case WHOLE:
                    return wholeDuration;
                case HALF:
                    return half_duration;
                case QUARTER:
                    return quarter_duration;
                case QUARTER_TRIPLET:
                    return quarter_triplet_duration;
                case EIGHT:
                    return eight_duration;
                case EIGHT_TRIPLET:
                    return eight_triplet_duration;
                case SIXTEENTH:
                    return sixteenth_duration;
                case SIXTEENTH_TRIPLET:
                    return sixteenth_triplet_duration;
            }
        }

        void playCurrentNoteAndScheduleNext() {
            //delay(50);  // Just to diferentiate between notes (of the same pitch)
            //tone(pin, notes[currentNote].pitch, notes[currentNote].duration-10);

            ledcWriteTone(0, notes[currentNote].pitch);


            changeNoteUntil = millis() + getDuration(notes[currentNote].duration);
            currentNote++;
            if (currentNote >= length) currentNote = 0;
        }

    public :
        AsyncBuzzerPlayer(unsigned int _pin, note _melody[], int _length, long _wholeDuration) {
            pin = _pin;
            notes = _melody;
            length = _length;
            wholeDuration = _wholeDuration;
            initDurations();
            //pinMode(pin, OUTPUT);

            int channel = 0;
            int resolution = 8;
            int freq = 2000;
            ledcSetup(channel, freq, resolution);
            ledcAttachPin(pin, channel);
            ledcWrite(channel, 60);
        }

        void turnOn() {
            enabled = true;
            playCurrentNoteAndScheduleNext();
        }

        void tick() {
            if (!enabled || millis() < changeNoteUntil) return;
            playCurrentNoteAndScheduleNext();
        }

        void turnOff() {
            ledcWrite(0, 0);


            enabled = false;
            currentNote = 0;
        }
};

#endif
