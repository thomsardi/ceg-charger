#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct NetworkSetting {
    String ssid;
    String pass;
    String ip;
    String gateway;
    String subnet;
    int8_t server;
    int8_t mode;
    bool flag = 0;
};

class JsonParser {
    public :
        JsonParser();
        NetworkSetting parseNetworkSetting(JsonVariant &json);
        int8_t parseReboot(JsonVariant &json);
    private :
};


#endif