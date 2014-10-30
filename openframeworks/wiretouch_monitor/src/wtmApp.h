/* Wiretouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of Wiretouch
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
 * along with Wiretouch. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "interpolator-types.h"
#include "interpolator.h"

#include "wtmTuioServer.h"
#include "wtmBlobTracker.h"

#define WINDOWWIDTH 1024
#define WINDOWHEIGHT 768
#define WINDOWBORDERDISTANCE 10
#define GUIWIDTH 300
#define GUIHEIGHT 600
#define WIDGETWIDTH (GUIWIDTH-(OFX_UI_GLOBAL_WIDGET_SPACING*2))
#define WIDGETHEIGHT 22

#define kGUIFPS                     ("FPS______")
#define kGUIHalfwaveAmpName         ("HALFWAVE AMP")
#define kGUIOutputAmpName           ("OUTPUT AMP")
#define kGUISampleDelayName         ("SAMPLE DELAY")
#define kGUISignalFrequencyName     ("SIGNAL FREQUENCY")
#define kGUIPostProcessingName      ("POST PROCESSING")
#define kGUIUpSamplingName          ("UPSAMPLING")
#define kGUIGridName                ("GRID")
#define kGUIInterpTypeName          ("INTERP_TYPE")
#define kGUIBlobsName               ("BLOBS")
#define kGUICalibrateName           ("CALIBRATE")
#define kGUIStartName               ("START")
#define kGUIStopName                ("STOP")
#define kGUIConnectSerialName       ("CONNECT SERIAL")
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
#define kGUIFirmwareName            ("FIRMWARE")

typedef enum _wtmAppState {
    wtmAppStateIdle,
    wtmAppStateReceivingSettings,
    wtmAppStateReceivingTouches
} wtmAppState;

class wtmApp : public ofBaseApp {

    public:
        void setup();
        void update();
        void draw();

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
        void consumeSettings(const char* json);
    
    protected:
        void updateInterpolator();
        void startSensor();
        void stopSensor();
        void drainSerial();
        void guiEvent(ofxUIEventArgs &e);
        void sendSliderData(ofxUIEventArgs &e, char command);
        void updateFPSLabelWithValue(float fps);
        void updateInterpolationTypeLabel(const char* newName);
        void updateFirmwareVersionLabel(const char* newVersion);
    
        int thresholdImageAlpha;
        double inputGamma;
    
        wtmAppState state;
    
        int sensorColumns, sensorRows, bytesPerFrame;
    
        ofSerial serial;
        bool bSerialConnectionAvailable, bSerialConnectionConfigured;
        bool resumeAfterSettingsReceipt;
        unsigned char* recvBuffer;
        float lastRecvFrameTime;
        float lastWindowResizeTime;
        float serialOpenTime;
        bool bSerialUpdated;
    
        string* settingsString;
    
        ofTexture* texture;
    
        uint16_t* capGridValues;
    
        wtmInterpolator*        interpolator;
        wtmInterpolatorType     interpolatorType;
        int                     interpolatorUpsampleX, interpolatorUpsampleY;
    
        wtmBlobTracker          blobTracker;
    
        ofxUISuperCanvas *gui;
        bool bTrackBlobs;
        bool bDrawGrid;
    
        wtmTuioServer* tuioServer;
    
        vector<string> serialDevicesNames;
    
        void distributeTuio();
};
