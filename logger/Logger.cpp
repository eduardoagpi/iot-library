/*
Usage:

At globals:
Logger logger = Logger();

then:
void setup() {
    logger.init();
    ...
}

*/
#include <Arduino.h>
#include <WiFi.h>

WiFiServer server(23); // --> default port for communication usign TELNET protocol | Server Instance

class Logger
{
private:
    WiFiClient telnetClient;

public:

    void init() {
        server.begin();
        server.setNoDelay(true);
    }

    void println(const char *text)
    {
        /*
        if (telnetClient != NULL)
        {*/
        telnetClient.println(text);
        //}
    }

    void println(const Printable &x)
    {
        if (telnetClient != NULL)
        {
            telnetClient.println(x);
        }
    }

    void println(const bool x)
    {
        if (telnetClient != NULL)
        {
            telnetClient.println(x);
        }
    }

    void handleNewIncommingTelnetClient()
    {
        if (server.hasClient())
        {
            if (telnetClient)
            {
                telnetClient.stop();
            }
            telnetClient = server.available();
            Serial.println(telnetClient == NULL);
            telnetClient.flush(); // clear input buffer, else you get strange characters
            //logger.setTelnetClient(&client);
            Serial.println("client set");
        }
    }
};