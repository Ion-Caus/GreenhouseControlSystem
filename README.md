# IoT part of semester project

*Greenhouse control system*

IoT team's GitHub name disambiguations: 
| GitHub username | Name | Student Nr. |
|-|-| -|
|Ion-Caus| Ion Caus | 304208 |
| GeraltWitcher1 | Deniss Puzirevs | 304579 |
| lukisko | Lukás Pleva | 304154 |
|helloItsBob | Aleksandrs Bistrovs | 304542 |

## Communication protocol for the LoRaWAN bridge
- Temperature is on byte 0 and 1 ->  int16_t multiplied by 10 (example 26°C = 260)
- Humidity is on byte 2 and 3 -> uint16_t multiplied by 10 (example 40% = 400)
- CO2 is on byte 4 and 5 -> uint16_t value is in ppm (example 2000ppm = 2000)
- Moisture[6] on byte 6 to 11 -> uint8_t value in % (example 54% moisture = 54)
- Status is on byte 12 -> each of bits represents state of some sensor - bit0: temperature, bit1: humidity, bit2: co2, bit3: moisture (example: 0000 0101_2 = temperature and co2 are working, humidity and moisture is not)


## Testing
testing in this project is made using google test and mocking.
to test something there is test solution for visual studio.

To test some new .c file you need to add the file to the project _right click on solution -> add -> existing file -> navigate to the .c file_.

