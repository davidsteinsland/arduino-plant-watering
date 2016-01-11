Arduino Plant Watering
======================

A Real Time Clock controlled water pump regulator, for watering your plants.
The LCD have four available states:

- Initial state: Shows the time and how many times the pump has ran
- Pump configuration: Configure how many seconds the pump will run
- Schedule configuration: Enable/disable the timer
- Alarm configuration: Configure the time between each watering (can be every minute; a specific minute every hour; or a specific time each day).

The internal software is using a mixture of the RTC module and AVR's Watchdog Timer to properly time the pump and the schedule.
A float switch is also connected to the circuit, allowing the pump to run if there is enough water. It will also cut-off the pump as soon
as float switch turns off.

The power supply is a 12v wall-wart that's fused with 50mA to Arduino and 5A to the pump.

The three buttons are regulated at 0V, 1V and 2V, and read using `A0`. The switches are debounced in software.

[![Circuit](circuit_small.png)](circuit.png)

# Hardware

* [12V relay module](http://www.kjell.com/se/sortiment/el/elektronik/arduino/moduler/relamodul-for-arduino-p87878)
* [Water hose](http://www.biltema.no/no/Bil---MC/Bilreservedeler/Viskerutstyr/Spylerpumpe/Spylerpumpe-ovrig/Spylerslange-5-m-2000031910/)
* [Windshield washer pump](http://www.biltema.no/no/Bil---MC/Bilreservedeler/Viskerutstyr/Spylerpumpe/Spylerpumpe/Vindusspylerpumpe-universal-2000017965/)
* [12V power supply](http://www.biltema.no/no/Kontor---Teknikk/Batteri-og-Stromforsyning/Nettaggregat-og-ladere/Nettadapter-2000022525/)
* [16x2 LCD](http://hobbykomponenter.no/elektronikk/komponenter/display/lcd/16-and-215-2-lcd-display-blaa/)
* [DS3231 RTC module](http://hobbykomponenter.no/elektronikk/moduler/ds3231-rtc-modul/) (or similar)
* [Float switch](http://hobbykomponenter.no/elektronikk/sensorer/nivaasensor-sidemontert-flottr/)

## Additional

* Resistors
* Wires
* LEDs
* A switch button
* Free time.

# Future improvements

- Flowmeter: Configure the amount of water your want to be pumped, instead of pump time
- Hardware debouncing of buttons
- Noise reducing of water pump when it's active
- Status LEDs
- Bluetooth / integration with the nRF51 kit by Nordic Semiconductor.
