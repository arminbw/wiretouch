#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "interpolator-types.h"
#include "interpolator.h"

#define WINDOWWIDTH 1024
#define WINDOWHEIGHT 768
#define WINDOWBORDERDISTANCE 10

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
    
        ofxUICanvas *gui;
    	void guiEvent(ofxUIEventArgs &e);
        bool bDrawBlobs;
        bool bDrawGrid;
};
