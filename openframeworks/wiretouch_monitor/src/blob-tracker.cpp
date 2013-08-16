//
//  blob-tracker.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 16/8/13.
//
//

#include "blob-tracker.h"

wtmBlobTracker::wtmBlobTracker()
{
    this->threshold = 100;
    
    this->blobsMinArea = 10;
    this->blobsMaxArea = 1000;
    this->blobsNumMax = 50;
    
    this->blobsManager.normalizePercentage = 0.7;
}

wtmBlobTracker::~wtmBlobTracker()
{
    this->trackedImage.clear();
}

void
wtmBlobTracker::setGrayscalePixels(const unsigned char* pixels, int width, int height)
{
    if (NULL != pixels && width > 0 && height > 0) {
        if (this->trackedImage.getWidth() != width || this->trackedImage.getHeight() != height) {
            this->trackedImage.clear();
            this->trackedImage.allocate(width, height);
        }
    }
    
    this->trackedImage.setFromPixels(pixels, width, height);
    this->hasNewData = true;
}

void
wtmBlobTracker::update()
{
    if (this->hasNewData) {
        this->hasNewData = false;
        
        this->trackedImage.threshold(this->threshold);
        
        this->contourFinder.findContours(this->trackedImage,
                                         this->blobsMinArea,
                                         this->blobsMaxArea,
                                         this->blobsNumMax,
                                         false);

        this->blobsManager.update(this->contourFinder.blobs);
    }
}

void
wtmBlobTracker::draw()
{
    float scaleX = float(this->trackedImage.getWidth())/float(ofGetWidth());
	float scaleY = float(this->trackedImage.getHeight())/float(ofGetHeight());
	
    ofPushStyle();
	ofEnableAlphaBlending();
    
	int numBlobs = this->blobsManager.blobs.size();
	for( int i = 0; i < numBlobs; i++ )	{
		ofxStoredBlobVO& blob = this->blobsManager.blobs.at(i);
		
		int x = blob.centroid.x / scaleX;
		int y = blob.centroid.y / scaleY;
		
		ofFill();
		ofSetColor(255,0,0,127);
		ofCircle(x, y, 10);
        
		if(blob.id >= 10)
            x -= 4;
		ofDrawBitmapString(ofToString(blob.id),x-4,y+5);
	}
    
    ofDisableAlphaBlending();
    ofPopStyle();
}
