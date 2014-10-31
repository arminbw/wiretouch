#WireTouch

WireTouch is an open capacitive multi-touch tracker in the making.
Please visit http://www.wiretouch.net for demos and background information.


##WireTouch Monitor

###Building
We are currently using Xcode 6.1 on OSX 10.10 for development. To successully build the Xcode project you will have to download [openFrameworks 0.8.4](https://github.com/openframeworks/openFrameworks). You will also have to download the following openFrameworks addons:

* ofxCv: https://github.com/kylemcdonald/ofxCv
* ofxUI: https://github.com/rezaali/ofxUI
* ofxBlobsManager: https://github.com/peteruithoven/ofxBlobsManager

Put them into your openFrameworks "addons" directory.

Copy the wiretouch_monitor directory of this repository into your openFrameworks "apps/myApps/" directory and open the Xcode project. **Build it**.

Next to the openFrameworks addons we are also using several other 3rd-party libraries. You do not have to download them manually as they already reside in the src/3rd-party directory. This list is just for future reference and to give credit where credit is due:
* cjson: http://cjson.sourceforge.net/
* oscpack: http://code.google.com/p/oscpack/
* TUIO C++ library: http://reactivision.sourceforge.net

##License
WireTouch is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

A few of the included 3rd-party libraries are licensed under the GPL-compatible MIT license.