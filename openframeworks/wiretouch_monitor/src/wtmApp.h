/* Wiretouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of WireTouch
 *
 * Wiretouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wiretouch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WireTouch. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "cJSON.h"

#include "interpolator-types.h"
#include "interpolator.h"

#include "wtmTuioServer.h"
#include "wtmBlobTracker.h"

#define WIRETOUCHVERSION            ("WireTouch 0.21")

#define WINDOWWIDTH 1024
#define WINDOWHEIGHT 768
#define WINDOWBORDERDISTANCE 10
#define GUIWIDTH 280
#define GUIHEIGHT 695
#define WIDGETWIDTH (GUIWIDTH-(OFX_UI_GLOBAL_WIDGET_SPACING*2))
#define WIDGETHEIGHT 22

#define kGUIFPS                     ("FPS__")
#define kGUIHalfwaveAmpName         ("HALFWAVE AMP")
#define kGUIOutputAmpName           ("OUTPUT AMP")
#define kGUISampleDelayName         ("SAMPLE DELAY")
#define kGUISignalFrequencyName     ("SIGNAL FREQUENCY")
#define kGUIUpSamplingName          ("UPSAMPLING")
#define kGUIGridName                ("GRID")
#define kGUIInterpolationDropDownName   ("INTERPOLATION TYPE")
#define kGUISerialDropDownName          ("CHOOSE SERIAL PORT")
#define kGUIBlobsName               ("SHOW BLOBS")
#define kGUICalibrateName           ("CALIBRATE")
#define kGUIResetName               ("RESET")
#define kGUIStartName               ("START")
#define kGUIStopName                ("STOP")
#define kGUIFirmwareName            ("FIRMWARE")

#define kGUILinearName              ("LINEAR")
#define kGUICatmullName             ("CATMULL")
#define kGUICosineName              ("COSINE")
#define kGUICubicName               ("CUBIC")
#define kGUIHermiteName             ("HERMITE")
#define kGUIWNNName                 ("WNN")
#define kGUILagrangeName            ("LAGRANGE")

#define kGUIBlobThresholdName       ("THRESHOLD")
#define kGUIBlobVisualizationName   ("BLOB VISUALIZATION")
#define kGUIBlobGammaName           ("GAMMA")
#define kGUIBlobAdaptiveThresholdRangeName ("ADAPTIVE RANGE")

typedef enum _wtmAppState {
    wtmAppStateNoSerialConnection,
    wtmAppStateStartWhenSerialInitalized,
    wtmAppStateIdle,
    wtmAppStateReceivingCalibrationResults,
    wtmAppStateReceivingTouches,
} wtmAppState;

class wtmApp : public ofBaseApp {

    public:
        void setup();
        void update();
        void draw();

        void moveWidgetsBeneathDropdown(ofxUIDropDownList* widget, bool moveBack);
        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void exit();

        void consumePacketData();
        void consumeCalibrationResults(const char* json);
        void saveSettings();
        void getJSONObject(cJSON *root, const char* string);
        void loadSettings();
    
    protected:
        void initGUI();
        void updateInterpolator();
        void startSensor();
        void stopSensor();
        void startCalibration();
        void resetCalibration();
        bool initAndStartSerialConnection(string serialDeviceName);
        void closeSerialConnection();
        void drainSerial();
        void guiEvent(ofxUIEventArgs &e);
        void sendSliderData(ofxUISlider* slider);
        void updateFPSLabelWithValue(float fps);
        void updateFrequencyLabel();
        void updateFirmwareVersionLabel(const char* newVersion);
        void distributeTuio();
    
        int thresholdImageAlpha;
        double inputGamma;
    
        wtmAppState state;
    
        int sensorColumns, sensorRows, bytesPerFrame;
    
        ofSerial serial;
        unsigned char* recvBuffer;
        float lastRecvFrameTime;
        int fpsCounter;
        float lastWindowResizeTime;
        float serialOpenTime;
    
        string calibrationResultsString;
    
        ofTexture* texture;
    
        uint16_t* capGridValues;
    
        wtmInterpolator*        interpolator;
        wtmInterpolatorType     interpolatorType;
        int                     interpolatorUpsampleX, interpolatorUpsampleY;
    
        wtmBlobTracker*         blobTracker;
    
        ofxUISuperCanvas *gui;
        bool bNewDataToShow;
        bool bTrackBlobs;
        bool bDrawGrid;
    
        wtmTuioServer* tuioServer;
    
        vector<string> serialDevicesNames;
};
