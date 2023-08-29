#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
// #include <ESP32CAN.h>
#include <CAN.h>
// #include <CAN_config.h>
#include <CegCharger.h>
#include <Vector.h>
#include <Preferences.h>
#include <DataDef.h>
#include <JsonParser.h>
#include <OneButton.h>

#define DEBUG

int controllerAddress = 0xF0;
int internalLed = 2;
unsigned long lastReconnectMillis;
unsigned long lastTime;
unsigned long lastPrintTime;
int printInterval = 100;
int interval = 5000;
int reconnectInterval = 3000;


CegCharger cegCharger(0xF0);
OneButton button(32);

TaskHandle_t canSenderTaskHandle;
QueueHandle_t canSenderTaskQueue = xQueueCreate(64, sizeof(CanMessage));

Preferences network;
Preferences deviceParameter;

JsonParser jsonParser;

String ipAddress;
String gateways;
String subnets;
IPAddress gateway;
IPAddress subnet;
int mode;
int serverType;

#ifdef DEBUG
  String ssid = "mikrotik";
  String password = "mikrotik";
  IPAddress local_ip(192, 168, 2, 162);
  // IPAddress gateway(192, 168, 2, 1);
  // IPAddress subnet(255, 255, 255, 0);
#else
  const char *ssid = "Ruang_Laminate";
  const char *password = "sundaya22";
  // IPAddress local_ip(192, 168, 2, 162);
  // IPAddress gateway(192, 168, 2, 1);
  // IPAddress subnet(255, 255, 255, 0);
#endif

AsyncWebServer server(80);


// put function declarations here:
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ipAddress = WiFi.localIP().toString();
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    subnets = WiFi.subnetMask().toString();
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    gateways = WiFi.gatewayIP().toString();
    Serial.print("DNS 1: ");
    Serial.println(WiFi.dnsIP(0));
    Serial.print("DNS 2: ");
    Serial.println(WiFi.dnsIP(1));
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
    digitalWrite(internalLed, HIGH);
}

void WiFiAPConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("AP Connected");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP().toString());
  Serial.print("Subnet Mask: ");
  Serial.println(subnets);
  Serial.print("Gateway IP: ");
  Serial.println(gateways);
  Serial.print("Hostname: ");
  Serial.println(WiFi.softAPgetHostname());
  digitalWrite(internalLed, HIGH);
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Wifi Connected");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
    digitalWrite(internalLed, LOW);
    Serial.println("Disconnected from WiFi access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    Serial.println("Trying to Reconnect");
    WiFi.begin(ssid.c_str(), password.c_str());
}

void onReceive(int _packetSize) 
{
  CanMessage canmsg;
  canmsg.frameId.id = cegCharger.packetId();
  // Serial.println("Received");
  // Serial.println(canmsg.frameId.frameField.destinationAddress);
  if(canmsg.frameId.frameField.destinationAddress != controllerAddress)
  {
    // Serial.println("Invalid controller address");
    return;
  }

  if (canmsg.frameId.frameField.commandNumber < 1 || canmsg.frameId.frameField.commandNumber > 28) 
  {
    // Serial.println("Invalid command number");
    return;
  }

  // Serial.println("Process");

  canmsg.rtr = cegCharger.packetRtr();
  canmsg.extended = cegCharger.packetExtended();
  canmsg.dlc = _packetSize;

  int i = 0;
  while (cegCharger.available())
  {
    canmsg.data[i] = cegCharger.read();
    i++;
  }
  xQueueSend(canSenderTaskQueue, &canmsg, 0);
}

void canTask(void *parameter)
{
  int groupNumber = 0;
  int moduleNumber = 128;
  Serial.println("CAN Task");
  while (1)
  {
    CanMessage rxmsg;
    // cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
    // cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, 0x01);
    // cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
    // cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Group_Module, 0x01);
    // cegCharger.readModuleVoltageCurrent(0x00);
    // cegCharger.readModuleExtraInformation(0x01);
    // cegCharger.setWalkIn(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
    // cegCharger.setWalkIn(CEG_CHARGER::DeviceNumber::Single_Module, 0x00, false);
    // cegCharger.setBlink(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, true);
    // cegCharger.setBlink(CEG_CHARGER::DeviceNumber::Single_Module, 0x00, false);
    // cegCharger.setOnOff(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
    // cegCharger.setOnOff(CEG_CHARGER::DeviceNumber::Single_Module, 0x01);
    // cegCharger.setOnOff(CEG_CHARGER::DeviceNumber::Group_Module, 0x02);
    // cegCharger.setSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, 300000, 10000);
    // cegCharger.setSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, 0x02, 200000, 5000);
    // cegCharger.setModuleVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, 300000, 10000);
    // cegCharger.setModuleVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, 0x02, 200000, 5000);
    // cegCharger.printStack();
    if (!cegCharger.run())
    {
      if(xQueueReceive(canSenderTaskQueue, &rxmsg, 100)==pdTRUE)
      {
        if (cegCharger.processPacket(rxmsg))
        {
          // cegCharger.printStack();
        }
      }
      else
      {
        // Serial.println("No CAN Data");
        cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
        cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
        cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, groupNumber);
        cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Group_Module, groupNumber);
        cegCharger.readModuleVoltageCurrent(moduleNumber);
        cegCharger.readModuleExtraInformation(moduleNumber);
        
        // for (int i = 0; i < 60; i++) //send command to each group
        // {
        //   Serial.println("Send Read System");
        //   cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, i);
        //   Serial.println("Finish read sys voltage current");
        //   cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Group_Module, i);
        //   Serial.println("Finish read sys extra information");
        // }

        // for (int i = 128; i >= 0; i--) //send command to each module
        // {
        //   Serial.println(i);
        //   Serial.println("Send Read Module");
        //   cegCharger.readModuleVoltageCurrent(i);
        //   cegCharger.readModuleExtraInformation(i);
        // }

        for (size_t i = 0; i < cegCharger.getModuleStackSize(); i++)
        {
          CegData::ModuleData d = cegCharger.getModuleData(i);
          cegCharger.readModuleInputVoltageInformation(d.number);
          cegCharger.readModuleExternalVoltageAvailableCurrent(d.number);
        }
        groupNumber++;
        moduleNumber--;

        if(groupNumber > 60)
        {
          groupNumber = 0;
        }

        if(moduleNumber < 0)
        {
          moduleNumber = 128;
        }

      }
    }

    if (millis() - lastTime > interval)
    {
      cegCharger.cleanUp();
      
      lastTime = millis();
    }
    if (millis() - lastPrintTime > printInterval)
    {
      bool a = digitalRead(internalLed);
      digitalWrite(internalLed, !a);
      cegCharger.printStack();
      lastPrintTime = millis();
    }
    

    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

void setDefaultPreference()
{
  
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String defaultSsid = "ESP32-" + mac;
  network.putString("default_ssid", defaultSsid);
  network.putString("default_pass", "esp32-default");
  network.putString("default_ip", "192.168.1.100");
  network.putString("default_gateway", "192.168.1.1");
  network.putString("default_subnet", "255.255.255.0");
  network.putChar("default_server", Network::Server::STATIC);
  network.putChar("default_mode", Network::MODE::AP);
  network.putChar("set_flag", 1);

  deviceParameter.putULong("default_voltage", 550000);
  deviceParameter.putULong("default_current", 15000);
  deviceParameter.putChar("set_flag", 1);
}

void setUserPreference()
{
  String defaultSsid = WiFi.macAddress();
  network.putString("ssid", "mikrotik");
  network.putString("pass", "mikrotik");
  network.putString("ip", "192.168.2.162");
  network.putString("gateway", "192.168.2.1");
  network.putString("subnet", "255.255.255.0");
  network.putChar("server", Network::Server::DHCP);
  network.putChar("mode", Network::MODE::STATION);

  deviceParameter.putULong("voltage", 440000);
  deviceParameter.putULong("current", 10000);

}

void factoryReset()
{
  network.putChar("set_flag", 1);
  deviceParameter.putChar("set_flag", 1);
  ESP.restart();
}

void reboot()
{
  ESP.restart();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(internalLed, OUTPUT);
  button.attachClick(reboot);
  button.attachLongPressStart(factoryReset);
  // startButton.attachDoubleClick(buttonDoubleClicked);
  button.setDebounceTicks(50);
  Serial.begin(115200);
  while (!Serial);

  if (!cegCharger.begin(125E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  if (network.begin("network"))
  {
    Serial.println("Successfully set network preference");
  }

  if (deviceParameter.begin("device"))
  {
    Serial.println("Successfully set device preference");
  }
  int8_t networkSetFlag = network.getChar("set_flag");
  int8_t deviceSetFlag = deviceParameter.getChar("set_flag");
  if (networkSetFlag == 0)
  {
    Serial.println("Initialize setting parameter");
    setDefaultPreference();
    setUserPreference();
  }
    
  FrameId idFilter;
  idFilter.frameField.errorCode = 0x00;
  idFilter.frameField.deviceNumber = 0x0a;
  idFilter.frameField.commandNumber = 0x00;
  idFilter.frameField.destinationAddress = controllerAddress;
  
  FrameId idMask;
  idMask.frameField.errorCode = 0x0;
  idMask.frameField.deviceNumber = 0x1;
  idMask.frameField.commandNumber = 0x1f;
  idMask.frameField.destinationAddress = 0;
  idMask.frameField.sourceAddress = 0xff;
  
  cegCharger.filterExtended(idFilter.id, idMask.id);
  cegCharger.onReceive(onReceive);

  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.mode(WIFI_MODE_NULL);
  // WiFi.mode(WIFI_STA);

  if (network.getChar("set_flag") == 1)
  {
    ssid = network.getString("default_ssid");
    password = network.getString("default_pass");
    ipAddress = network.getString("default_ip");
    gateways = network.getString("default_gateway");
    subnets = network.getString("default_subnet");
    mode = network.getChar("default_mode");
    serverType = network.getChar("default_server");
    // Serial.println("=====Default=====");
    // Serial.println("SSID : " + ssid);
    // Serial.println("Pass : " + password);
    // Serial.println("Ip : " + ipAddress);
    // Serial.println("Gateway : " + gateways);
    // Serial.println("Subnet : " + subnets);
    switch (mode)
    {
    case Network::MODE::AP :
      // Serial.println("Default AP");
      WiFi.mode(WIFI_AP);
      break;
    case Network::MODE::STATION :
      // Serial.println("Default Station");
      WiFi.mode(WIFI_STA);
      break;
    default:
      break;
    }

    switch (serverType)
    {
    case Network::Server::STATIC :
      // Serial.println("Default Static");
      local_ip.fromString(ipAddress);
      gateway.fromString(gateways);
      subnet.fromString(subnets);

      if (mode == Network::MODE::STATION)
      {
        if (!WiFi.config(local_ip, gateway, subnet))
        {
            Serial.println("STA Failed to configure");
        }
      }
      else
      {
        if (!WiFi.softAPConfig(local_ip, gateway, subnet))
        {
          Serial.println("AP Failed to configure");
        }
      }
      break;

    case Network::Server::DHCP :
      // Serial.println("Default Dynamic");
      break;
    
    default:
      break;
    }
  }
  else
  {
    ssid = network.getString("ssid");
    password = network.getString("pass");
    ipAddress = network.getString("ip");
    gateways = network.getString("gateway");
    subnets = network.getString("subnet");
    mode = network.getChar("mode");
    serverType = network.getChar("server");
    // Serial.println("=====User=====");
    // Serial.println("SSID : " + ssid);
    // Serial.println("Pass : " + password);
    // Serial.println("Ip : " + ipAddress);
    // Serial.println("Gateway : " + gateways);
    // Serial.println("Subnet : " + subnets);
    switch (mode)
    {
    case Network::MODE::AP :
      // Serial.println("User AP");
      WiFi.mode(WIFI_AP);
      break;
    case Network::MODE::STATION :
      // Serial.println("User Station");
      WiFi.mode(WIFI_STA);
      break;
    default:
      break;
    }

    switch (serverType)
    {
    case Network::Server::STATIC :
      local_ip.fromString(ipAddress);
      gateway.fromString(gateways);
      subnet.fromString(subnets);
      if (!WiFi.config(local_ip, gateway, subnet))
      {
        Serial.println("STA Failed to configure");
      }
      break;
    case Network::Server::DHCP :
      // Serial.println("User Dynamic");
      break;
    
    default:
      break;
    }
  }

  // if (!WiFi.config(local_ip, gateway, subnet))
  // {
  //     Serial.println("STA Failed to configure");
  // }

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  if (mode == Network::MODE::STATION)
  {
    WiFi.begin(ssid.c_str(), password.c_str());
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      if (timeout >= 10)
      {
        Serial.println("Failed to connect into " + ssid);
        break;
      }
      Serial.print(".");
      delay(500);
      timeout++;
    }
  }
  else
  {
    WiFi.softAP(ssid,password);
    Serial.println("AP Connected");
    Serial.print("SSID : ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    ipAddress = WiFi.softAPIP().toString();
    Serial.println(WiFi.softAPIP().toString());
    Serial.print("Subnet Mask: ");
    Serial.println(subnets);
    Serial.print("Gateway IP: ");
    Serial.println(gateways);
    Serial.print("Hostname: ");
    Serial.println(WiFi.softAPgetHostname());
    digitalWrite(internalLed, HIGH);
  }
    
  server.on("/get-data", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      Serial.println("get-data");
      Serial.println(cegCharger.getDataJson());
      request->send(200, "application/json", cegCharger.getDataJson()); });

  server.on("/get-network-info", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      // Serial.println("get-data");
      NetworkSetting s;
      switch (mode)
      {
      case Network::MODE::STATION :
        s.ssid = WiFi.SSID();
        s.ip = WiFi.localIP().toString();
        break;
      case Network::MODE::AP :
        s.ssid = WiFi.softAPSSID();
        s.ip = WiFi.softAPIP().toString();
        break;
      default:
        break;
      }
      // Serial.println(jsonParser.getNetworkInfo(s));
      request->send(200, "application/json", jsonParser.getNetworkInfo(s)); });

  server.on("/get-user-network-setting", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      // Serial.println("get-data");
      NetworkSetting s;
      s.ssid = network.getString("ssid");
      s.pass = network.getString("pass");
      s.ip = network.getString("ip");
      s.gateway = network.getString("gateway");
      s.subnet = network.getString("subnet");
      s.mode = network.getChar("mode");
      s.server = network.getChar("server");
      // Serial.println(jsonParser.getNetworkInfo(s));
      request->send(200, "application/json", jsonParser.getUserNetworkSetting(s)); });

  AsyncCallbackJsonWebHandler *setAllSystemVoltageCurrent = new AsyncCallbackJsonWebHandler("/set-sync-system-voltage-current", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSyncSystemVoltageCurrentJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setGroupSystemVoltageCurrent = new AsyncCallbackJsonWebHandler("/set-sync-group-voltage-current", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSyncGroupVoltageCurrentJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setGroupVoltageCurrent = new AsyncCallbackJsonWebHandler("/set-all-group-voltage-current", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseAllGroupVoltageCurrentJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setModuleVoltageCurrent = new AsyncCallbackJsonWebHandler("/set-single-group-voltage-current", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSingleGroupVoltageCurrentJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setAllModule = new AsyncCallbackJsonWebHandler("/set-all-module", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSetAllModuleJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setSingleModule = new AsyncCallbackJsonWebHandler("/set-single-module", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSetSingleModuleJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setSingleGroup = new AsyncCallbackJsonWebHandler("/set-single-group", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int status = cegCharger.parseSetSingleGroupJson(json);
    if (status > 0)
    {
      response.replace(":status:", String(status));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setNetwork = new AsyncCallbackJsonWebHandler("/set-network", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    NetworkSetting setting = jsonParser.parseNetworkSetting(json);
    if (setting.flag > 0)
    {
      network.putString("ssid", setting.ssid);
      network.putString("pass", setting.pass);
      network.putString("ip", setting.ip);
      network.putString("gateway", setting.gateway);
      network.putString("subnet", setting.subnet);
      network.putChar("server", setting.server);
      network.putChar("mode", setting.mode);
      network.putChar("set_flag", 2);
      response.replace(":status:", String(setting.flag));
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setReboot = new AsyncCallbackJsonWebHandler("/set-reboot", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int8_t reboot = jsonParser.parseReboot(json);
    if (reboot >= 0)
    {
      response.replace(":status:", String(reboot));
      request->send(200, "application/json", response);
      if (reboot == 1)
      {
        ESP.restart();
      }
    }
    else
    {
      request->send(400);
    }
  });

  AsyncCallbackJsonWebHandler *setFactoryReset = new AsyncCallbackJsonWebHandler("/factory-reset", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    
    int8_t fReset = jsonParser.parseFactoryReset(json);
    if (fReset >= 0)
    {
      response.replace(":status:", String(fReset));
      request->send(200, "application/json", response);
      if (fReset == 1)
      {
        network.putChar("set_flag", 1);
        deviceParameter.putChar("set_flag", 1);
        ESP.restart();
      }
    }
    else
    {
      request->send(400);
    }
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });

  server.addHandler(setAllSystemVoltageCurrent);
  server.addHandler(setGroupSystemVoltageCurrent);
  server.addHandler(setGroupVoltageCurrent);
  server.addHandler(setModuleVoltageCurrent);
  server.addHandler(setAllModule);
  server.addHandler(setSingleModule);
  server.addHandler(setSingleGroup);
  server.addHandler(setNetwork);
  server.addHandler(setReboot);
  server.addHandler(setFactoryReset);
  server.begin();

  xTaskCreatePinnedToCore(
    canTask,
    "canTask",
    2048,
    NULL,
    tskIDLE_PRIORITY,
    NULL,
    1
  );
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mode == Network::MODE::STATION)
  {
    if ((WiFi.status() != WL_CONNECTED) && (millis() - lastReconnectMillis >= reconnectInterval)) {
      digitalWrite(internalLed, LOW);
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      lastReconnectMillis = millis();
    }
  }
  
  delay(100);
}