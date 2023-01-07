#if(ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#define PRESSURE14_DEFAULT_ADDRESS 0x28
#define PRESSURE14_MAXIMUM_PSI 60
#define PRESSURE14_MINIMUM_PSI 0

#define PRESSURE14_BUSY_FLAG 0x20
#define PRESSURE14_INTEGRITY_FLAG 0x04
#define PRESSURE14_MATH_SAT_FLAG 0x01

#define PRESSURE14_OUTPUT_MAX 0xE66666
#define PRESSURE14_OUTPUT_MIN 0x19999A

#define PRESSURE14_CONST_TEMP_MIN - 50.000000
#define PRESSURE14_CONST_TEMP_MAX 150.000000
#define PRESSURE14_CONST_TEMP_OUT_MAX 16777215.0

#define PRESSURE14_CONST_PSI 1.00000
#define PRESSURE14_CONST_PASCAL 6894.7573
#define PRESSURE14_CONST_KILOPASCAL 6.89476
#define PRESSURE14_CONST_MMHG 51.7149
#define PRESSURE14_CONST_INHG 2.03602
#define PRESSURE14_CONST_ATMOSPHERIC 0.06805
#define PRESSURE14_CONST_BAR 0.06895

#define PRESSURE14_CONST_ADDI_CELS 0.00000
#define PRESSURE14_CONST_MULT_CELS 1.00000
#define PRESSURE14_CONST_ADDI_KELV 273.150
#define PRESSURE14_CONST_MULT_KELV 1.00000
#define PRESSURE14_CONST_ADDI_FAHR 32.0000
#define PRESSURE14_CONST_MULT_FAHR 1.80000

enum Pressure_Units {
  PSI,
  PA,
  KPA,
  TORR,
  INHG,
  ATM,
  BAR
};

enum Temperature_Units {
  KELVIN,
  CELSIUS,
  FAHRENHEIT
};

class Mikroe_PressureClick14 {
  public: Mikroe_PressureClick14(int8_t eoc_pin = -1, int8_t rst_pin = -1, uint8_t minimumPSI = PRESSURE14_MINIMUM_PSI, uint8_t maximumPSI = PRESSURE14_MAXIMUM_PSI);
  bool begin(uint8_t deviceAddress = PRESSURE14_DEFAULT_ADDRESS, TwoWire & wirePort = Wire);
  uint8_t readStatus(void);
  float readPressure(Pressure_Units units = PSI);
  float readTemperature(Temperature_Units units = CELSIUS);

  private: int8_t _address;
  int8_t _eoc,
  _rst;

  uint8_t _minPsi,
  _maxPsi;

  TwoWire * _i2cPort;
};