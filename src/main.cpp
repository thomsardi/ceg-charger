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

int controllerAddress = 0xF0;
int internalLed = 2;
unsigned long lastReconnectMillis;
unsigned long lastTime;
int interval = 5000;
int reconnectInterval = 3000;

CegCharger cegCharger(0xF0);

TaskHandle_t canSenderTaskHandle;
QueueHandle_t canSenderTaskQueue = xQueueCreate(64, sizeof(CanMessage));

const char *ssid = "mikrotik";
const char *password = "mikrotik";

AsyncWebServer server(80);
IPAddress local_ip(192, 168, 2, 162);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// put function declarations here:
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS 1: ");
    Serial.println(WiFi.dnsIP(0));
    Serial.print("DNS 2: ");
    Serial.println(WiFi.dnsIP(1));
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
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
    // WiFi.begin(ssid, password);
    WiFi.begin(ssid, password);
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
        cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);
        cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f);

        for (size_t i = 0; i < 8; i++)
        {
          cegCharger.readSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, i);
          cegCharger.readSystemNumberInformation(CEG_CHARGER::DeviceNumber::Group_Module, i);
        }

        for (size_t i = 0; i < 32; i++)
        {
          cegCharger.readModuleVoltageCurrent(i);
          cegCharger.readModuleExtraInformation(i);
        }
      }
    }

    if (millis() - lastTime > interval)
    {
      cegCharger.cleanUp();
      cegCharger.printStack();
      lastTime = millis();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(internalLed, OUTPUT);
  Serial.begin(115200);
  while (!Serial);

  if (!cegCharger.begin(125E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
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
  WiFi.mode(WIFI_STA);

  // if (!WiFi.config(local_ip, gateway, subnet))
  // {
  //     Serial.println("STA Failed to configure");
  // }

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  server.on("/get-data", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      Serial.println("get-data");
      Serial.println(cegCharger.getDataJson());
      request->send(200, "application/json", cegCharger.getDataJson()); });

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
  if ((WiFi.status() != WL_CONNECTED) && (millis() - lastReconnectMillis >= reconnectInterval)) {
    digitalWrite(internalLed, LOW);
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    lastReconnectMillis = millis();
  }
  delay(100);
}