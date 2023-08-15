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

CegCharger cegCharger(0xF0);

TaskHandle_t canSenderTaskHandle;
QueueHandle_t canSenderTaskQueue = xQueueCreate(64, sizeof(CanMessage));

const char *ssid = "RnD_Sundaya";
const char *password = "sundaya22";

AsyncWebServer server(80);

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

  if(canmsg.frameId.frameField.destinationAddress != controllerAddress)
  {
    return;
  }

  if (canmsg.frameId.frameField.commandNumber < 1 && canmsg.frameId.frameField.commandNumber > 28) 
  {
    return;
  }
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
          cegCharger.printStack();
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

        for (size_t i = 0; i < 16; i++)
        {
          cegCharger.readModuleVoltageCurrent(i);
          cegCharger.readModuleExtraInformation(i);
        }
      }
    }
   
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  if (!cegCharger.begin(125E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  cegCharger.onReceive(onReceive);

  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.mode(WIFI_MODE_NULL);
  WiFi.mode(WIFI_STA);

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(500);
  }
  
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
  // Serial.println("Array Size : " + String(arrSize));
  

  // if (cegCharger.putToQueue(canmsg))
  // {
  //   Serial.println("Success put to queue");
  // }

  // CAN.beginExtendedPacket(0xabcdef);
  // CAN.write('w');
  // CAN.write('o');
  // CAN.write('r');
  // CAN.write('l');
  // CAN.write('d');
  // CAN.endPacket();
  delay(10);
}