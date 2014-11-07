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
    
    this->sensorColumns = 32;
    this->sensorRows = 22;
    
    this->inputGamma = 1;
    
    
    this->bytesPerFrame = (sensorColumns*sensorRows*10)/8;
    this->recvBuffer = (unsigned char*)malloc(this->bytesPerFrame * sizeof(unsigned char));
    this->settingsString = NULL;
    this->capGridValues = (uint16_t*)malloc(sensorColumns * sensorRows * sizeof(uint16_t));
    
    this->interpolator = NULL;
    this->interpolatorType = wtmInterpolatorTypeCatmullRom;
    this->interpolatorUpsampleX = 8;
    this->interpolatorUpsampleY = 8;
    this->updateInterpolator();

    initGUI(); // see wtmApp_Gui.cpp
    
    gui->setTriggerWidgetsUponLoad(false); // TODO
    // gui->loadSettings("GUI/guiSettings.xml");   // TODO: no serial here yet!

    this->state = wtmAppStateIdle;
    this->bSerialUpdated = false;
    bSerialConnectionAvailable = false; // TODO
    bSerialConnectionConfigured = false;
    this->serialOpenTime = ofGetElapsedTimef();
    
    cout << "starting up local TUIO Server." << endl;
    this->tuioServer = new wtmTuioServer();
    this->tuioServer->start("127.0.0.1", 3333);
    
    this->lastWindowResizeTime = -1.0;
    
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
    // the application has 4 different states
    // wtmAppStateNoSerialConnection:   The serial connection has not been initialized yet.
    // wtmAppStateReceivingTouches:     The user pressed START. The application continously retrieves data.
    // wtmAppStateIdle:                 The user pressed STOP.
    // wtmAppStateReceivingSettings:    The user pressed CALIBRATE. Settings are now retrieved from the hardware.
    switch (this->state) {
        case wtmAppStateNoSerialConnection: {
            break;
        }
        case wtmAppStateReceivingSettings: {
            while(serial.available()) {
                if (NULL == this->settingsString) this->settingsString = new string();
                int c = serial.readByte();
                
                *this->settingsString += (char)c;
                
                if ('\n' == c) {
                    ofxUILabelButton* button = (ofxUILabelButton*)gui->getWidget(kGUICalibrateName);
                    button->setLabelText(kGUICalibrateName);
                    
                    this->consumeSettings(this->settingsString->c_str());
                    
                    delete this->settingsString;
                    this->settingsString = NULL;
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
                        
                        this->updateFPSLabelWithValue(1.0 / (now - this->lastRecvFrameTime));
                        
                        this->lastRecvFrameTime = now;
                    } else
                        this->lastRecvFrameTime = ofGetElapsedTimef();
                    
                    this->consumePacketData();
                }
                if (this->bTrackBlobs) {
                    if (this->bSerialUpdated) {
                        this->blobTracker.update();
                        this->distributeTuio();
                        this->bSerialUpdated = false;
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
    vector<ofxStoredBlobVO>& blobs = this->blobTracker.currentBlobs();
    
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
            ofTexture* thresholdedTexture = this->blobTracker.currentTresholdedTexture();
            
            if (NULL != thresholdedTexture) {
                ofPushStyle();
                ofEnableAlphaBlending();
                ofSetColor(255, 0, 222, this->thresholdImageAlpha);
                
                thresholdedTexture->draw(0, 0, ofGetWidth(), ofGetHeight());
                
                ofDisableAlphaBlending();
                ofPopStyle();
            }
        }
        this->blobTracker.draw();
    }
}

//--------------------------------------------------------------
void wtmApp::consumePacketData() {
    unsigned char* b = this->recvBuffer;
    int bs = 0, br = 0, cnt = 0;
    
    for (int i=0; i<this->bytesPerFrame; i++) {
        br |= b[i] << bs;
        bs += 8;
        while (bs >= 10) {
            int px = cnt / this->sensorRows, py = cnt % this->sensorRows;
            
            int pix_val = br & 0x3ff;
            
            pix_val = pow((double)pix_val / 1023, this->inputGamma) * 1023;
            
            this->capGridValues[py * this->sensorColumns + px] = pix_val;
            
            br >>= 10;
            bs -= 10;
            cnt++;
        }
    }
    
    this->interpolator->runInterpolation(this->capGridValues);
    this->texture = this->interpolator->currentTexture();
    
    if (this->bTrackBlobs)
        this->blobTracker.setGrayscalePixels(this->interpolator->currentPixels(),
                                             this->interpolator->getOutputWidth(),
                                             this->interpolator->getOutputHeight());
    this->bSerialUpdated = true;
}

//--------------------------------------------------------------
void wtmApp::consumeSettings(const char* json) {
    if (NULL != json) {
        cout << json << endl;
        cJSON* root = cJSON_Parse(json);
        cout << "parsing received settings" << endl;
        if (NULL != root) {
            cJSON* version = cJSON_GetObjectItem(root, "version"), *cur;
            this->updateFirmwareVersionLabel(version->valuestring);
            
            cur = cJSON_GetObjectItem(root, "halfwave_amp");
            ofxUISlider* slider = (ofxUISlider*)gui->getWidget(kGUIHalfwaveAmpName);
            slider->setValue(atoi(cur->valuestring));
            cout << "halfwave: " << cur->valuestring << endl;
            
            cur = cJSON_GetObjectItem(root, "output_amp");
            slider = (ofxUISlider*)gui->getWidget(kGUIOutputAmpName);
            slider->setValue(atoi(cur->valuestring));
            cout << "output: " << cur->valuestring << endl;
            
            cur = cJSON_GetObjectItem(root, "delay");
            slider = (ofxUISlider*)gui->getWidget(kGUISampleDelayName);
            slider->setValue(atoi(cur->valuestring));
            cout << "delay: " << cur->valuestring << endl;
            
            cur = cJSON_GetObjectItem(root, "freq");
            slider = (ofxUISlider*)gui->getWidget(kGUISignalFrequencyName);
            slider->setValue(atoi(cur->valuestring));
            cout << "freq: " << cur->valuestring << endl;
            
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
                                               this->sensorColumns,
                                               this->sensorRows,
                                               this->interpolatorUpsampleX,
                                               this->interpolatorUpsampleY);
}

//--------------------------------------------------------------
bool wtmApp::initSerialConnection(string serialDeviceName) {
    // TODO
    cout << "initalizing serial connection: " << serialDeviceName << endl;
    int baud = 300;
    serial.listDevices();
    if (serial.setup(serialDeviceName, baud)) {
        this->state = wtmAppStateReceivingSettings;
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void wtmApp::closeSerialConnection() {
    serial.close();
    this->state = wtmAppStateNoSerialConnection;
}

//--------------------------------------------------------------
void wtmApp::startSensor() {
    cout << "starting sensor" << endl;
    this->state = wtmAppStateReceivingTouches;
    this->drainSerial();
    serial.writeBytes((unsigned char*)"s\n", 2);
    sendSliderData((ofxUISlider *) gui->getWidget(kGUIHalfwaveAmpName));
    sendSliderData((ofxUISlider *) gui->getWidget(kGUIOutputAmpName));
    sendSliderData((ofxUISlider *) gui->getWidget(kGUISampleDelayName));
    sendSliderData((ofxUISlider *) gui->getWidget(kGUISignalFrequencyName));
}

//--------------------------------------------------------------
void wtmApp::stopSensor() {
    cout << "stopping sensor" << endl;
    this->state = wtmAppStateIdle;
    serial.writeBytes((unsigned char*)"x\n", 2);
    
    this->tuioServer->update();
    this->tuioServer->update(); // TODO: ???
    
    this->drainSerial();
}

//--------------------------------------------------------------
void wtmApp::startCalibration() {
    cout << "starting calibration" << endl;
    this->stopSensor();
    serial.writeBytes((unsigned char*)"c\n", 2);
    this->receiveSettings();
}

//--------------------------------------------------------------
void wtmApp::receiveSettings() {
    cout << "receiving settings" << endl;
    this->drainSerial();
    this->state = wtmAppStateReceivingSettings;
    serial.writeBytes((unsigned char*)"i\n", 2);
}

//--------------------------------------------------------------
void wtmApp::sendSliderData(ofxUISlider* slider) {
    if (this->state == wtmAppStateReceivingTouches) {
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        string widgetName = slider->getName();
        char command;
        if (widgetName == kGUIHalfwaveAmpName) command = 'h';
        if (widgetName == kGUIOutputAmpName) command = 'o';
        if (widgetName == kGUISampleDelayName) command = 'd';
        if (widgetName == kGUISignalFrequencyName) command = 'f';
        char buf[32];
        snprintf(buf, sizeof(buf), "%c%d\n", command, val);
        serial.writeBytes((unsigned char*)buf, strlen(buf));
    }
}

//--------------------------------------------------------------
// TODO: flush?
void wtmApp::drainSerial() {
    if (serial.isInitialized()) {
        while (serial.available()) (void)serial.readByte();
    }
}

//--------------------------------------------------------------
void wtmApp::updateFirmwareVersionLabel(const char* newVersion) {
    ofxUILabel* firmwareLabel = (ofxUILabel*)gui->getWidget(kGUIFirmwareName);
    if (NULL != firmwareLabel) {
        char buf[64];
        snprintf(buf, 64, "FIRMWARE VERSION: %s", newVersion);
        
        firmwareLabel->setLabel(buf);
    }
}

//--------------------------------------------------------------
void wtmApp::exit() {
    this->closeSerialConnection();
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    ofSleepMillis(100);
}
