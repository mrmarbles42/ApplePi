# **ApplePi sensor project**

================================================

## Description

The purpose of this project is to create a sensor network that is capable of identifying and monitoring agricultural pathogen conditions for IPM activities and research.

## Features

1. Ease of implementation and modification
2. Minimal costs related to extension and implementation

## How to use
### Required software and libraries
Arduino client
MS8607 library
TSL2591 library
Sandeepmistry arduino-LoRa library (github.com/sandeepmistry/arduino-LoRa)
Any raspbian distribution (for use on the Raspberry Pi)

### Required hardware
2x Adafruit 32u4 Feather with LoRa radio module
1x Adafruit MS8607 Pressure, Humidity & Temp sensor
1x Adafruit TSL2591 Light sensor
1x Grove Shield FeatherWing for Particle Mesh and all Feathers
2x Grove to STEMMA QT / Qwiic / JST SH cable - 100mm
2x 3.5in wire antenna
2x 12 and 16-pin female stacking headers
1x Lithium Ion Polymer Battery - 3.7v 350mAh
1x Raspberry Pi Zero WH
1x MicroSD card 16gb

### Hardware assembly
Begin preparation for assembly of the hardware by soldering the pairs of 12 and 16-pin stacking headers onto the LoRa Feathers. The stacking headers are necessary for connection to the Grove shield underneath and also enabled the attachment of a prototyping board on top of the feather. Then, cut and solder the 3.5 inch wire antennas to the boards to allow data transmission over longer distances (up to 2km line of sight with LoRa). This length is the suggested length of a wire antenna for the specific transmission frequency used in this design. Because of the STEMMA QT connectors, all that is left is to attach the cables to the sensors, connect those sensors to the Feather, and connect the feather to the battery. For the gateway component, all that would be required is to connect the LoRa Feather to the Raspberry Pi via usb connection without the battery. For field deployment, the assembled hardware would then be mounted inside the chosen water-proof housing. Battery levels would require periodic checking of charges.

### Software 
The software required for the platform is arduino libraries for the sensors, the arduino-LoRa library, the desired operating system distribution for the Raspberry Pi, and a client for compiling and uploading the Arduino sketches to the devices. 
The programming of the object is composed of the transmitting and receiving arduino sketches. The code was adapted from the individual sensor files and the LoRa library into a single .ino file that was capable of calibrating the sensor settings, reading from the sensors, making the sensor readings into a string object for transmission, and sending that data to the gateway component at a preset interval.

## Technologies
Many of the components involved in this project are developed and distributed by Adafruit! I chose Adafruit because of their good-quality, (mostly) easy to use, and well documented tech. All of these aspects of thier products make it much easier to implement and modify the platform effectively.

## Collaborators

## License
