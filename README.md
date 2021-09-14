# Domaci automatizace s Arduidem

## Hystereze lednice (Arduino MKR Wifi 1010)
Budes potrebovat micro USB kabel. Toto se tyka [Arduino MKR Wifi 1010](https://docs.arduino.cc/hardware/mkr-wifi-1010). Nezapomen spravne nastavit `arduino_secrets.h`!
* Min a max hodnoty hystereze lze nastavit [zde](https://github.com/podhrmic/HomeAutomation/blob/master/Chata_Wifi_Adafruit/chata_wifi.cpp#L431) - doporucuji testovat s rozdilnymi dobami spanku (viz nize).

## Doba spanku (Arduino Uno)
Pozor - budes potrebovat USB A->B kabel. Toto se tyka [Arduino Uno](https://www.arduino.cc/en/Main/arduinoBoardUno&gt;).
* Muzeme nastavit jak dlouho bude arduino spat - defaultni je 120 vterin, lze zmenit [zde](https://github.com/podhrmic/HomeAutomation/blob/master/Chata_uno_sleep/Chata_uno_sleep.ino#L9)
* Muzeme take nastavit jak dlouho bude arduino vzuhuru nez pujde zase spat - defaultni je 120 vterin, lze zmenit [zde](https://github.com/podhrmic/HomeAutomation/blob/master/Chata_uno_sleep/Chata_uno_sleep.ino#L11). Pro spravnou funkcni hystereze doporucuji alespon 30min, tzn. 1800 vterin.
* Stale vzhuru - staci spojit piny 12 a 13 (pinout [zde](https://www.circuito.io/blog/arduino-uno-pinout/)) a arduino bude stale vzhuru. Logika je naprogramovana [zde](https://github.com/podhrmic/HomeAutomation/blob/master/Chata_uno_sleep/Chata_uno_sleep.ino#L108). Pozor - stale vzhuru nedoporucuji, nebot Arduino MKR ma problemy po cca hodine nepretrziteho chodu.