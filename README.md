#WireTouch

WireTouch is an open capacitive multi-touch tracker in the making.
Please visit http://www.wiretouch.net for demos and background information.

##Building
###WireTouch Firmware
1. The heart of the WireTouch hardware is an ATmega328P microcontroller equipped with an Arduino bootloader. To upload the WireTouch firmware connect the mainboard with your computer. Use an USB to serial UART FTDI 5V cable.

2. Install the virtual com port [FTDI driver](http://www.ftdichip.com/Drivers/VCP.htm). Open arduino/sensor/sensor.io in the [Arduino IDE](http://arduino.cc/en/Main/Software). Select "Tools>Board>Arduino UNO" and the correct port. **Upload the Arduino sketch**.

3. You need to patch the FTDIUSBSerialDriver configuration to increase the baud rate of the computer-mainboard connection (see [FTDI Application Note 120](http://www.ftdichip.com/Support/Documents/AppNotes/AN_120_Aliasing_VCP_Baud_Rates.pdf)). We prepared a little perl script for that. Run it as the superuser:
```sudo perl/modify-ftdi-driver-settings.pl```
If you are running El Capitan you will also have to deactivate the new system integrity protection to make this work (see [https://github.com/arminbw/wiretouch/issues/1](issue #1).

###WireTouch Monitor
1. Download [openFrameworks 0.8.4](https://github.com/openframeworks/openFrameworks).
2. Download the following openFrameworks addons and put them into your openFrameworks "addons" directory.

  * ofxCv: https://github.com/kylemcdonald/ofxCv
  * ofxUI: https://github.com/rezaali/ofxUI
  * ofxBlobsManager: https://github.com/peteruithoven/ofxBlobsManager

3. Copy the wiretouch_monitor directory of this repository into your openFrameworks "apps/myApps/" directory.
4. We are currently using Xcode 7.3 on OSX 10.11.4 for development. Open the Xcode project. **Build it**.

Next to the openFrameworks addons we are also using several other 3rd-party libraries. You do not have to download them manually as they already reside inside the src/3rd-party directory (This list is just for future reference and to give credit).
* cjson: https://github.com/DaveGamble/cJSON
* oscpack: http://code.google.com/p/oscpack
* TUIO C++ library: https://github.com/mkalten/TUIO11_CPP

##License
WireTouch is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

A few of the included 3rd-party libraries are licensed under the GPL-compatible MIT license.

WireTouch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of merchantability or fitness for a particular purpose. See the [http://www.gnu.org/licenses](GNU General Public License) for more details.
