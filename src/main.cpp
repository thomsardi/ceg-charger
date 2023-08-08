#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN.h>
#include <CAN_config.h>
#include <CegCharger.h>
#include <Vector.h>

CegCharger cegCharger;

TaskHandle_t canSenderTaskHandle;
QueueHandle_t canSenderTaskQueue = xQueueCreate(64, sizeof(CanMessage));

int count = 0;

// put function declarations here:
int myFunction(int, int);

void onReceive(int _packetSize) 
{
  // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  // xSemaphoreGiveFromISR(xCANReceived, &xHigherPriorityTaskWoken);
  // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  // packetSize = _packetSize;

  CanMessage canmsg;
  canmsg.frameId.id = cegCharger.packetId();
  canmsg.rtr = cegCharger.packetRtr();
  canmsg.extended = cegCharger.packetExtended();
  canmsg.dlc = _packetSize;
  // Serial.println("Length = " + String(canmsg.dlc));
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
  while (1)
  {
    Serial.println("CAN Task");
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  if (!cegCharger.begin(250E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  cegCharger.onReceive(onReceive);

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
  cegCharger.run();
  CanMessage canmsg;
  canmsg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
  canmsg.frameId.frameField.deviceNumber = CEG_CHARGER::DeviceNumber::Single_Module;
  canmsg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Read_Module_Output_Voltage_Current_Information;
  canmsg.frameId.frameField.destinationAddress = 0x00;
  canmsg.frameId.frameField.sourceAddress = 0xf0;
  canmsg.extended = true;
  canmsg.rtr = false;
  canmsg.dlc = 8;
  canmsg.data[0] = 0x00;
  canmsg.data[1] = 0x01;
  canmsg.data[2] = 0x02;
  canmsg.data[3] = 0x03;
  canmsg.data[4] = 0x04;
  canmsg.data[5] = 0x05;
  canmsg.data[6] = 0x06;
  canmsg.data[7] = 0x07;

  CanMessage rxmsg;

  if(xQueueReceive(canSenderTaskQueue,&rxmsg, 1000)==pdTRUE)
  {
    Serial.println("Count : " + String(count));
    Serial.print("Frame id : ");
    Serial.println(rxmsg.frameId.id, HEX);
    Serial.println("Extended : " + String(rxmsg.extended));
    Serial.println("Remote : " + String(rxmsg.rtr));
    Serial.println("Length : " + String(rxmsg.dlc));
    Serial.print("Data : ");
    for (size_t i = 0; i < rxmsg.dlc; i++)
    {
      Serial.print(rxmsg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    count++;
  }

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