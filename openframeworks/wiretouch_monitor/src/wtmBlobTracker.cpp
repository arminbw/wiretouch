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

#include "wtmBlobTracker.h"

wtmBlobTracker::wtmBlobTracker()
{
    this->threshold = 100;
    this->adaptiveThreshRange = 15;
    
    this->blobsMinArea = 0.1;
    this->blobsMaxArea = 3.0;
    this->blobsNumMax = 50;
    
    this->blobsManager.normalizePercentage = .5;
    this->blobsManager.maxMergeDis         = 16;
    this->blobsManager.minDetectedTime     = 0;
    this->blobsManager.maxUndetectedTime   = 0;
    this->blobsManager.enableMinDetectedTimeFilter = false;
    this->blobsManager.giveLowestPossibleIDs = true;
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
        
        int width = this->trackedImage.getWidth();
        
        this->contourFinder.findContours(this->trackedImage,
                                         this->blobsMinArea * width,
                                         this->blobsMaxArea * width,
                                         this->blobsNumMax,
                                         false);

        this->blobsManager.update(this->contourFinder.blobs);
        
        int numBlobs = this->blobsManager.blobs.size();
        for( int i = 0; i < numBlobs; i++ )	{
            ofxStoredBlobVO& blob = this->blobsManager.blobs.at(i);
            this->updateBlobCentroidFromImageIntensities(blob);
        }
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
         
        int x = blob.centroid.x / scaleX;
        int y = blob.centroid.y / scaleY;

        if(blob.id >= 10)
        x -= 4;

        ofPushStyle();
        ofDrawBitmapString(
            ofToString(blob.id),
            bbox.x / scaleX,
            (bbox.y + bbox.height) / scaleY + 12
        );
        ofPopStyle();
	}
    
    ofDisableAlphaBlending();
    ofPopStyle();
}

ofTexture*
wtmBlobTracker::currentTresholdedTexture()
{
    return &this->trackedImage.getTextureReference();
}

vector<ofxStoredBlobVO>&
wtmBlobTracker::currentBlobs()
{
    return this->blobsManager.blobs;
}

void
wtmBlobTracker::setAdaptiveThresholdRange(int aRange)
{
    this->adaptiveThreshRange = aRange;
}

void
wtmBlobTracker::updateBlobCentroidFromImageIntensities(ofxCvBlob& aBlob)
{
    int     x = floor(aBlob.boundingRect.getX()),
            y = floor(aBlob.boundingRect.getY()),
            w = floor(aBlob.boundingRect.getWidth()),
            h = floor(aBlob.boundingRect.getHeight());
    
    unsigned char* pixels = this->trackedImage.getPixels();
    int iw = this->trackedImage.getWidth();
    
    float accumX = 0, accumY = 0, wsum = 0.0;
    
    for (int i = x; i < x+w; i++) {
        for (int j = y; j < y+h; j++) {
            float weight = (float)(pixels[iw * j + i]) / 255.0;
            
            accumX += i * weight;
            accumY += j * weight;
            wsum   += weight;
        }
    }
    
    aBlob.centroid.set(accumX / wsum, accumY / wsum);
}
