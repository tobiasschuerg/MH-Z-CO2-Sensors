# MH-Z14A, MH-Z19B CO2 ... Module

Arduino implementation for MH-Z CO2 sensors such as **MH-Z14A**, **MH-Z18B** (as I didn't find all info in one place).

The sensor is available for ~20 bucks at the usual places.

## Implementation
See example.

> PPMuart: 602, PPMpwm: 595, Temperature: 23

### Implementation details

```
C ppm = 5000 * (T_high - 2 ms) / (T_high + T_low - 4ms)
```

The implementation is mostly based on  https://forum.arduino.cc/index.php?topic=525459.msg3587557#msg3587557


## Resources:

Datasheet:
http://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf

More info about the sensor:
https://revspace.nl/MHZ19


## Further reading:
- http://ihormelnyk.com/Page/mh-z19_co2_meter

(russuian, but google translate does a good job)
- https://geektimes.ru/post/285572/
- https://geektimes.ru/post/278178/
