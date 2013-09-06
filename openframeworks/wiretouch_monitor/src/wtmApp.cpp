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
 * along with rfm12b-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include "wtmApp.h"
#include "cJSON.h"

//--------------------------------------------------------------
void wtmApp::setup()
{
    ofSetVerticalSync(true);
	ofBackground(0);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    this->inputGamma = 1.0;
    
    this->sensorColumns = 32;
    this->sensorRows = 22;
    
    state = wtmAppStateIdle;
	
    this->bytesPerFrame = (sensorColumns*sensorRows*10)/8;
    this->recvBuffer = (unsigned char*)malloc(this->bytesPerFrame * sizeof(unsigned char));
    
    this->settingsString = NULL;
    
    this->capGridValues = (uint16_t*)malloc(sensorColumns * sensorRows * sizeof(uint16_t));
    
    this->interpolator = NULL;
    this->interpolatorType = wtmInterpolatorTypeCatmullRom;
    this->interpolatorUpsampleX = 8;
    this->interpolatorUpsampleY = 8;
    
    this->updateInterpolator();
    
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 300;
	bSerialConnectionAvailable = serial.setup(0, baud);
    // vector <ofSerialDeviceInfo> serialDevices = serial.getDeviceList();
    
    // setup the graphical user interface
    gui = new ofxUISuperCanvas("WIRETOUCH 0.2",WINDOWWIDTH-(GUIWIDTH+WINDOWBORDERDISTANCE),WINDOWBORDERDISTANCE, GUIWIDTH, GUIHEIGHT);
    ofxUILabel* fpsLabel = new ofxUILabel(kGUIFPS, OFX_UI_FONT_SMALL);
    gui->addWidgetPosition(fpsLabel, OFX_UI_WIDGET_POSITION_RIGHT, OFX_UI_ALIGN_RIGHT);
    this->updateFPSLabelWithValue(0.);
    gui->addSpacer();
    gui->addWidgetDown(new ofxUILabel("SENSOR PARAMETERS", OFX_UI_FONT_MEDIUM));
    gui->addSlider(kGUIHalfwaveAmpName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIOutputAmpName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUISampleDelayName, 0.0, 100.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUISignalFrequencyName, 1.0, 60.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSpacer();
    gui->addWidgetDown(new ofxUILabel("INTERPOLATION", OFX_UI_FONT_MEDIUM));
    // gui->addWidgetDown(new ofxUISpectrum(widgetWidth, widgetHeight, buffer, 256, 0.0, 1.0, "SPECTRUM"));
    vector<string> interpolationTypes;
    interpolationTypes.push_back(kGUILinearName);
    interpolationTypes.push_back(kGUICatmullName);
    interpolationTypes.push_back(kGUICosineName);
    interpolationTypes.push_back(kGUICubicName);
    interpolationTypes.push_back(kGUIHermiteName);
    interpolationTypes.push_back(kGUIWNNName);
    interpolationTypes.push_back(kGUILagrangeName);
    ofxUIDropDownList *interpolationDropDownMenu = gui->addDropDownList(kGUIInterpTypeName, interpolationTypes, (WIDGETWIDTH/2)-(OFX_UI_GLOBAL_WIDGET_SPACING));
    gui->addSlider(kGUIUpSamplingName, 1.0, 8.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSpacer();
    gui->addWidgetDown(new ofxUILabel("BLOB DETECTION", OFX_UI_FONT_MEDIUM));
    ofxUILabelToggle* toggle = gui->addLabelToggle(kGUIBlobsName, false, (WIDGETWIDTH/2)-OFX_UI_GLOBAL_WIDGET_SPACING, WIDGETHEIGHT);
    toggle->setLabelVisible(true); // doesn't get set by default!
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    // toggle = gui->addLabelToggle(kGUIGridName, false, (WIDGETWIDTH/2)-(OFX_UI_GLOBAL_WIDGET_SPACING/2), WIDGETHEIGHT);
    toggle->setLabelVisible(true);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider(kGUIBlobThresholdName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIBlobVisualizationName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIBlobGammaName, 0.0, 6.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(2);
    gui->addSlider(kGUIBlobAdaptiveThresholdRangeName, 0.0, 100.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(2);
    gui->addSpacer();
    
    ofxUILabel* firmwareLabel = new ofxUILabel(kGUIFirmwareName, OFX_UI_FONT_SMALL);
    gui->addWidgetDown(firmwareLabel);
    
    this->updateFirmwareVersionLabel("unknown");
    
    ofxUILabelButton* button = gui->addLabelButton(kGUICalibrateName, false, WIDGETWIDTH, WIDGETHEIGHT);
    button->setLabelVisible(true);
    button = gui->addLabelButton(kGUIStartName, false, WIDGETWIDTH, WIDGETHEIGHT);
    button->setLabelVisible(true);
    
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(120));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL, ofColor(255, 120)); // font color
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_OUTLINE_HIGHLIGHT, ofColor(0,0,255));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL_HIGHLIGHT, ofColor(239, 171, 233));
    gui->setColorBack(ofColor(100, 80));
    
    ofAddListener(gui->newGUIEvent, this, &wtmApp::guiEvent);

    gui->loadSettings("GUI/guiSettings.xml");
    interpolationDropDownMenu->setAutoClose(true);
    interpolationDropDownMenu->setShowCurrentSelected(true);
    
    this->tuioServer = new wtmTuioServer();
    this->tuioServer->start("127.0.0.1", 3333);
    
    this->lastWindowResizeTime = -1.0;
}

//--------------------------------------------------------------
void wtmApp::update()
{
    float now = ofGetElapsedTimef();
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
    
    switch (this->state) {
        case wtmAppStateReceivingSettings: {
            while(serial.available()) {
                if (NULL == this->settingsString)
                    this->settingsString = new string();
            
                int c = serial.readByte();
                
                *this->settingsString += (char)c;
                
                if ('\n' == c) {
                    if (this->resumeAfterSettingsReceipt) {
                        this->startSensor();
                        this->resumeAfterSettingsReceipt = false;
                    } else {
                        this->state = wtmAppStateIdle;
                    }
                    
                    ofxUILabelButton* button = (ofxUILabelButton*)gui->getWidget(kGUICalibrateName);
                    button->setLabelText(kGUICalibrateName);
                    
                    this->consumeSettings(this->settingsString->c_str());
                    
                    delete this->settingsString;
                    this->settingsString = NULL;
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
            }
            
            break;
        }
            
        case wtmAppStateIdle:
        default:
            this->drainSerial();
            break;
    }
    
    if (this->bTrackBlobs) {
        this->blobTracker.update();
        
        this->distributeTuio();
    }
}

void
wtmApp::distributeTuio()
{
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
void wtmApp::draw()
{    
    if (this->texture && this->texture->isAllocated())
        this->texture->draw(0, 0, ofGetWidth(), ofGetHeight());
    
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

    if (this->bTrackBlobs)
        this->blobTracker.draw();    
}

//--------------------------------------------------------------
void wtmApp::consumePacketData()
{
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
}

//--------------------------------------------------------------
void wtmApp::consumeSettings(const char* json)
{
    if (NULL != json) {
        cJSON* root = cJSON_Parse(json);
        
        if (NULL != root) {
            cJSON* version = cJSON_GetObjectItem(root, "version"), *cur;
            this->updateFirmwareVersionLabel(version->valuestring);
            
            cur = cJSON_GetObjectItem(root, "halfwave_amp");
            ofxUISlider* slider = (ofxUISlider*)gui->getWidget(kGUIHalfwaveAmpName);
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "output_amp");
            slider = (ofxUISlider*)gui->getWidget(kGUIOutputAmpName);
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "delay");
            slider = (ofxUISlider*)gui->getWidget(kGUISampleDelayName);
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "freq");
            slider = (ofxUISlider*)gui->getWidget(kGUISignalFrequencyName);
            slider->setValue(atoi(cur->valuestring));
            
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
void wtmApp::sendSliderData(ofxUIEventArgs &e, char command) {
    ofxUISlider *slider = (ofxUISlider *) e.widget;
    int val = round(slider->getScaledValue());
    slider->setValue(val);
    char buf[32];
    snprintf(buf, sizeof(buf), "%c%d\n", command, val);
    serial.writeBytes((unsigned char*)buf, strlen(buf));
}

void
wtmApp::startSensor()
{
    this->drainSerial();
    
    serial.writeBytes((unsigned char*)"s\n", 2);
    
    this->state = wtmAppStateReceivingTouches;
}

void
wtmApp::stopSensor()
{
    serial.writeBytes((unsigned char*)"x\n", 2);
    
    this->tuioServer->update();
    this->tuioServer->update();
    
    this->drainSerial();
    
    this->state = wtmAppStateIdle;
}

void
wtmApp::drainSerial()
{
    while (serial.available())
        (void)serial.readByte();
}

void
wtmApp::updateFPSLabelWithValue(float fps)
{
    ofxUILabel* fpsLabel = (ofxUILabel*)gui->getWidget(kGUIFPS);
    
    if (NULL != fpsLabel) {
        char buf[16];
        
        snprintf(buf, 16, "FPS: %.2f", fps);
        
        fpsLabel->setLabel(buf);
    }
}

void
wtmApp::updateInterpolationTypeLabel(const char* newName)
{
    ofxUIDropDownList* list = (ofxUIDropDownList*)gui->getWidget(kGUIInterpTypeName);
    if (NULL != list)
        list->setLabelText(newName);
}

void
wtmApp::updateFirmwareVersionLabel(const char* newVersion)
{
    ofxUILabel* firmwareLabel = (ofxUILabel*)gui->getWidget(kGUIFirmwareName);
    if (NULL != firmwareLabel) {
        char buf[64];
        snprintf(buf, 64, "FIRMWARE VERSION: %s", newVersion);
        
        firmwareLabel->setLabel(buf);
    }
}

//--------------------------------------------------------------
void wtmApp::guiEvent(ofxUIEventArgs &e)
{
    string widgetName = e.widget->getName();

	if (widgetName == kGUIHalfwaveAmpName) {
        sendSliderData(e, 'h');
    } else if (widgetName == kGUIOutputAmpName) {
        sendSliderData(e, 'o');
    } else if (widgetName == kGUISampleDelayName) {
        sendSliderData(e, 'd');
    } else if (widgetName == kGUIUpSamplingName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        this->interpolatorUpsampleX = val;
        this->interpolatorUpsampleY = val;
        this->updateInterpolator();
    } else if (widgetName == kGUISignalFrequencyName) {
        sendSliderData(e, 'f');
    } else if (widgetName == kGUILinearName) {
        this->interpolatorType = wtmInterpolatorTypeLinear;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUICatmullName) {
        this->interpolatorType = wtmInterpolatorTypeCatmullRom;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUICosineName) {
        this->interpolatorType = wtmInterpolatorTypeCosine;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUICubicName) {
        this->interpolatorType = wtmInterpolatorTypeCubic;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUIHermiteName) {
        this->interpolatorType = wtmInterpolatorTypeHermite;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUIWNNName) {
        this->interpolatorType = wtmInterpolatorTypeWNN;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUILagrangeName) {
        this->interpolatorType = wtmInterpolatorTypeLagrange;
        this->updateInterpolator();
        this->updateInterpolationTypeLabel(widgetName.c_str());
    } else if (widgetName == kGUIBlobsName) {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bTrackBlobs = button->getValue();
    } else if (widgetName == kGUIGridName) {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawGrid = button->getValue();
    } else if (widgetName == kGUIBlobThresholdName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        this->blobTracker.threshold = val;
    } else if (widgetName == kGUIBlobVisualizationName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        this->thresholdImageAlpha = val;
    } else if (widgetName == kGUIBlobAdaptiveThresholdRangeName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        
        double value = (slider->getScaledValue()/100.0) * this->interpolator->getOutputWidth();
        
        this->blobTracker.setAdaptiveThresholdRange(((int)value) | 1);
    } else if (widgetName == kGUIBlobGammaName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        this->inputGamma = slider->getScaledValue();
    } else if (widgetName == kGUICalibrateName) {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        
        if (wtmAppStateReceivingSettings != this->state) {            
            bool wasRunning = (wtmAppStateReceivingTouches == this->state);
            
            if (wasRunning)
                this->stopSensor();
            
            this->state = wtmAppStateReceivingSettings;
            serial.writeBytes((unsigned char*)"c\ni\n", 4);
            
            this->resumeAfterSettingsReceipt = wasRunning;
            
            button->setLabelText("CALIBRATING...");
        }
    } else if (widgetName == kGUIStartName) {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;

        if (button->getValue()) {
            if (wtmAppStateIdle == this->state) {
                button->setLabelText(kGUIStopName);
                
                if (wtmAppStateReceivingSettings == this->state)
                    this->resumeAfterSettingsReceipt = true;
                else
                    this->startSensor();
            } else {
                button->setLabelText(kGUIStartName);
                
                if (wtmAppStateReceivingSettings == this->state)
                    this->resumeAfterSettingsReceipt = false;
                else
                    this->stopSensor();
            }
        }
    }
}
//--------------------------------------------------------------
void wtmApp::keyReleased(int key){

}


//--------------------------------------------------------------
void wtmApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void wtmApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void wtmApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void wtmApp::mousePressed(int x, int y, int button){
    // TODO: close all pullDown menus when click happens somewhere outside the menus
    
}

//--------------------------------------------------------------
void wtmApp::mouseReleased(int x, int y, int button){
    // the gui title bar shouldn't leave the app window
    if (gui->getRect()->getY() < 0) {
        gui->getRect()->setY((0.0));
    }
}

//--------------------------------------------------------------
void wtmApp::windowResized(int w, int h){
    this->lastWindowResizeTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void wtmApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void wtmApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void wtmApp::exit()
{
    serial.close();
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}
