# MH-Z19B CO2 Module

Arduino implementation for the MH-Z19B sensor (as I didn't find all info in one place).
The sensor is available for ~20 bucks at the usual places.

## Resources:

Datasheet:
http://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf

More info about the sensor:
https://revspace.nl/MHZ19


## Implementation

```
C ppm = 5000 * (T_high - 2 ms) / (T_high + T_low - 4ms)
```

The implementation is mostly based on  https://forum.arduino.cc/index.php?topic=525459.msg3587557#msg3587557


## Further reading:
- http://ihormelnyk.com/Page/mh-z19_co2_meter

(russuian, but google translate does a good job)
- https://geektimes.ru/post/285572/
- https://geektimes.ru/post/278178/