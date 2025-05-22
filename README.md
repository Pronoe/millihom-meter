# milliohm-meter
Portable and low cost milliommeter with good performances

# Introduction
When you have to debug electronic systems or find failure on an electronic board or electrical system, you frequently need to locate short circuits.
Common multimeters  are generally limited down to 1 ohm in their measurement and this is not sufficient for efficient characterization and localization of a short circuit.
Below the 1 ohm domain, a milliohmmemeter is the required instrument but Industrial miiliohmmeters are expensive so I decided to build my own device.

# General design
I had viewed a good publication for a DIY milliohmmeter from the "Electro-bidouilleur" YouTube channel. The Elektor magazine sells also a add-on device for multimeters.
However this projects suffers several drawbacks:
-	need a separate multimeter for the measurement
-	provides no protection against excessive voltage on the device under test
-	do not use 4 wires measurement for better accuracy
The main characteristics of my design are:
-	use of a INA199 from Texas Instrument for the measurement
o	this is a Voltage-Output, Current-Shunt Monitor which need no additional component
o	provides very low offset voltage and very low drift
o	high accuracy gain
-	current source of 10.6 mA build with a ½ TLC272 which is a precision dual operational amplifier
-	voltage limiter with a BC548 transistor
-	use of and Arduino Uno board for:
o	output voltage measurement,
o	resistance computation from voltage measurement
o	initial zero compensation
o	full range calibration
-	use of a 2 lines x 16 characters LCD display shield for user interface
-	use of a a 1200 mAh LiPo battery combined with a step up converter/ battery charger  for portable operation
-	use of a "Kelvin" 4 wires cable for device connection

see detailed information in technical files
