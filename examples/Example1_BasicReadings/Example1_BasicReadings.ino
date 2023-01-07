#include<Wire.h>

#include <Mikroe_PressureClick14.h>

/*
 * Initialize Constructor
 * Optional parameters:
 *  - EOC_PIN: End Of Conversion (defualt: -1)
 *  - RST_PIN: Reset (defualt: -1)
 *  - MIN_PSI: Minimum Pressure (default: 0 PSI)
 *  - MAX_PSI: Maximum Pressure (default: 60 PSI)
 */
Mikroe_PressureClick14 mpc14;

void setup() {
  // Initalize UART, I2C bus, and connect to the sensor
  Serial.begin(115200);
  Wire.begin();

  /* The pressure and temperature sensor uses default settings with the address 0x28 using Wire.

     The pressure and temperature sensor has a fixed I2C address, if another address is used it
     can be defined here. If you need to use two pressure and temperature sensors, and your
     microcontroller has multiple I2C buses, these parameters can be changed here.

     E.g. mpr.begin(ADDRESS, Wire1)

     Will return true on success or false on failure to communicate. */
  if (!mpc14.begin()) {
    Serial.println("Cannot connect to MIKROE pressure and temperature sensor.");
    while (1);
  }
}

void loop() {
  /* The pressure sensor outputs readings in pounds per square inch (PSI).
     Optionally, if you prefer pressure in another unit, the library can convert the
     pressure reading to: pascals, kilopascals, bar, torr, inches of murcury, and
     atmospheres.
   */
  Serial.println("MIKROE PRESSURE");
  Serial.print(mpc14.readPressure(), 4);
  Serial.println(" PSI");
  Serial.print(mpc14.readPressure(PA), 1);
  Serial.println(" Pa");
  Serial.print(mpc14.readPressure(KPA), 4);
  Serial.println(" kPa");
  Serial.print(mpc14.readPressure(TORR), 3);
  Serial.println(" torr");
  Serial.print(mpc14.readPressure(INHG), 4);
  Serial.println(" inHg");
  Serial.print(mpc14.readPressure(ATM), 6);
  Serial.println(" atm");
  Serial.print(mpc14.readPressure(BAR), 6);
  Serial.println(" bar");
  Serial.println("###############################");
  Serial.println();
  delay(500);

  /* The temperature sensor outputs pressure readings in celsius.
     Optionally, if you prefer pressure in another unit, the library can convert the
     pressure reading to: kelvin and fahrenheit.
   */
  Serial.println("MIKROE TEMPERATURE");
  Serial.print(mpc14.readTemperature(CELSIUS), 2);
  Serial.println(" C");
  Serial.print(mpc14.readTemperature(KELVIN), 2);
  Serial.println(" K");
  Serial.print(mpc14.readTemperature(FAHRENHEIT), 2);
  Serial.println(" F");
  Serial.println("###############################");
  Serial.println();
  delay(500);
}