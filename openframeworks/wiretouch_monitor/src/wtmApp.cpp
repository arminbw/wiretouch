/* WireTouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of WireTouch
 *
 * WireTouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WireTouch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WireTouch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "wtmApp.h"
#include "cJSON.h"

//--------------------------------------------------------------
void wtmApp::setup() {
    ofSetVerticalSync(true);
	ofBackground(0);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    this->signalWires = SIGNALWIRES;
    this->sensorWires = SENSORWIRES;
    
    this->inputGamma = 1; // TODO
    
    this->bytesPerFrame = (signalWires*sensorWires*10)/8;
    this->recvBuffer = (unsigned char*) malloc(this->bytesPerFrame * sizeof(unsigned char));
    this->calibrationResultsString.clear();
    this->capGridValues = (uint16_t*) malloc(signalWires * sensorWires * sizeof(uint16_t));
    
    this->interpolator = NULL;
    this->interpolatorType = wtmInterpolatorTypeCatmullRom;
    this->interpolatorUpsampleX = 8;
    this->interpolatorUpsampleY = 8;
    this->updateInterpolator();
    
    this->blobTracker = new wtmBlobTracker();

    this->state = wtmAppStateNoSerialConnection;
    this->lastWindowResizeTime = -1.0;
    this->serialOpenTime = 0;
    this->bNewDataToShow = false;
    
    // see wtmApp_Gui.cpp
    initGUI();
    loadSettings();
    
    ofLogNotice() << "starting up local TUIO Server.";
    this->tuioServer = new wtmTuioServer();
    this->tuioServer->start("127.0.0.1", 3333);     // TODO    
}

//--------------------------------------------------------------
void wtmApp::update() {
    float now = ofGetElapsedTimef();

    // window resizing
    if (this->lastWindowResizeTime > 0) {
        if ((now - this->lastWindowResizeTime) <.5)
            return;
        else if (now - this->lastWindowResizeTime > .5) {
            this->lastWindowResizeTime = -1.0;
            
            if (wtmAppStateReceivingTouches == this->state) {
                this->stopSensor();
                ofSleepMillis(100);
                this->startSensor();
            }
        }
    }
    
    // serial communication
    // the application has 5 different states
    // wtmAppStateNoSerialConnection:           The serial connection has not been initialized yet.
    // wtmAppStateStartWhenSerialInitalized:    The serial connection is opening. Changes into wtmAppStateReceivingTouches.
    // wtmAppStateReceivingTouches:             The application continously retrieves touch data.
    // wtmAppStateIdle:                         The user pressed STOP.
    // wtmAppStateReceivingSettings:            The user pressed CALIBRATE. Settings are now retrieved from the hardware.
    switch (this->state) {
        case wtmAppStateNoSerialConnection: {
            break;
        }
        case wtmAppStateStartWhenSerialInitalized: {
            if (serial.isInitialized()) {
                if (now > (this->serialOpenTime + 2)) { // let's give the serial connection some time (2 seconds)
                    ofLogNotice() << "init finished";
                    this->stopSensor(); // restart. just in case (the mainboard might already send data)
                    this->startSensor();
                }
            }
            break;
        }
        case wtmAppStateReceivingCalibrationResults: {
            while (serial.available()) {
                int c = serial.readByte();
                this->calibrationResultsString += (char)c;
                if ('\n' == c) {
                    ofxUILabelButton* button = (ofxUILabelButton*)gui->getWidget(kGUICalibrateName);
                    button->setLabelText(kGUICalibrateName);
                    
                    this->consumeCalibrationResults(this->calibrationResultsString.c_str());
                    this->calibrationResultsString.clear();
                    this->stopSensor();
                    this->startSensor();
                    break;
                }
            }
            break;
        }
        case wtmAppStateReceivingTouches: {
            if (serial.available() >= this->bytesPerFrame) {
                int len = serial.readBytes(this->recvBuffer, this->bytesPerFrame);
                
                if (len == this->bytesPerFrame) {
                    if (0 != this->lastRecvFrameTime) {
                        float now = ofGetElapsedTimef();
                        fpsCounter++;
                        if (now - this->lastRecvFrameTime > 1.0) {
                            this->updateFPSLabelWithValue(fpsCounter);
                            fpsCounter = 0;
                            this->lastRecvFrameTime = now;
                        }
                    } else
                        this->lastRecvFrameTime = ofGetElapsedTimef();
                    
                    this->consumePacketData();
                }
                if (this->bTrackBlobs) {
                    if (this->bNewDataToShow) {
                        this->blobTracker->update();
                        this->distributeTuio();
                        this->bNewDataToShow = false;
                    }
                }
            }
            break;
        }
        case wtmAppStateIdle:
        default:
            this->drainSerial();
            break;
    }
}

//--------------------------------------------------------------
void wtmApp::distributeTuio() {
    vector<ofxStoredBlobVO>& blobs = this->blobTracker->currentBlobs();
    
    float w = this->interpolator->getOutputWidth(), h = this->interpolator->getOutputHeight();
    int numBlobs = blobs.size();
	for(int i = 0; i < numBlobs; i++ )	{
		ofxStoredBlobVO& blob = blobs.at(i);
        
        this->tuioServer->registerCursorPosition(blob.id,
                                                 (float)blob.centroid.x / w,
                                                 (float)blob.centroid.y / h);
    }
    
    this->tuioServer->update();
}

//--------------------------------------------------------------
void wtmApp::draw() {
    if (this->texture && this->texture->isAllocated())
        this->texture->draw(0, 0, ofGetWidth(), ofGetHeight());

    if (this->bTrackBlobs) {
        if (wtmAppStateReceivingTouches == this->state && 0 < this->thresholdImageAlpha) {
            ofTexture* thresholdedTexture = this->blobTracker->currentTresholdedTexture();
            
            if (NULL != thresholdedTexture) {
                ofPushStyle();
                ofEnableAlphaBlending();
                ofSetColor(255, 0, 222, this->thresholdImageAlpha);
                
                thresholdedTexture->draw(0, 0, ofGetWidth(), ofGetHeight());
                
                ofDisableAlphaBlending();
                ofPopStyle();
            }
        }
        this->blobTracker->draw();
    }
}

//--------------------------------------------------------------
void wtmApp::consumePacketData() {
    unsigned char* b = this->recvBuffer;
    int bs = 0, br = 0, cnt = 0;
        // ofLogNotice() << "consuming packet:";
    
    for (int i=0; i<this->bytesPerFrame; i++) {
        br |= b[i] << bs;
        bs += 8;
        while (bs >= 10) {
            int px = cnt / this->sensorWires, py = cnt % this->sensorWires;
            
            int pix_val = br & 0x3ff;
            
            pix_val = pow((double)pix_val / 1023, this->inputGamma) * 1023;
            
            this->capGridValues[py * this->signalWires + px] = pix_val;
            
            br >>= 10;
            bs -= 10;
            cnt++;
        }
    }
    
    this->interpolator->runInterpolation(this->capGridValues);
    this->texture = this->interpolator->currentTexture();
    
    if (this->bTrackBlobs)
        this->blobTracker->setGrayscalePixels(this->interpolator->currentPixels(),
                                             this->interpolator->getOutputWidth(),
                                             this->interpolator->getOutputHeight());
    this->bNewDataToShow = true;
}

//--------------------------------------------------------------
void wtmApp::consumeCalibrationResults(const char* json) {
    ofLogNotice() << json;
    if (NULL != json) {
        cJSON* root = cJSON_Parse(json);
        ofLogNotice() << "parsing received settings";
        if (NULL != root) {
            cJSON* cur = cJSON_GetObjectItem(root, "version");
            if (cur != NULL) {
                this->updateFirmwareVersionLabel(cur->valuestring);
            }
            ofxUISlider* slider;
            cur = cJSON_GetObjectItem(root, "halfwave_amp");
            if (cur != NULL) {
                slider = (ofxUISlider*) gui->getWidget(kGUIHalfwaveAmpName);
                slider->setValue(atoi(cur->valuestring));
            }
            cur = cJSON_GetObjectItem(root, "output_amp");
            if (cur != NULL) {
                slider = (ofxUISlider*)gui->getWidget(kGUIOutputAmpName);
                slider->setValue(atoi(cur->valuestring));
            }
            cJSON_Delete(root);
        }
    }
}

//--------------------------------------------------------------
void wtmApp::updateInterpolator()
{
    if (NULL != this->interpolator) {
        delete this->interpolator;
        this->interpolator = NULL;
    }
    
    this->interpolator = wtmInterpolatorOfType(this->interpolatorType,
                                               this->signalWires,
                                               this->sensorWires,
                                               this->interpolatorUpsampleX,
                                               this->interpolatorUpsampleY);
}

//--------------------------------------------------------------
bool wtmApp::initAndStartSerialConnection(string serialDeviceName) {
    // serial.listDevices();
    if (serial.setup(serialDeviceName, BAUD)) {
        this->state = wtmAppStateStartWhenSerialInitalized;
        ofLogNotice() << "initalizing serial connection: " << serialDeviceName;
        this->serialOpenTime = ofGetElapsedTimef();
        return true;
    }
    ofLogError() << "can't open serial connection.";
    return false;
}

//--------------------------------------------------------------
void wtmApp::closeSerialConnection() {
    serial.close();
    this->state = wtmAppStateNoSerialConnection;
}

//--------------------------------------------------------------
void wtmApp::startSensor() {
    ofLogNotice() << "starting sensor";
    ofxUISlider *slider;
    sendSliderData((ofxUISlider *)  gui->getWidget(kGUIOutputAmpName));
    sendSliderData((ofxUISlider *)  gui->getWidget(kGUIHalfwaveAmpName));
    sendSliderData((ofxUISlider *)  gui->getWidget(kGUISampleDelayName));
    sendSliderData((ofxUISlider *)  gui->getWidget(kGUISignalFrequencyName));
    this->state = wtmAppStateReceivingTouches;
    serial.writeBytes((unsigned char*)"s\n", 2);
}

//--------------------------------------------------------------
void wtmApp::stopSensor() {
    ofLogNotice() << "stopping sensor";
    serial.writeBytes((unsigned char*)"x\n", 2); // mainboard flushes the remaining outgoing data
    this->drainSerial();
    this->state = wtmAppStateIdle;
}

//--------------------------------------------------------------
void wtmApp::startCalibration() {
    ofLogNotice() << "starting calibration";
    this->state = wtmAppStateReceivingCalibrationResults;
    serial.writeBytes((unsigned char*)"c\ni\n", 4);
    ofLogNotice() << "receiving calibration settings";
}

void wtmApp::resetCalibration() {
    ofLogNotice() << "reseting calibration";
    serial.writeBytes((unsigned char*)"r\n", 2);
}

//--------------------------------------------------------------
void wtmApp::sendSliderData(ofxUISlider* slider) {
   int val = round(slider->getValue());
   string widgetName = slider->getName();
   char command;
   if (widgetName == kGUIHalfwaveAmpName) command = 'h';
   if (widgetName == kGUIOutputAmpName) command = 'o';
   if (widgetName == kGUISampleDelayName) command = 'd';
   if (widgetName == kGUISignalFrequencyName) command = 'f';
   if (widgetName == kGUIResetName) command = 'r';
   char buf[32];
   snprintf(buf, sizeof(buf), "%c%d\n", command, val);
   serial.writeBytes((unsigned char*)buf, strlen(buf));
}

//--------------------------------------------------------------
void wtmApp::drainSerial() {
    while (serial.available()) int c = serial.readByte();
}

//--------------------------------------------------------------
void wtmApp::exit() {
    ofLogNotice() << "exiting";
    if (wtmAppStateNoSerialConnection != this->state) {
        if (wtmAppStateReceivingTouches == this->state) stopSensor();
        this->closeSerialConnection();
    }
    this->saveSettings();
    delete gui;
    ofSleepMillis(100);
}
