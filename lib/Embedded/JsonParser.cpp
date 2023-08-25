#include "JsonParser.h"

JsonParser::JsonParser()
{

}

/**
 * @brief   get network information such as ssid and ip
 * @return  json formatted string
*/
String JsonParser::getNetworkInfo(const NetworkSetting &networkSetting)
{
    StaticJsonDocument<256> doc;
    String output;
    doc["ssid"] = networkSetting.ssid;
    doc["ip"] = networkSetting.ip;
    // doc["mode"] = networkSetting.mode;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief   get user network setting
 * @return  json formatted string
*/
String JsonParser::getUserNetworkSetting(const NetworkSetting &networkSetting)
{    
    StaticJsonDocument<256> doc;
    String output;
    doc["ssid"] = networkSetting.ssid;
    doc["pass"] = networkSetting.pass;
    doc["ip"] = networkSetting.ip;
    doc["gateway"] = networkSetting.gateway;
    doc["subnet"] = networkSetting.subnet;
    doc["server"] = networkSetting.server;
    doc["mode"] = networkSetting.mode;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief   parse json POST request
 * @return  NetworkSetting struct datatype that contain the setting, set the .flag attribute to 0 when fail, otherwise set to 1
*/
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
    auto subnet = json["subnet"].as<String>();
    auto server = json["server"].as<int8_t>();
    auto mode = json["mode"].as<int8_t>();
    setting.ssid = ssid;
    setting.pass = pass;
    setting.ip = ip;
    setting.gateway = gateway;
    setting.subnet = subnet;
    setting.server = server;
    setting.mode = mode;
    setting.flag = 1;
    return setting;
}

/**
 * @brief   parse reboot POST request
 * @return  value of "reboot" key, return -1 when fail, otherwise return the value of reboot
*/
int8_t JsonParser::parseReboot(JsonVariant &json)
{
    if(!json.containsKey("reboot"))
    {
        return -1;
    }
    
    auto reboot = json["reboot"].as<int8_t>();
    return reboot;
}

/**
 * @brief   parse factory_reset POST request
 * @return  value of "factory_reset" key, return -1 when fail, otherwise return the value of factory_reset
*/
int8_t JsonParser::parseFactoryReset(JsonVariant &json)
{
    if(!json.containsKey("factory_reset"))
    {
        return -1;
    }
    
    auto fReset = json["factory_reset"].as<int8_t>();
    return fReset;
}