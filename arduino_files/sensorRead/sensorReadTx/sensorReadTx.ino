//Included libraries
#include <Wire.h>
#include <RH_RF95.h>
#include <LoRa.h>
#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "Adafruit_SHT4x.h"


/*
DEFINITIONS
*/

 //for feather 32u4 board
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

/* for feather m0 board
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
*/

//for water sensor
int value = 0;
#define POWER_PIN 13 //define digital pin attachment
#define SIGNAL_PIN A0 //define analogRead pin
#define VBATPIN 7 //define battery voltage pin

//for RF95
#define RF95_FREQ 915.0 // Change to 434.0 or other frequency, must match RX's freq!
RH_RF95 rf95(RFM95_CS, RFM95_INT); // Singleton instance of the radio driver

/** RF95 addressing
RH_RF95 class does not provide for addressing or reliability, 
so you should only use RH_RF95 if you do not need the higher-level 
messaging abilities.
**/

const long frequency = 915E6;  // LoRa Frequency

const int csPin = 8;          // LoRa radio chip select
const int resetPin = 4;        // LoRa radio reset
const int irqPin = 7;          // change for your board; must be a hardware interrupt pin

//Sensor definitions
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); 
Adafruit_MS8607 ms8607;
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

/*
FUNCTIONS
*/
//Read battery voltage
void battVoltage(void) {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
}

//Display TSL properties
void displayTslDetails(void) {
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}

//Configure RFM95 LoRa radio
/*
Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
The default transmitter power is 13dBm, using PA_BOOST.
If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
you can set transmitter powers from 5 to 23 dBm:
*/

void ConfigureLora(void) {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Node");
  Serial.println("Only receive messages from gateways");
  Serial.println("Tx: invertIQ disable");
  Serial.println("Rx: invertIQ enable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}
}


//Configure TSL sensor (init, gain, and integration time)   
void configureTsl(void) {
  if (tsl.begin()) 
  {
    Serial.println(F("Found TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("Failed to find TSL2591"));
    while (1);
  }
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  /*
  Changing the integration time gives you a longer time over which to sense light
  longer timelines are slower, but are good in very low light situtations!
  */ 
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
   tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  //Display TSL gain and integration time for reference and debug
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

//Configure SHT40 (init, precision, and heater)
 
void configureSht(void) { // You can have 3 different precisions, higher precision takes longer

 sht4.setPrecision(SHT4X_HIGH_PRECISION); //Set sensor precision
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }

 sht4.setHeater(SHT4X_NO_HEATER); //Set sensor heater
   switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER:  // You can have 6 different heater settings; higher heat and longer times uses more power and reads will take longer too!
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }
  
}

//Configure MS8607 (init and pressure/humidity resolutions)
void configureMs(void) {
   if (ms8607.begin()) 
  {
    Serial.println("Found MS8607 sensor"); 
  }
  else
  {
    Serial.println("Failed to find MS8607");
    while (1);
  }
  ms8607.setHumidityResolution(MS8607_HUMIDITY_RESOLUTION_OSR_8b);
  Serial.print("Humidity resolution set to ");
  switch (ms8607.getHumidityResolution()){
    case MS8607_HUMIDITY_RESOLUTION_OSR_12b: Serial.println("12-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_11b: Serial.println("11-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_10b: Serial.println("10-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_8b: Serial.println("8-bit"); break;
  }
  ms8607.setPressureResolution(MS8607_PRESSURE_RESOLUTION_OSR_4096);
  Serial.print("Pressure and Temperature resolution set to ");
  switch (ms8607.getPressureResolution()){
    case MS8607_PRESSURE_RESOLUTION_OSR_256: Serial.println("256"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_512: Serial.println("512"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_1024: Serial.println("1024"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_2048: Serial.println("2048"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_4096: Serial.println("4096"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_8192: Serial.println("8192"); break;
  }
  Serial.println("");
}

//MS8607 read event
void msRead(void) {
  sensors_event_t temp, pressure, humidity;
  ms8607.getEvent(&pressure, &temp, &humidity);
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Pressure: ");Serial.print(pressure.pressure); Serial.println(" hPa");
  Serial.print("Humidity: ");Serial.print(humidity.relative_humidity); Serial.println(" %rH");
  Serial.println("");
  delay(500);
}

//TSL read events

/*
Simple data read example. Just read the infrared, fullspecrtrum diode 
or 'visible' (difference between the two) channels.
This can take 100-600 milliseconds!
*/
void tslSimpleRead(void) {
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
  delay(500);
}

void shtRead(void) {
  sensors_event_t humidity, temp;
  
  //uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  //timestamp = millis() - timestamp;

  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  Serial.print("Read duration (ms): ");
  //Serial.println(timestamp);
}
/*
More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
That way you can do whatever math and comparisons you want!
*/
void tslAdvancedRead(void) {
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir; 
  uint16_t full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
  delay(500);
}

//Water sensor read event
void waterSensorRead(void){
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = (analogRead(SIGNAL_PIN) * 100) / 400; // read the analog value from sensor and convert to rough %
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Sensor value: ");
  Serial.println(value);
}

String csv_val;

//Print sensor values to csv-style string for transmit
void csvPrint(void) {
  sensors_event_t temp, pressure, humidity;
  //uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  uint16_t x = tsl.getFullLuminosity();
  
  csv_val = String(temp.temperature, DEC);
  csv_val = csv_val + ",";
  csv_val = csv_val + String(pressure.pressure);
  csv_val = csv_val + ",";
  csv_val = csv_val + String(humidity.relative_humidity);
  csv_val = csv_val + ",";
  csv_val = csv_val + String(x, DEC);
  Serial.println(csv_val);
  
}

//Send csv_print string over LoRa
void loraSend(void) {
  if (runEvery(1000)) { // repeat every 1000 millis

    String message = csv_val;
    message += millis();

    LoRa_sendMessage(message); // send a message

    Serial.println("Send Message!");
  }
}

//SETUP
void setup() {
    Serial.begin(9600);
    Serial.println("Sensor test!");

 while (!Serial) delay(10);

  configureTsl();
  configureMs();
  ConfigureLora();
  configureSht();
  displayTslDetails();
}

//LOOP
void loop() {
  //battVoltage();

  tslAdvancedRead();
  //tslSimpleRead();
  msRead();
  //shtRead();
  //waterSensorRead();

  csvPrint();
  delay(100);
  loraSend();
  delay(1000);
}
 