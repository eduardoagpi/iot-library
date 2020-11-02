#ifndef Exec_ticket
#define Exec_ticket

#include <Arduino.h>
#include <functional>

/*
Inspired in this callback solution approaches:
https://stackoverflow.com/a/45525074/4910439
https://stackoverflow.com/a/40944576/4910439
Some info about callbacks at C/C++:
https://stackoverflow.com/questions/2298242/callback-functions-in-c
*/

class ExecutionTicker
{

private:
    long lastExecution = 0;
    long interval = 0;
    std::function<void()> callback;
    bool enabled = true;

public:
    ExecutionTicker(long _interval, std::function<void()> callb, bool enabled = true)
    {
        interval = _interval;
        callback = callb;
    }

    void tick()
    {
        if (millis() - lastExecution > interval)
        {
            if (enabled)
                callback();
            lastExecution = millis();
        }
    };
    void changeInterval(long _newInterval)
    {
        interval = _newInterval;
    }
    void enable()
    {
        enabled = true;
    }
    void disable()
    {
        enabled = false;
    }
};

#endif
