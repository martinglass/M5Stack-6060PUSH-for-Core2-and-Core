# M5Stack-6060Push-for-Core2-and-Core

* The 6060PUSH has a calibration issue (at least on mine). 
* A command of X40 moves the Axis by 50mm (instead of 40mm) 
* and the full extent of 57mm is achieved with a command of X45!!!
* 
* The 6060Push takes its power from a 9V-24V supply (5mm socket).
* The same power (at 9V-24V) is connected to the orange HT3.96 RS485 sockets on the 6060Push
* 
* The 4 x screws of the baseplate on the moving axis of 6060PUSH are UNC 1/4 inch thread, not M3 nor M4.
* 
* To connect between the Core2/Core and the 6060Push the recommendation is to use the PLC Proto Module.
* 
* The PLC Proto Module has a small RS485-to-TTL ancillary PCB board included, 
* which converts the 9V-24V power supply and serial communication signal lines for the 6060Push to 5V/3.3V for the Core2/Core.
* There are no instructions nor schematics for this small ancillary PCB board, but it is fairly obvious how it is supposed to be wired in 
* (it's on the corner of the PCB, between the M-Bus and the RS485 connector).
* But this won't work with the Core2, as the ancillary PCB board doesn't fit with the Core2. 
* 
* First, the haptic motor in the Core2 obstructs the ancillary board. 
* Secondly, because the motor has a metal case, there is a risk of shorting the 9V-24V power line with M-Bus pins 16 and 17.
* 
* In order to use the PLC Proto Mobule, you need to connect the ancillary board by wire, and keep it away from the M-Bus connector.
* 
* As mentioned, you'll need to wire up pins 16 and 17 on the PLC Proto Module ancillary board to the Core2 M-bus.
* You'll also need to wire that connects GND between the ancillary board and the Core2 (you can use the M-Bus GND pin or any other GND pin).
* 
* IMPORTANTLY, the ancillary board provides 9V-24V to the HPWR connectors on the MBus (pins 25,27, 29), 
* which on a Core these pins will provide 9V-24V power to the Core, but not on the Core2!! 
* The HWPWR pins are not connected in the Core2, so you need to have a power wire going from thePLC Proto Module 5V supply to the 5V input on the Core2. 
* This isn't obvious to do. 
* 
* There are several places to connect to the 5V supply on the PLC Proto module. But you need to wire into the 5V input pin inside the Core2, 
* which is one of the pins on the 4-pin connector inside the Core2, next to the USB connector. The pins are marked DP,DM,GND and 5V. 
* You want to connect from the 5V input pin on the Core2 by a wire to a 5V supply pin on the PLC Proto module board.
* 
* It may be better just to get the M5Stack RS485-to-TTL converter Unit instead of the PLC Proto Module !!!
* https://m5stack.com/collections/m5-unit/products/isolated-rs485-unit
* 
* The other issue is that the 6060Push doesn't come with a HT3.96 4-pin plug, to connect into the RS485 socket on the 6060Push.
* Unfortunately, the PLC Proto Module (and the RS485 Unit) only comes with 1 x HT3.96 4-pin connector, not two.
* You'll need to source one HR3.96 plug to go onto the 6060Push RS485 HT3.96 socket and they're not easy to find. 
* Here's an Aliexpress link here: https://www.aliexpress.com/item/32751855312.html
* 
