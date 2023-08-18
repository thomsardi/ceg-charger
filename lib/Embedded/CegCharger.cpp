
#include <CegCharger.h>

CegCharger::CegCharger(int controllerAddress)
{
    _loopback = false;
    _controllerAddress = controllerAddress;
    _commandList.setStorage(_requestCommand);
    _canMessage.setStorage(_canMessageStorage);
    _groupData.setStorage(_cegData.grpData);
    _moduleData.setStorage(_cegData.mdlData);
    // fillStack();
}

void CegCharger::fillStack()
{
    _cegData.systemData.systemVoltage = 460.15;
    _cegData.systemData.totalSystemCurrent = 15.24;
    _cegData.systemData.connectedModule = 32;

    for (size_t i = 0; i < _groupData.max_size(); i++)
    {
        CegData::GroupData data;
        data.number = i;
        data.connectedModule = i;
        data.groupVoltage = i*100;
        data.totalGroupCurrent = i*10;
        _groupData.push_back(data);
    }

    for (size_t i = 0; i < _moduleData.max_size(); i++)
    {
        CegData::ModuleData data;
        data.connectedGroup = i;
        data.number = i;
        data.moduleVoltage = (i*200);
        data.moduleCurrent = (i*20);
        data.moduleState.state0.val = i + 123;
        data.moduleState.state1.val = i + 124;
        data.moduleState.state2.val = i + 125;
        _moduleData.push_back(data);
    }        
}

void CegCharger::printStack()
{
    Serial.println("===== Stack Info =====");
    Serial.println("System Voltage : " + String(_cegData.systemData.systemVoltage));
    Serial.println("Total System Current : " + String(_cegData.systemData.totalSystemCurrent));
    Serial.println("Connected Module : " + String(_cegData.systemData.connectedModule));

    for (size_t i = 0; i < _groupData.size(); i++)
    {
        Serial.println("Group Number : " + String(_groupData.at(i).number));
        Serial.println("Group Voltage : " + String(_groupData.at(i).groupVoltage));
        Serial.println("Total Group Current : " + String(_groupData.at(i).totalGroupCurrent));
        Serial.println("Connected Module : " + String(_groupData.at(i).connectedModule));
    }

    for (size_t i = 0; i < _moduleData.size(); i++)
    {
        Serial.println("Connected to Group : " + String(_moduleData.at(i).connectedGroup));
        Serial.println("Module Number : " + String(_moduleData.at(i).number));
        Serial.println("Module Voltage : " + String(_moduleData.at(i).moduleVoltage));
        Serial.println("Module Current : " + String(_moduleData.at(i).moduleCurrent));
        Serial.println("Module Temperature : " + String(_moduleData.at(i).temperature));
        Serial.println("State 0 : " + String(_moduleData.at(i).moduleState.state0.val));
        Serial.println("State 1 : " + String(_moduleData.at(i).moduleState.state1.val));
        Serial.println("State 2 : " + String(_moduleData.at(i).moduleState.state2.val));
    }    
    Serial.println("=========");   

}

int CegCharger::run()
{
    // printStack();
    int status = 0;
    // Serial.println(_commandList.size());
    if (_canMessage.size() > 0)
    {
        CanMessage canmsg = _canMessage.at(0);
        if (canmsg.extended)
        {
            Serial.println("Send CAN");
            beginExtendedPacket(canmsg.frameId.id, canmsg.dlc, canmsg.rtr);
            write(canmsg.data, canmsg.dlc);
            status = endPacket();
        }
        _canMessage.remove(0);
    }
    return status;
}

void CegCharger::readSystemVoltageCurrent(int deviceNumber, int destinationAddress)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Read_System_Output_Voltage_Current_Information;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.dlc = 8;
    beginExtendedPacket(msg.frameId.id, msg.dlc);
    write(msg.data, msg.dlc);
    endPacket();   
}

void CegCharger::readSystemNumberInformation(int deviceNumber, int destinationAddress)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Read_System_Number_Information;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.dlc = 8;
    beginExtendedPacket(msg.frameId.id, msg.dlc);
    write(msg.data, msg.dlc);
    endPacket();   
}

void CegCharger::readModuleVoltageCurrent(int destinationAddress)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = CEG_CHARGER::DeviceNumber::Single_Module;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Read_Module_Output_Voltage_Current_Information;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.dlc = 8;
    beginExtendedPacket(msg.frameId.id, msg.dlc);
    write(msg.data, msg.dlc);
    endPacket();   
}

void CegCharger::readModuleExtraInformation(int destinationAddress)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = CEG_CHARGER::DeviceNumber::Single_Module;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Read_Module_Number_Temperature_State_Information;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.dlc = 8;
    beginExtendedPacket(msg.frameId.id, msg.dlc);
    write(msg.data, msg.dlc);
    endPacket();   
}

void CegCharger::setWalkIn(int deviceNumber, int destinationAddress, uint8_t enable, uint16_t value)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Set_Walk_in;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.extended = true;
    msg.rtr = false;
    msg.dlc = 8;
    msg.data[0] = enable;
    msg.data[6] = value >> 8;
    msg.data[7] = value & 0xff;
    putToQueue(msg);
    // beginExtendedPacket(msg.frameId.id, msg.dlc);
    // write(msg.data, msg.dlc);
    // endPacket();   
}

void CegCharger::setBlink(int deviceNumber, int destinationAddress, uint8_t blink)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Set_Blink;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.extended = true;
    msg.rtr = false;
    msg.dlc = 8;
    msg.data[0] = blink;
    putToQueue(msg);
    // beginExtendedPacket(msg.frameId.id, msg.dlc);
    // write(msg.data, msg.dlc);
    // endPacket();   
}

void CegCharger::setOnOff(int deviceNumber, int destinationAddress, uint8_t off)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Set_On_Off;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.extended = true;
    msg.rtr = false;
    msg.dlc = 8;
    msg.data[0] = off;
    putToQueue(msg);
    // beginExtendedPacket(msg.frameId.id, msg.dlc);
    // write(msg.data, msg.dlc);
    // endPacket();   
}

void CegCharger::setSystemVoltageCurrent(int deviceNumber, int destinationAddress, uint32_t voltage, uint32_t current)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Set_System_Output_Voltage_Current;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.extended = true;
    msg.rtr = false;
    msg.dlc = 8;
    msg.data[0] = voltage >> 24;
    msg.data[1] = voltage >> 16;
    msg.data[2] = voltage >> 8;
    msg.data[3] = voltage & 0xff;
    msg.data[4] = current >> 24;
    msg.data[5] = current >> 16;
    msg.data[6] = current >> 8;
    msg.data[7] = current & 0xff;
    putToQueue(msg);
    // beginExtendedPacket(msg.frameId.id, msg.dlc);
    // write(msg.data, msg.dlc);
    // endPacket();   
}

void CegCharger::setModuleVoltageCurrent(int deviceNumber, int destinationAddress, uint32_t voltage, uint32_t current)
{
    CanMessage msg;
    msg.frameId.frameField.errorCode = CEG_CHARGER::ErrorType::No_Error;
    msg.frameId.frameField.deviceNumber = deviceNumber;
    msg.frameId.frameField.commandNumber = CEG_CHARGER::CommandNumber::Set_Module_Output_Voltage_Current;
    msg.frameId.frameField.destinationAddress = destinationAddress;
    msg.frameId.frameField.sourceAddress = _controllerAddress; 
    msg.extended = true;
    msg.rtr = false;
    msg.dlc = 8;
    msg.data[0] = voltage >> 24;
    msg.data[1] = voltage >> 16;
    msg.data[2] = voltage >> 8;
    msg.data[3] = voltage & 0xff;
    msg.data[4] = current >> 24;
    msg.data[5] = current >> 16;
    msg.data[6] = current >> 8;
    msg.data[7] = current & 0xff;
    putToQueue(msg);
    // beginExtendedPacket(msg.frameId.id, msg.dlc);
    // write(msg.data, msg.dlc);
    // endPacket();   
}

void CegCharger::insertGroupData(const CegData::GroupData &grpData, int commandNumber)
{
    for (size_t i = 0; i < _groupData.size(); i++)
    {
        if (_groupData.at(i).number == grpData.number)
        {
            switch (commandNumber)
            {
            case CEG_CHARGER::CommandNumber::Read_System_Output_Voltage_Current_Information:
                _groupData[i].groupVoltage = grpData.groupVoltage;
                _groupData[i].totalGroupCurrent = grpData.totalGroupCurrent;
                _groupData[i].counter++;
                // Serial.println("mod : " + String(grpData.connectedModule));
                return;
                break;
            case CEG_CHARGER::CommandNumber::Read_System_Number_Information:
                _groupData[i].connectedModule = grpData.connectedModule;
                _groupData[i].counter++;
                return;
                break;
            default:
                break;
            }
        }
    }
    _groupData.push_back(grpData);
    
}

void CegCharger::insertModuleData(const CegData::ModuleData &mdlData, int commandNumber)
{
    for (size_t i = 0; i < _moduleData.size(); i++)
    {
        if (_moduleData.at(i).number == mdlData.number)
        {
            switch (commandNumber)
            {
            case CEG_CHARGER::CommandNumber::Read_Module_Output_Voltage_Current_Information:
                _moduleData[i].moduleVoltage = mdlData.moduleVoltage;
                _moduleData[i].moduleCurrent = mdlData.moduleCurrent;
                _moduleData[i].counter++;
                return;
                break;
            case CEG_CHARGER::CommandNumber::Read_Module_Number_Temperature_State_Information:
                _moduleData[i].connectedGroup = mdlData.connectedGroup;
                _moduleData[i].temperature = mdlData.temperature;
                _moduleData[i].moduleState.state2.val = mdlData.moduleState.state2.val;
                _moduleData[i].moduleState.state1.val = mdlData.moduleState.state1.val;
                _moduleData[i].moduleState.state0.val = mdlData.moduleState.state0.val;
                _moduleData[i].counter++;
                return;
                break;
            default:
                break;
            }
        }
    }
    _moduleData.push_back(mdlData);
}

int CegCharger::putToQueue(const CanMessage &canMessage)
{
    int status = 0;
    CanMessage canmsg;
    canmsg.frameId.id = canMessage.frameId.id;
    canmsg.extended = canMessage.extended;
    canmsg.rtr = canMessage.rtr;
    canmsg.dlc = canMessage.dlc;
    for (size_t i = 0; i < canmsg.dlc; i++)
    {
        canmsg.data[i] = canMessage.data[i];
    }
    
    if (_canMessage.size() < 32)
    {
        _canMessage.push_back(canmsg);
        status = 1;
    }
    return status;
}

int CegCharger::isSendQueueEmpty()
{
    int queueSize = 0;
    if(_canMessage.size() <= 0)
    {
        queueSize = 1;
    }
    return queueSize;
}

int CegCharger::getSendQueueSize()
{
    return _canMessage.size();
}


int CegCharger::processPacket(const CanMessage &canMessage)
{
    int status = 0;
    if (canMessage.frameId.frameField.errorCode == 0x00)
    {
        switch (canMessage.frameId.frameField.commandNumber)
        {
        case CEG_CHARGER::CommandNumber::Read_System_Output_Voltage_Current_Information :
            if (canMessage.frameId.frameField.deviceNumber == CEG_CHARGER::DeviceNumber::Single_Module)
            {
                FloatRepresent f;
                f.buff[0] = canMessage.data[3];
                f.buff[1] = canMessage.data[2];
                f.buff[2] = canMessage.data[1];
                f.buff[3] = canMessage.data[0];
                _cegData.systemData.systemVoltage = f.floatVal;
                f.buff[0] = canMessage.data[7];
                f.buff[1] = canMessage.data[6];
                f.buff[2] = canMessage.data[5];
                f.buff[3] = canMessage.data[4];
                _cegData.systemData.totalSystemCurrent = f.floatVal;
                _cegData.systemData.counter++;
                status = 1;               
            }
            else
            {
                CegData::GroupData grpData;
                grpData.number = canMessage.frameId.frameField.sourceAddress;
                FloatRepresent f;
                f.buff[0] = canMessage.data[3];
                f.buff[1] = canMessage.data[2];
                f.buff[2] = canMessage.data[1];
                f.buff[3] = canMessage.data[0];
                grpData.groupVoltage = f.floatVal;
                f.buff[0] = canMessage.data[7];
                f.buff[1] = canMessage.data[6];
                f.buff[2] = canMessage.data[5];
                f.buff[3] = canMessage.data[4];
                grpData.totalGroupCurrent = f.floatVal;
                insertGroupData(grpData, CEG_CHARGER::CommandNumber::Read_System_Output_Voltage_Current_Information);
                status = 1;
            }
            break;
        case CEG_CHARGER::CommandNumber::Read_System_Number_Information :
            if (canMessage.frameId.frameField.deviceNumber == CEG_CHARGER::DeviceNumber::Single_Module)
            {
                _cegData.systemData.counter++;
                _cegData.systemData.connectedModule = canMessage.data[2];    
                status = 1;        
            }
            else
            {
                CegData::GroupData grpData;
                grpData.number = canMessage.frameId.frameField.sourceAddress;
                grpData.connectedModule = canMessage.data[2];
                insertGroupData(grpData, CEG_CHARGER::CommandNumber::Read_System_Number_Information);
                status = 1;
            }
            break;
        case CEG_CHARGER::CommandNumber::Read_Module_Output_Voltage_Current_Information :
            if (canMessage.frameId.frameField.deviceNumber == CEG_CHARGER::DeviceNumber::Single_Module)
            {
                CegData::ModuleData mdlData;
                mdlData.number = canMessage.frameId.frameField.sourceAddress;
                FloatRepresent f;
                f.buff[0] = canMessage.data[3];
                f.buff[1] = canMessage.data[2];
                f.buff[2] = canMessage.data[1];
                f.buff[3] = canMessage.data[0];
                mdlData.moduleVoltage = f.floatVal;
                f.buff[0] = canMessage.data[7];
                f.buff[1] = canMessage.data[6];
                f.buff[2] = canMessage.data[5];
                f.buff[3] = canMessage.data[4];
                mdlData.moduleCurrent = f.floatVal;
                insertModuleData(mdlData, CEG_CHARGER::CommandNumber::Read_Module_Output_Voltage_Current_Information);    
                status = 1;     
            }
            break;
        case CEG_CHARGER::CommandNumber::Read_Module_Number_Temperature_State_Information :
            if (canMessage.frameId.frameField.deviceNumber == CEG_CHARGER::DeviceNumber::Single_Module)
            {
                CegData::ModuleData mdlData;
                mdlData.number = canMessage.frameId.frameField.sourceAddress;
                mdlData.connectedGroup = canMessage.data[2];
                mdlData.temperature = canMessage.data[4];
                mdlData.moduleState.state2.val = canMessage.data[5];
                mdlData.moduleState.state1.val = canMessage.data[6];
                mdlData.moduleState.state0.val = canMessage.data[7];
                insertModuleData(mdlData, CEG_CHARGER::CommandNumber::Read_Module_Number_Temperature_State_Information); 
                status = 1;        
            }
            break;
        default:
            break;
        }
    }
    return status;
}

String CegCharger::getDataJson()
{
    String output;
    DynamicJsonDocument doc(3072); doc;
    doc["counter"] = _cegData.systemData.counter;
    doc["system_voltage"] = _cegData.systemData.systemVoltage;
    doc["system_current"] = _cegData.systemData.totalSystemCurrent;
    doc["connected_module"] = _cegData.systemData.connectedModule;

    JsonArray group_data = doc.createNestedArray("group_data");
    for (size_t i = 0; i < _groupData.size(); i++)
    {
        JsonObject group_data_0 = group_data.createNestedObject();
        group_data_0["counter"] = _groupData.at(i).counter;
        group_data_0["group_number"] = _groupData.at(i).number;
        group_data_0["group_voltage"] = _groupData.at(i).groupVoltage;
        group_data_0["group_current"] = _groupData.at(i).totalGroupCurrent;
        group_data_0["connected_module"] = _groupData.at(i).connectedModule;
        JsonArray group_data_0_module_data = group_data_0.createNestedArray("module_data");
        for (size_t j = 0; j < _moduleData.size(); j++)
        {
            if (_moduleData.at(j).connectedGroup == _groupData.at(i).number)
            {
                JsonObject group_data_0_module_data_0 = group_data_0_module_data.createNestedObject();
                group_data_0_module_data_0["counter"] = _moduleData.at(j).counter;
                group_data_0_module_data_0["module_number"] = _moduleData.at(j).number;
                group_data_0_module_data_0["module_voltage"] = _moduleData.at(j).moduleVoltage;
                group_data_0_module_data_0["module_current"] = _moduleData.at(j).moduleCurrent;
                group_data_0_module_data_0["module_temperature"] = _moduleData.at(j).temperature;
                group_data_0_module_data_0["state_0"] = _moduleData.at(j).moduleState.state0.val;
                group_data_0_module_data_0["state_1"] = _moduleData.at(j).moduleState.state1.val;
                group_data_0_module_data_0["state_2"] = _moduleData.at(j).moduleState.state2.val;
            }            
        }
        
    }
    serializeJson(doc, output);
    return output;
}

int CegCharger::parseSyncSystemVoltageCurrentJson(JsonVariant &json)
{ 
    if(!json.containsKey("voltage"))
    {
        return -1;
    }

    if(!json.containsKey("total_current"))
    {
        return -1;
    }
    
    auto voltage = json["voltage"].as<int>();
    auto totalCurrent = json["total_current"].as<int>();
    setSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, voltage, totalCurrent);
    return 1;
}

int CegCharger::parseSyncGroupVoltageCurrentJson(JsonVariant &json)
{ 
    if(!json.containsKey("group"))
    {
        return -1;
    }

    if(!json.containsKey("voltage"))
    {
        return -1;
    }

    if(!json.containsKey("total_current"))
    {
        return -1;
    }
    
    auto group = json["group"].as<int>();
    auto voltage = json["voltage"].as<int>();
    auto totalCurrent = json["total_current"].as<int>();
    setSystemVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, group, voltage, totalCurrent);
    return 1;
}

int CegCharger::parseAllGroupVoltageCurrentJson(JsonVariant &json)
{ 
    if(!json.containsKey("voltage"))
    {
        return -1;
    }

    if(!json.containsKey("current"))
    {
        return -1;
    }
    
    auto voltage = json["voltage"].as<int>();
    auto current = json["current"].as<int>();
    setModuleVoltageCurrent(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, voltage, current);
    return 1;
}

int CegCharger::parseSingleGroupVoltageCurrentJson(JsonVariant &json)
{ 
    if(!json.containsKey("group"))
    {
        return -1;
    }

    if(!json.containsKey("voltage"))
    {
        return -1;
    }

    if(!json.containsKey("current"))
    {
        return -1;
    }
    
    auto group = json["group"].as<int>();
    auto voltage = json["voltage"].as<int>();
    auto current = json["current"].as<int>();
    setModuleVoltageCurrent(CEG_CHARGER::DeviceNumber::Group_Module, group, voltage, current);
    return 1;
}

int CegCharger::parseSetAllModuleJson(JsonVariant &json)
{ 
    if(!json.containsKey("disable"))
    {
        return -1;
    }
    
    auto disable = json["disable"].as<int>();
    setOnOff(CEG_CHARGER::DeviceNumber::Single_Module, 0x3f, disable);
    return 1;
}

int CegCharger::parseSetSingleGroupJson(JsonVariant &json)
{ 
    if(!json.containsKey("group"))
    {
        return -1;
    }
    
    if(!json.containsKey("disable"))
    {
        return -1;
    }
    
    auto group = json["group"].as<int>();
    auto disable = json["disable"].as<int>();
    setOnOff(CEG_CHARGER::DeviceNumber::Group_Module, group, disable);
    return 1;
}

int CegCharger::parseSetSingleModuleJson(JsonVariant &json)
{ 
    if(!json.containsKey("module"))
    {
        return -1;
    }
    if(!json.containsKey("disable"))
    {
        return -1;
    }
    auto module = json["module"].as<int>();
    auto disable = json["disable"].as<int>();
    setOnOff(CEG_CHARGER::DeviceNumber::Single_Module, module, disable);
    return 1;
}

void CegCharger::cleanUp()
{
    for (size_t i = 0; i < _moduleData.size(); i++)
    {
        if (_moduleData.at(i).prevCounter != _moduleData.at(i).counter)
        {
            Serial.println("Update module counter");
            _moduleData.at(i).prevCounter = _moduleData.at(i).counter;
        }
        else
        {
            Serial.println("remove module");
            _moduleData.remove(i);
        }
    }

    for (size_t i = 0; i < _groupData.size(); i++)
    {
        if (_groupData.at(i).prevCounter != _groupData.at(i).counter)
        {
            Serial.println("Update group counter");
            _groupData.at(i).prevCounter = _groupData.at(i).counter;
        }
        else
        {
            Serial.println("remove group");
            _groupData.remove(i);
        }
    } 

    if (_cegData.systemData.prevCounter != _cegData.systemData.counter)
    {
        _cegData.systemData.prevCounter = _cegData.systemData.counter;
    }
    else
    {
        _cegData.systemData.connectedModule = 0;
        _cegData.systemData.systemVoltage = 0;
        _cegData.systemData.totalSystemCurrent = 0;
    }

}

CegData* CegCharger::getStackAddress()
{
    return &_cegData;
}

/**
 * [int] filterExtended
 * @param id lowest id range
 * @param mask mask value
 * @return
 * 
 * To determine the mask value :
 * 1. Write the list of id that need to pass the filter
 * 2. XNOR bitwise each of ID
 * 3. Bitwise Not the result of the step 2
 * 
 * Example 1:
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ID 2 : 0b1011000000000000011100000000
 * XNOR : 0b1010111111111111111111111111
 * MASK : 0b0101000000000000000000000000
 * 
 * This will get id range from 0b1110000000000000011100000000 - 0b1110001111111111111111111111
 * Example 2 :
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ...
 * ID X : 0b1110001111111111111111111111
 * XNOR : 0b1110110000000000000000000000
 * MASK : 0b1111000000000000011100000000
*/

int CegCharger::filterExtended(long id, long mask)
{
    // uint32_t temp;
    // temp = mask;
    // id &= 0x1FFFFFFF;
    // temp = ~(temp & 0x1FFFFFFF);
    Serial.print("ID Filter : ");
    Serial.println(id, BIN);

    Serial.print("Mask Filter : ");
    Serial.println(mask, BIN);

    // mask = temp;

    modifyRegister(REG_MOD, 0x17, 0x01); // reset

    writeRegister(REG_ACRn(0), id >> 21);
    writeRegister(REG_ACRn(1), id >> 13);
    writeRegister(REG_ACRn(2), id >> 5);
    writeRegister(REG_ACRn(3), id << 5);

    writeRegister(REG_AMRn(0), mask >> 21);
    writeRegister(REG_AMRn(1), mask >> 13);
    writeRegister(REG_AMRn(2), mask >> 5);
    writeRegister(REG_AMRn(3), (mask << 5) | 0x1f);

    modifyRegister(REG_MOD, 0x17, 0x00); // normal

    return 1;
}

void CegCharger::modifyRegister(uint8_t address, uint8_t mask, uint8_t value)
{
  volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

  *reg = (*reg & ~mask) | value;
}

void CegCharger::writeRegister(uint8_t address, uint8_t value)
{
  volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

  *reg = value;
}

uint8_t CegCharger::readRegister(uint8_t address)
{
  volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

  return *reg;
}

int CegCharger::endPacket()
{
    if (!CANControllerClass::endPacket()) {
    return 0;
  }

  const int timeoutValue = 500;
  uint8_t currenTimeout = 0;

  // wait for TX buffer to free
  while ((readRegister(REG_SR) & 0x04) != 0x04 && (currenTimeout < timeoutValue)) {
    yield();
    currenTimeout++;
  }

  if (currenTimeout == timeoutValue)
  {
    return -1;
  }
  
  currenTimeout = 0;

  int dataReg;

  if (_txExtended) {
    writeRegister(REG_EFF, 0x80 | (_txRtr ? 0x40 : 0x00) | (0x0f & _txLength));
    writeRegister(REG_EFF + 1, _txId >> 21);
    writeRegister(REG_EFF + 2, _txId >> 13);
    writeRegister(REG_EFF + 3, _txId >> 5);
    writeRegister(REG_EFF + 4, _txId << 3);

    dataReg = REG_EFF + 5;
  } else {
    writeRegister(REG_SFF, (_txRtr ? 0x40 : 0x00) | (0x0f & _txLength));
    writeRegister(REG_SFF + 1, _txId >> 3);
    writeRegister(REG_SFF + 2, _txId << 5);

    dataReg = REG_SFF + 3;
  }

  for (int i = 0; i < _txLength; i++) {
    writeRegister(dataReg + i, _txData[i]);
  }

  if ( _loopback) {
    // self reception request
    modifyRegister(REG_CMR, 0x1f, 0x10);
  } else {
    // transmit request
    modifyRegister(REG_CMR, 0x1f, 0x01);
  }

  // wait for TX complete
  while ((readRegister(REG_SR) & 0x08) != 0x08 && (currenTimeout < timeoutValue)) {
    if (readRegister(REG_ECC) == 0xd9) {
      modifyRegister(REG_CMR, 0x1f, 0x02); // error, abort
      return 0;
    }
    yield();
    currenTimeout++;
  }

  if (currenTimeout == timeoutValue)
  {
    return -2;
  }

  return 1;
}

int CegCharger::loopback()
{
  _loopback = true;

  modifyRegister(REG_MOD, 0x17, 0x01); // reset
  modifyRegister(REG_MOD, 0x17, 0x04); // self test mode

  return 1;
}