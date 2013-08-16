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
    
    void setGrayscalePixels(const unsigned char* pixels, int width, int height);
    
    void update();
    
    void draw();
    
protected:
    
    int                     threshold;
    int                     blobsMinArea;
    int                     blobsMaxArea;
    int                     blobsNumMax;
    
    bool                    hasNewData;
    
    ofxCvGrayscaleImage     trackedImage;
    ofxCvContourFinder      contourFinder;
    ofxBlobsManager         blobsManager;
};


#endif /* defined(__wiretouch_monitor__blob_tracker__) */
