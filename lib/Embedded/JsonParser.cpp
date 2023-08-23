#include "JsonParser.h"

JsonParser::JsonParser()
{

}

NetworkSetting JsonParser::parseNetworkSetting(JsonVariant &json)
{
    NetworkSetting setting;
    if(!json.containsKey("ssid"))
    {
        return setting;
    }

    if(!json.containsKey("pass"))
    {
        return setting;
    }

    if(!json.containsKey("ip"))
    {
        return setting;
    }
    if(!json.containsKey("gateway"))
    {
        return setting;
    }
    if(!json.containsKey("subnet"))
    {
        return setting;
    }
    if(!json.containsKey("server"))
    {
        return setting;
    }
    if(!json.containsKey("mode"))
    {
        return setting;
    }
    
    auto ssid = json["ssid"].as<String>();
    auto pass = json["pass"].as<String>();
    auto ip = json["ip"].as<String>();
    auto gateway = json["gateway"].as<String>();
    auto server = json["server"].as<int8_t>();
    auto mode = json["mode"].as<int8_t>();
    setting.ssid = ssid;
    setting.pass = pass;
    setting.ip = ip;
    setting.gateway = gateway;
    setting.server = server;
    setting.mode = mode;
    setting.flag = 1;
    return setting;
}

int8_t JsonParser::parseReboot(JsonVariant &json)
{
    if(!json.containsKey("reboot"))
    {
        return -1;
    }
    
    auto reboot = json["reboot"].as<int8_t>();
    return reboot;
}