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
    this->adaptiveThreshRange = 15;
    
    this->blobsMinArea = 1;
    this->blobsMaxArea = 10000;
    this->blobsNumMax = 50;
    
    this->blobsManager.normalizePercentage = .75;
    this->blobsManager.maxMergeDis         = 4;
    this->blobsManager.minDetectedTime     = 0;
    this->blobsManager.maxUndetectedTime   = 0;
    this->blobsManager.enableMinDetectedTimeFilter = false;
    this->blobsManager.giveLowestPossibleIDs = false;
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
    
    this->trackedImage.setUseTexture(true);
    this->trackedImage.setFromPixels(pixels, width, height);
    this->hasNewData = true;
}

void
wtmBlobTracker::update()
{
    if (this->hasNewData) {
        this->hasNewData = false;
        
        //this->trackedImage.threshold(this->threshold);
        this->trackedImage.adaptiveThreshold(this->adaptiveThreshRange, 128-this->threshold, false, false);
        this->trackedImage.updateTexture();
        
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
    ofSetColor(255, 0, 222, 127);
    ofSetLineWidth(2);
    
	int numBlobs = this->blobsManager.blobs.size();
	for( int i = 0; i < numBlobs; i++ )	{
		ofxStoredBlobVO& blob = this->blobsManager.blobs.at(i);
		vector <ofPoint>& pts = blob.pts;
        int nPts = blob.nPts;
        
        ofNoFill();
        ofBeginShape();
        for (int i = 0; i < nPts; i++){
            ofVertex(pts[i].x / scaleX, pts[i].y / scaleY);
        }
        ofEndShape(true);
		
        ofRectangle bbox = blob.boundingRect;
        
        ofRect(bbox.x / scaleX,
               bbox.y / scaleY,
               bbox.width / scaleX,
               bbox.height / scaleY);
        
		ofFill();
		ofCircle(blob.centroid.x / scaleX, blob.centroid.y / scaleY, 2);
        
		/*
         
         int x = blob.centroid.x / scaleX;
         int y = blob.centroid.y / scaleY;
         
         if(blob.id >= 10)
            x -= 4;
		
        ofPushStyle();
        ofDrawBitmapString(ofToString(blob.id),x-4,y+5);
        ofPopStyle();*/
	}
    
    ofDisableAlphaBlending();
    ofPopStyle();
}

ofTexture*
wtmBlobTracker::currentTresholdedTexture()
{
    return &this->trackedImage.getTextureReference();
}

void
wtmBlobTracker::setAdaptiveThresholdRange(int aRange)
{
    this->adaptiveThreshRange = aRange;
}
