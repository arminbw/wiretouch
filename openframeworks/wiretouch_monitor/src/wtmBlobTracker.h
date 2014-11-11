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

#ifndef __wiretouch_monitor__blob_tracker__
#define __wiretouch_monitor__blob_tracker__

#include "ofMain.h"
#include "ofxBlobsManager.h"

class wtmBlobTracker {
public:
    wtmBlobTracker();
    ~wtmBlobTracker();
    
    int threshold;
    
    void setGrayscalePixels(const unsigned char* pixels, int width, int height);
    
    void update();
    
    void draw();
    
    ofTexture* currentTresholdedTexture();
    
    void setAdaptiveThresholdRange(int aRange);
    void setThreshold(int threshold);
    
    vector<ofxStoredBlobVO>& currentBlobs();
    
protected:
    float                   blobsMinArea;
    float                   blobsMaxArea;
    int                     blobsNumMax;
    int                     adaptiveThreshRange;
    
    bool                    hasNewData;
    
    ofxCvContourFinder      contourFinder;
    ofxBlobsManager         blobsManager;
    ofxCvGrayscaleImage     trackedImage;
    
    void updateBlobCentroidFromImageIntensities(ofxCvBlob& aBlob);
};


#endif /* defined(__wiretouch_monitor__blob_tracker__) */
