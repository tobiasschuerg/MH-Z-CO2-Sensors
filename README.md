# MH-Z14A, MH-Z19B CO2 ... Module

Arduino implementation for MH-Z CO2 sensors such as **MH-Z14A**, **MH-Z19B** and **MH-Z19C** (as I didn't find all info in one place).

The sensor is available for ~20 bucks at the usual places.

## Implementation
See example.

> PPMuart: 602, PPMpwm: 595, Temperature: 23

### Implementation details

```
C ppm = 5000 * (T_high - 2 ms) / (T_high + T_low - 4ms)
```

The implementation is mostly based on  https://forum.arduino.cc/index.php?topic=525459.msg3587557#msg3587557

## Usage
By default the PWM range value is set to 5000 and there is no need to change anything in the class constructor (if the Cppm value is in the expected range 400-1000), otherwise you may want to test it with 2000 range value :
```
#include <MHZ.h>
#define CO2_IN 9
#define MH_Z19_RX 10
#define MH_Z19_TX 11

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B); // here the range value is set to 5000 by default (RANGE_5K)
MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B, RANGE_2K); // here the range value is set to 2000
```

## Resources:

Datasheet (MH-Z19B):
http://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf

Datasheet (MH-Z19C):
https://pdf1.alldatasheet.com/datasheet-pdf/view/1303687/WINSEN/MH-Z19C.html

More info about the sensor:
https://revspace.nl/MHZ19


## Further reading:
- http://ihormelnyk.com/Page/mh-z19_co2_meter

(russuian, but google translate does a good job)
- https://geektimes.ru/post/285572/
- https://geektimes.ru/post/278178/
