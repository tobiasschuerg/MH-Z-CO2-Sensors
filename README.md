# MH-Z14A, MH-Z19B CO2 ... Module

This repository contains an Arduino implementation for MH-Z CO2 sensors, including MH-Z14A, MH-Z19B, and MH-Z19C. It aims to consolidate information on these sensors in one place for easy access.

The sensor is available for ~20 bucks at the usual places.

## Implementation

Refer to the provided example for implementation details.

> PPMuart: 602, PPMpwm: 595, Temperature: 23

### Implementation details

```
C ppm = 5000 * (T_high - 2 ms) / (T_high + T_low - 4ms)
```

The implementation primarily draws from this [Arduino forum post](https://forum.arduino.cc/index.php?topic=525459.msg3587557#msg3587557).

## Usage
By default, the PWM range value is set to 5000. You do not need to change anything in the class constructor if the Cppm value is within the expected range of 400-1000. However, if necessary, you can test it with a 2000 range value:

### Read co2 via PWM
```cpp
#include <MHZ.h>
#define CO2_IN 9

MHZ co2(CO2_IN, MHZ::MHZ19B); // here the range value is set to 5000 by default (RANGE_5K)
int ppm = co2.readCO2PWM();
```

### Read co2 via UART
```cpp
#include <MHZ.h>
#define MH_Z19_RX 10
#define MH_Z19_TX 11
MHZ co2(MH_Z19_RX, MH_Z19_TX, MHZ19B);
int ppm = co2.readCO2UART();
```

## Supported Sensors
All `MH` sensors work mostly the same. They only differ in detection range and timings.  
Here is a list of all explicitly supported sensors:

| Sensor    | Detection Range | Reference / Datasheet                                            |
|-----------|-----------------|------------------------------------------------------|
| MH-Z14A   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z14a.html   |
| MH-Z14B   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z19b.html   |
| MH-Z16    | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z16.html    |
| MH-Z1911A | 0~10000ppm      | https://www.winsen-sensor.com/product/mh-z1911a.html |
| MH-Z19B   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z19b.html   |
| MH-Z19C   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z19c.html   |
| MH-Z19D   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z19d.html   |
| MH-Z19E   | 400~10000ppm    | https://www.winsen-sensor.com/product/mh-z19e.html   |

Here is also a [complete overview of all CO2 sensors](https://www.winsen-sensor.com/co2-sensor/).

## Resources:

Good overview of the different sensor types, how they work and calibrate:
https://www.winsen-sensor.com/knowledge/what-is-co2-sensor.html

### More infos about the MHZ sensors an wiring:
 - https://revspace.nl/MHZ19
 - https://wolles-elektronikkiste.de/en/mh-z14-and-mh-z19-co2-sensors
 - https://emariete.com/en/sensor-co2-mh-z19b/

### Further reading:
- [Building a MH-Z19 CO2 meter](http://ihormelnyk.com/Page/mh-z19_co2_meter)

(russuian, but google translate does a good job)
- https://geektimes.ru/post/285572/
- https://geektimes.ru/post/278178/
