#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "interpolator-types.h"
#include "interpolator.h"

#define WINDOWWIDTH 1024
#define WINDOWHEIGHT 768
#define WINDOWBORDERDISTANCE 10
#define GUIWIDTH 300
#define GUIHEIGHT 600
#define WIDGETWIDTH (GUIWIDTH-(OFX_UI_GLOBAL_WIDGET_SPACING*2))
#define WIDGETHEIGHT 22

#define kGUIHalfwaveAmpName         ("HALFWAVE AMP")
#define kGUIOutputAmpName           ("OUTPUT AMP")
#define kGUISampleDelayName         ("SAMPLE DELAY")
#define kGUISignalFrequencyName     ("SIGNAL FREQUENCY")
#define kGUIPostProcessingName      ("POST PROCESSING")
#define kGUIUpSamplingName          ("UPSAMPLING")
#define kGUIInterpolationTypeName   ("INTERPOLATION")
#define kGUIGridName                ("GRID")
#define kGUIBlobsName               ("BLOBS")
#define kGUIStartName               ("START")
#define kGUILinearName              ("LINEAR")
#define kGUICatmullName             ("CATMULL")
#define kGUICosineName              ("COSINE")
#define kGUICubicName               ("CUBIC")
#define kGUIHermiteName             ("HERMITE")
#define kGUIWNNName                 ("WNN")
#define kGUILagrangeName            ("LAGRANGE")
#define kGUIBlobThresholdName       ("THRESHOLD")

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
        void guiEvent(ofxUIEventArgs &e);
        void sendSliderData(ofxUIEventArgs &e, char command);
    
        wtmAppState state;
    
        int sensorColumns, sensorRows, bytesPerFrame;
    
        ofSerial serial;
        unsigned char* recvBuffer;
        float lastRecvFrameTime;
    
        string* settingsString;
    
        ofTexture* texture;
    
        uint16_t* capGridValues;
    
        wtmInterpolator*        interpolator;
        wtmInterpolatorType     interpolatorType;
        int                     interpolatorUpsampleX, interpolatorUpsampleY;
    
        ofxUISuperCanvas *gui;
        bool bDrawBlobs;
        bool bDrawGrid;
};
