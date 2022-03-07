#include <Wire.h>
#include <Adafruit_ms8607.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_Sensor.h>

void setup() {
    Serial.begin(9600);
    Serial.println("MS and TSL test!");

    
}

void loop() {
    //MS8607
    sensors_event_t temp, pressure, humidity;
    ms8607.getEvent(&pressure, &temp, &humidity);
    //TSL2591

    delay(500);

}

void simpleRead(void) {

}