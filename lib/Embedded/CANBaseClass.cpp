#include "CANBaseClass.h"

CANBaseClass::CANBaseClass()
{
    _loopback = false;
}

void CANBaseClass::modifyRegister(uint8_t address, uint8_t mask, uint8_t value)
{
    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

    *reg = (*reg & ~mask) | value;
}
void CANBaseClass::writeRegister(uint8_t address, uint8_t value)
{
    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

    *reg = value;
}
uint8_t CANBaseClass::readRegister(uint8_t address)
{
    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

    return *reg;
}

/**
 * @brief   derived method from ESP32SJA1000 class, added timeout to prevent deadlock when CAN is not terminated or connected
*/
int CANBaseClass::endPacket()
{
    if (!CANControllerClass::endPacket()) {
    return 0;
  }

  const int timeoutValue = 100;
  int currenTimeout = 0;

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


/**
 * [int] filterExtended
 * @brief set the filter register, there are 2 register that need to be set, REG_ACR (Acceptance Code Register) and REG_AMR (Acceptance Mask Register). each register is 4 bytes
 *        divided into REG_ACR(0) - REG_ACR(3) and REG_AMR(0) - REG_AMR(3). REG_ACR is for the format id that can pass through filter. REG_AMR is the mask register which
 *        determine which bits need to be checked, set the bit as 1 mean that the bit position need to be evaluated, while 0 means don't care
 * @param id lowest id range
 * @param mask mask value
 * @return
 * 
 * To determine the mask value :
 * 1. Write the list of id that need to pass the filter
 * 2. XNOR bitwise the lowest of ID with the highest ID (for every different bit value will result as 0)
 * 3. To get Mask value, Bitwise Not the result of the step 2 (flip the bit of the XNOR result with its opposite value, e.g 1 become 0, 0 become 1)
 * 
 * Example 1:
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ID 2 : 0b1011000000000000011100000000
 * XNOR : 0b1010111111111111111111111111
 * MASK : 0b0101000000000000000000000000
 * 
 * This will pass id range from 0b1110000000000000011100000000 - 0b1110001111111111111111111111
 * Example 2 :
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ...
 * ID X : 0b1110001111111111111111111111
 * XNOR : 0b1111110000000000011100000000
 * MASK : 0b0000001111111111100011111111
*/
int CANBaseClass::filterExtended(long id, long mask)
{
    // uint32_t temp;
    // temp = mask;
    id &= 0x1FFFFFFF; //ignore the 29th - 31st bits by zero'ed the bits
    // temp = ~(temp & 0x1FFFFFFF);

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

int CANBaseClass::xnor(int a, int b)
{
    // Make sure a is larger
    if (a < b)
        swap(a, b);

    if (a == 0 && b == 0)
        return 1;

    int a_rem = 0; // for last bit of a
    int b_rem = 0; // for last bit of b

    // counter for count bit
    // and set bit  in xnornum
    int count = 0;

    // to make new xnor number
    int xnornum = 0;

    // for set bits in new xnor number
    while (a)
    {
        // get last bit of a
        a_rem = a & 1;

        // get last bit of b
        b_rem = b & 1;

        // Check if current two
        // bits are same
        if (a_rem == b_rem)       
            xnornum |= (1 << count);
        
        // counter for count bit
        count++;
        a = a >> 1;
        b = b >> 1;
    }
    return xnornum;
}

void CANBaseClass::swap(int &a, int &b)
{
    int num1, num2;
    num1 = a;
    num2 = b;
    b = num1;
    a = num2;
}

int CANBaseClass::loopback()
{
  _loopback = true;

  modifyRegister(REG_MOD, 0x17, 0x01); // reset
  modifyRegister(REG_MOD, 0x17, 0x04); // self test mode

  return 1;
}