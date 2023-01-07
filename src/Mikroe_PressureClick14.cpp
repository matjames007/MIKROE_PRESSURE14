/*
  This library was written by Matthew Budram for the MIKROE PRESSURE CLICK 14.  The sensor
  is capable of reading from 0 to 60 PSI.
  By: Matthew Budram
  Date: January 2023 
  License: This code is public domain but you buy me a bottle of wine if you use this and 
  we meet someday.
 */

#include "Mikroe_PressureClick14.h"

/* Constructor and sets default values.
   - (Optional) eoc_pin, End of Conversion indicator. Default: -1 (skip)
   - (Optional) rst_pin, Reset pin for MPR sensor. Default: -1 (skip)
   - minimumPSI, minimum range value of the sensor (in PSI). Default: 0
   - maximumPSI, maximum range value of the sensor (in pSI). Default: 60
*/
Mikroe_PressureClick14::Mikroe_PressureClick14(int8_t eoc_pin, int8_t rst_pin, uint8_t minimumPSI, uint8_t maximumPSI) {
  _eoc = eoc_pin;
  _rst = rst_pin;
  _minPsi = minimumPSI;
  _maxPsi = maximumPSI;
}

/* Initialize hardware
  - deviceAddress, I2C address of the sensor. Default: 0x28
  - wirePort, sets the I2C bus used for communication. Default: Wire
  
  - Returns 0/1: 0: sensor not found, 1: sensor connected
*/
bool Mikroe_PressureClick14::begin(uint8_t deviceAddress, TwoWire & wirePort) {
  _address = deviceAddress;
  _i2cPort = & wirePort;

  if (_eoc != -1) {
    pinMode(_eoc, INPUT);
  }

  if (_rst != -1) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(5);
    digitalWrite(_rst, HIGH);
    delay(5);
  }

  _i2cPort -> beginTransmission(_address);

  uint8_t error = _i2cPort -> endTransmission();

  if (error == 0) return true;
  else return false;
}

/* Read the status byte of the sensor
  - Returns status byte
*/
uint8_t Mikroe_PressureClick14::readStatus(void) {
  _i2cPort -> requestFrom(_address, 1);
  return _i2cPort -> read();
}

/* Read the Pressure Sensor Reading
 - (optional) Pressure_Units, can return various pressure units. Default: PSI
   Pressure Units available:
     - PSI: Pounds per Square Inch
	 - PA: Pascals
	 - KPA: Kilopascals
	 - TORR
	 - INHG: Inch of Mercury
	 - ATM: Atmospheres
	 - BAR
*/
float Mikroe_PressureClick14::readPressure(Pressure_Units units) {
  _i2cPort -> beginTransmission(_address);
  _i2cPort -> write((uint8_t) 0xAA);
  _i2cPort -> write((uint8_t) 0x00);
  _i2cPort -> write((uint8_t) 0x00);
  _i2cPort -> endTransmission();

  // Wait for new pressure reading available
  if (_eoc != -1) // Use GPIO pin if defined
  {
    while (!digitalRead(_eoc)) {
      delay(1);
    }
  } else // Check status byte if GPIO is not defined
  {
    uint8_t status = readStatus();
    while ((status & PRESSURE14_BUSY_FLAG) && (status != 0xFF)) {
      delay(1);
      status = readStatus();
    }
  }

  _i2cPort -> requestFrom(_address, 7);

  uint8_t status = _i2cPort -> read();

  //check memory integrity and math saturation bits
  if ((status & PRESSURE14_INTEGRITY_FLAG) || (status & PRESSURE14_MATH_SAT_FLAG)) {
    return NAN;
  }

  //read 24-bit pressure
  uint32_t reading = 0;
  for (uint8_t i = 0; i < 3; i++) {
    reading |= _i2cPort -> read();
    if (i != 2) reading = reading << 8;
  }

  //ignoring the temperature values
  for (uint8_t i = 0; i < 3; i++) {
    _i2cPort -> read(); //not storing or using these values 
  }

  //convert from 24-bit to float psi value
  float pressure;
  pressure = (reading - PRESSURE14_OUTPUT_MIN) * (_maxPsi - _minPsi);
  pressure = (pressure / (PRESSURE14_OUTPUT_MAX - PRESSURE14_OUTPUT_MIN)) + _minPsi;

  if (units == PSI) return pressure * PRESSURE14_CONST_PSI; //PSI
  else if (units == PA) return pressure * PRESSURE14_CONST_PASCAL;
  else if (units == KPA) return pressure * PRESSURE14_CONST_KILOPASCAL;
  else if (units == TORR) return pressure * PRESSURE14_CONST_MMHG; //torr (mmHg)
  else if (units == INHG) return pressure * PRESSURE14_CONST_INHG; //inHg (inch of mercury)
  else if (units == ATM) return pressure * PRESSURE14_CONST_ATMOSPHERIC; //atm (atmosphere)
  else if (units == BAR) return pressure * PRESSURE14_CONST_BAR; //bar
  else return pressure * PRESSURE14_CONST_PSI; //PSI
}

/* Read the Temperature Sensor Reading
 - (optional) Temperature_Units, can return various temperature units. Default: CELSIUS
   Temperature Units available:
     - CELSIUS
	 - KELVIN
	 - FAHRENHEIT
*/
float Mikroe_PressureClick14::readTemperature(Temperature_Units unit) {
  _i2cPort -> beginTransmission(_address);
  _i2cPort -> write((uint8_t) 0xAA);
  _i2cPort -> write((uint8_t) 0x00);
  _i2cPort -> write((uint8_t) 0x00);
  _i2cPort -> endTransmission();

  // Wait for new pressure reading available
  if (_eoc != -1) { // Use GPIO pin if defined

    while (!digitalRead(_eoc)) {
      delay(1);
    }
  } else { // Check status byte if GPIO is not defined
    uint8_t status = readStatus();
    while ((status & PRESSURE14_BUSY_FLAG) && (status != 0xFF)) {
      delay(1);
      status = readStatus();
    }
  }

  _i2cPort -> requestFrom(_address, 7);

  uint8_t status = _i2cPort -> read();

  //check memory integrity and math saturation bits
  if ((status & PRESSURE14_INTEGRITY_FLAG) || (status & PRESSURE14_MATH_SAT_FLAG)) {
    return NAN;
  }

  //ignoring the pressure reading portion of the buffer
  uint32_t reading = 0;
  for (uint8_t i = 0; i < 3; i++) {
    reading = _i2cPort -> read();
  }

  reading = 0;
  //read 24-bit temperature
  for (uint8_t i = 0; i < 3; i++) {
    reading |= _i2cPort -> read();
    if (i != 2) reading = reading << 8;
  }

  float conv_addi_constant;
  float conv_mult_constant;
  float result;

  switch (unit) {
  case CELSIUS:
    conv_addi_constant = PRESSURE14_CONST_ADDI_CELS;
    conv_mult_constant = PRESSURE14_CONST_MULT_CELS;
    break;

  case KELVIN:
    conv_addi_constant = PRESSURE14_CONST_ADDI_KELV;
    conv_mult_constant = PRESSURE14_CONST_MULT_KELV;
    break;

  case FAHRENHEIT:
    conv_addi_constant = PRESSURE14_CONST_ADDI_FAHR;
    conv_mult_constant = PRESSURE14_CONST_MULT_FAHR;
    break;
  default:
    conv_addi_constant = PRESSURE14_CONST_ADDI_CELS;
    conv_mult_constant = PRESSURE14_CONST_MULT_CELS;
    break;
  }

  result = (float)(reading);
  result *= PRESSURE14_CONST_TEMP_MAX - PRESSURE14_CONST_TEMP_MIN;
  result /= PRESSURE14_CONST_TEMP_OUT_MAX;
  result += PRESSURE14_CONST_TEMP_MIN;
  result *= conv_mult_constant;
  result += conv_addi_constant;
  return result;
}