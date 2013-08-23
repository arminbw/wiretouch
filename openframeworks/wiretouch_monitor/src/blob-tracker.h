//
//  blob-tracker.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 16/8/13.
//
//

#ifndef __wiretouch_monitor__blob_tracker__
#define __wiretouch_monitor__blob_tracker__

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBlobsManager.h"

class wtmBlobTracker {
public:
    wtmBlobTracker();
    ~wtmBlobTracker();
    
    int                     threshold;
    
    void setGrayscalePixels(const unsigned char* pixels, int width, int height);
    
    void update();
    
    void draw();
    
    ofTexture* currentTresholdedTexture();
    
    void setAdaptiveThresholdRange(int aRange);
    
protected:
    int                     blobsMinArea;
    int                     blobsMaxArea;
    int                     blobsNumMax;
    int                     adaptiveThreshRange;
    
    bool                    hasNewData;
    
    ofxCvContourFinder      contourFinder;
    ofxBlobsManager         blobsManager;
    ofxCvGrayscaleImage     trackedImage;
};


#endif /* defined(__wiretouch_monitor__blob_tracker__) */
