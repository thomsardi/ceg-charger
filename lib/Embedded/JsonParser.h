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
    int8_t server; //server = 1 is for static, 2 for dhcp
    int8_t mode; //mode = 1 is for AP, 2 for Station
    bool flag = 0;
};

class JsonParser {
    public :
        JsonParser();
        NetworkSetting parseNetworkSetting(JsonVariant &json);
        int8_t parseReboot(JsonVariant &json);
        int8_t parseFactoryReset(JsonVariant &json);
        String getNetworkInfo(NetworkSetting &networkSetting);
    private :
};


#endif