
#include <CegCharger.h>

CegCharger::CegCharger(int controllerAddress)
{
    _loopback = false;
    _controllerAddress = controllerAddress;
    _commandList.setStorage(_requestCommand);
    _canMessage.setStorage(_canMessageStorage);
    _moduleData.setStorage(_cegData.moduleData);
}

int CegCharger::run()
{
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
    if (canMessage.frameId.frameField.commandNumber == 0x01 && canMessage.frameId.frameField.errorCode == 0x00)
    {
        switch (canMessage.frameId.frameField.deviceNumber)
        {
        case 0x0a:
            
            break;
        
        default:
            break;
        }
    }
}

int CegCharger::updateFrameId(int msgId)
{
    switch (msgId)
    {
        case MessageIdRequest::Module_On_Off_32:
            _pCode = 0x38;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_On_Off_64:
            _pCode = 0x38;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_32:
            _pCode = 0x2C;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_64:
            _pCode = 0x2C;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;    
        case MessageIdRequest::Module_Voltage_Mode:
            _pCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Voltage:
            _pCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Current:
            _pCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Module_Modify_Group:
            _pCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
        case MessageIdRequest::Query_Single_Module_Info:
            _pCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            _destinationAddr = (_groupNumber << 6) + _subAddress;
            _sourceAddr = (_monitorGroup << 6) + _monitorSubAddress;
            buildFrameId(_pCode, _destinationAddr, _sourceAddr);
            return 1;
            break;
    }
    _frameId = -1;
    return -1;
}

int CegCharger::buildFrameId(int protocolCode, int destinationAddr, int sourceAddr)
{
    _frameId = (protocolCode << 22) + (destinationAddr << 11) + sourceAddr;
    // _frameId = (_pCode << 22) + (_groupNumber << 17) + (_subAddress << 11) + (_monitorGroup << 6) + _monitorSubAddress;
    return 1;
}

int CegCharger::updateData(int msgId, int32_t value)
{
    switch (msgId)
    {
        case MessageIdRequest::Module_On_Off_32:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_On_Off_64:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_32:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_64:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;     
        case MessageIdRequest::Module_Voltage_Mode:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Voltage:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Current:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Modify_Group:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Query_Single_Module_Info:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(0);
            return 1;
            break;
    }
    _msgType = 0;
    _msgId[0] = 0;
    _msgId[1] = 0;
    return -1;
}

int CegCharger::buildData(int32_t value)
{
    _data[0] = _msgType;
    _data[1] = _errType;
    _data[2] = _msgId[0];
    _data[3] = _msgId[1];
    _data[4] = (value & 0xFF000000) >> 24;
    _data[5] = (value & 0xFF0000) >> 16;
    _data[6] = (value & 0xFF00) >> 8;
    _data[7] = value & 0xFF;
    return 1;
}

int32_t CegCharger::getFrameId()
{
    return _frameId;
}

int CegCharger::getData(int destination[], size_t arrSize)
{
    int status = -1;
    if (arrSize >= 8)
    {
        for (size_t i = 0; i < arrSize; i++)
        {
            destination[i] = _data[i];
        }
        status = 1;
    }
    return status;
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
 * XNOR : 0b1010110000000000000000000000
 * MASK : 0b0101001111111111111111111111
*/

int CegCharger::filterExtended(long id, long mask)
{
    // uint32_t temp;
    // temp = mask;
    id &= 0x1FFFFFFF;
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