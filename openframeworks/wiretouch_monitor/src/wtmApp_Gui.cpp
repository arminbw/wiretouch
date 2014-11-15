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
void wtmApp::initGUI() {
    // setup the graphical user interface
    gui = new ofxUISuperCanvas(WIRETOUCHVERSION, WINDOWWIDTH-(GUIWIDTH+WINDOWBORDERDISTANCE), WINDOWBORDERDISTANCE, GUIWIDTH, GUIHEIGHT);
    gui->addSpacer();
    
    gui->addWidgetDown(new ofxUILabel("SENSOR PARAMETERS", OFX_UI_FONT_MEDIUM));
    gui->addSlider(kGUIHalfwaveAmpName, 1.0, 255.0, 164, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIOutputAmpName, 1.0, 255.0, 111, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUISampleDelayName, 1.0, 100.0, 1, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    ofxUISlider* slider = gui->addSlider(kGUISignalFrequencyName, 1.0, 60.0, 20, WIDGETWIDTH, WIDGETHEIGHT);
    slider->setLabelPrecision(2);
    this->updateFrequencyLabel();
    gui->addSpacer();
    
    gui->addWidgetDown(new ofxUILabel("INTERPOLATION", OFX_UI_FONT_MEDIUM));
    vector<string> interpolationTypes;
    interpolationTypes.push_back(kGUILinearName);
    interpolationTypes.push_back(kGUICatmullName);
    interpolationTypes.push_back(kGUICosineName);
    interpolationTypes.push_back(kGUICubicName);
    interpolationTypes.push_back(kGUIHermiteName);
    interpolationTypes.push_back(kGUIWNNName);
    interpolationTypes.push_back(kGUILagrangeName);
    ofxUIDropDownList *interpolationDropDownMenu = gui->addDropDownList(kGUIInterpolationDropDownName, interpolationTypes, WIDGETWIDTH);
    interpolationDropDownMenu->setAutoClose(true);
    interpolationDropDownMenu->getSelected().push_back(gui->getWidget(kGUICatmullName));
    interpolationDropDownMenu->setShowCurrentSelected(true);
    gui->addSlider(kGUIUpSamplingName, 1.0, 12.0, 8, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSpacer();
    
    gui->addWidgetDown(new ofxUILabel("BLOB DETECTION", OFX_UI_FONT_MEDIUM));
    ofxUILabelToggle* toggle = gui->addLabelToggle(kGUIBlobsName, false, (WIDGETWIDTH/2)-OFX_UI_GLOBAL_WIDGET_SPACING, WIDGETHEIGHT);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    // TODO: Grid
    // toggle = gui->addLabelToggle(kGUIGridName, false, (WIDGETWIDTH/2)-(OFX_UI_GLOBAL_WIDGET_SPACING/2), WIDGETHEIGHT);
    // toggle->setLabelVisible(true);
    
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider(kGUIBlobThresholdName, 0.0, 255.0, 161, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    this->blobTracker->setThreshold(161);
    gui->addSlider(kGUIBlobVisualizationName, 0.0, 255.0, 31, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIBlobGammaName, 0.0, 16.0, this->inputGamma, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(2);
    gui->addSlider(kGUIBlobAdaptiveThresholdRangeName, 3.0, 100.0, 41, WIDGETWIDTH, WIDGETHEIGHT);
    this->blobTracker->setAdaptiveThresholdRange(((int) 41) | 1);
    gui->addSpacer();
    
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    for(int i=0; i<deviceList.size();i++) {
        this->serialDevicesNames.push_back(deviceList[i].getDeviceName().c_str());
    }
    ofxUIDropDownList *serialDeviceDropDownMenu = gui->addDropDownList(kGUISerialDropDownName, serialDevicesNames, WIDGETWIDTH);
    serialDeviceDropDownMenu->setShowCurrentSelected(true);
    serialDeviceDropDownMenu->setAutoClose(true);
    
    ofxUILabelButton* button = gui->addLabelButton(kGUIStartName, false, WIDGETWIDTH, WIDGETHEIGHT);
    button->setLabelVisible(true);
    
    button = gui->addLabelButton(kGUICalibrateName, false, WIDGETWIDTH, WIDGETHEIGHT);
    button->setLabelVisible(true);
    
    gui->addSpacer();

    ofxUILabel* firmwareLabel = new ofxUILabel(kGUIFirmwareName, OFX_UI_FONT_SMALL);
    gui->addWidgetDown(firmwareLabel);
    this->updateFirmwareVersionLabel("unknown");

    ofxUILabel* fpsLabel = new ofxUILabel(kGUIFPS, OFX_UI_FONT_SMALL);
    gui->addWidgetDown(fpsLabel);
    this->updateFPSLabelWithValue(0.);
    
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(120));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL, ofColor(255, 120)); // font color
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_OUTLINE_HIGHLIGHT, ofColor(0,0,255));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL_HIGHLIGHT, ofColor(239, 171, 233));
    gui->setColorBack(ofColor(100, 80));
    ofAddListener(gui->newGUIEvent, this, &wtmApp::guiEvent);
}

//--------------------------------------------------------------
void wtmApp::saveSettings() {
    cout << "saving settings" << endl;
    cJSON *root = cJSON_CreateObject();
    const char* sliderNames[9] = {
        kGUIHalfwaveAmpName, kGUIOutputAmpName, kGUISampleDelayName, kGUISignalFrequencyName, kGUIUpSamplingName,
        kGUIBlobThresholdName, kGUIBlobVisualizationName, kGUIBlobGammaName, kGUIBlobAdaptiveThresholdRangeName
    };
    for (int i=0; i < 8; i++) {
        cJSON_AddNumberToObject(root, sliderNames[i], ((ofxUISlider *) gui->getWidget(sliderNames[i]))->getValue() );
    }
    
    ofxUIDropDownList *interpolationDropDown = (ofxUIDropDownList *) gui->getWidget(kGUIInterpolationDropDownName);
    if (!(vector<string> *) interpolationDropDown->getSelectedNames().empty())
        cJSON_AddStringToObject(root, kGUIInterpolationDropDownName, interpolationDropDown->getSelectedNames()[0].c_str() );
    ofxUIDropDownList *serialDropDown = (ofxUIDropDownList *) gui->getWidget(kGUISerialDropDownName);
    if (!(vector<string> *) serialDropDown->getSelectedNames().empty())
        cJSON_AddStringToObject(root, kGUISerialDropDownName, serialDropDown->getSelectedNames()[0].c_str() );
    
    cJSON_AddNumberToObject(root, kGUIBlobsName, this->bTrackBlobs );
    // cJSON_AddNumberToObject(root, kGUIGridName, this->bDrawGrid ); // TODO
    
    char *renderedJson = cJSON_Print(root);
    ofFile file;
    file.open(ofToDataPath("settings.json"), ofFile::WriteOnly, false);
    file << renderedJson;
    file.close();
    cJSON_Delete(root);
    delete(renderedJson);
}

//--------------------------------------------------------------
void wtmApp::loadSettings() {
    cout << "loading settings" << endl;
    ofFile file;
    if (file.open(ofToDataPath("settings.json"), ofFile::ReadOnly, false) == false) return;
    ofBuffer buffer = file.readToBuffer();
    cJSON *root = cJSON_Parse((buffer.getText()).c_str());
    
    const char* sliderNames[9] = {
        kGUIHalfwaveAmpName, kGUIOutputAmpName, kGUISampleDelayName, kGUISignalFrequencyName, kGUIUpSamplingName,
        kGUIBlobThresholdName, kGUIBlobVisualizationName, kGUIBlobGammaName, kGUIBlobAdaptiveThresholdRangeName
    };
    for (int i=0; i < 8; i++) {
        if (cJSON_GetObjectItem(root, sliderNames[i]) != NULL) {
            ((ofxUISlider *) gui->getWidget(sliderNames[i]))->setValue(cJSON_GetObjectItem(root, sliderNames[i])->valueint);
        }
    }
    this->updateFrequencyLabel();
    this->updateInterpolator();
    
    if (cJSON_GetObjectItem(root, kGUIInterpolationDropDownName) != NULL)
        gui->triggerEvent(gui->getWidget(cJSON_GetObjectItem(root, kGUIInterpolationDropDownName)->valuestring));
    if (cJSON_GetObjectItem(root, kGUISerialDropDownName) != NULL) {
        string selectedDeviceName = (char *) cJSON_GetObjectItem(root, kGUISerialDropDownName)->valuestring;
        ofxUIDropDownList *serialDropDown = (ofxUIDropDownList *) gui->getWidget(kGUISerialDropDownName);
        for (int i=0; i<this->serialDevicesNames.size(); ++i) {
            if (selectedDeviceName == serialDevicesNames[i]) {
                moveWidgetsBeneathDropdown(serialDropDown, false); // TODO: refactor
                serialDropDown->triggerEvent(gui->getWidget(cJSON_GetObjectItem(root, kGUISerialDropDownName)->valuestring));
            }
        }
    }
    if (cJSON_GetObjectItem(root, kGUIBlobsName) != NULL)
        ((ofxUILabelToggle *) gui->getWidget(kGUIBlobsName))->setValue(cJSON_GetObjectItem(root, kGUIBlobGammaName)->valueint);
    
    file.close();
    cJSON_Delete(root);
}

//--------------------------------------------------------------
void wtmApp::updateFPSLabelWithValue(float fps) {
    ofxUILabel* fpsLabel = (ofxUILabel*) gui->getWidget(kGUIFPS);
    if (NULL != fpsLabel) {
        char buf[16];
        snprintf(buf, 16, "FPS: %.2f", fps);
        fpsLabel->setLabel(buf);
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
void wtmApp::updateFrequencyLabel() {
    ofxUISlider* slider = (ofxUISlider*) gui->getWidget(kGUISignalFrequencyName);
    slider->getLabelWidget()->setLabel(slider->getName() + ": ~" + ofxUIToString(16000000./(round(slider->getValue())+1.)/2000., 0) + " kHz");
}

//--------------------------------------------------------------
// The current implementation of ofxUIDropDownList is problematic
// as it triggers events of widgets positioned underneath the dropdown menu.
// This workaround changes the dropdown menu into a "disclosure menu".
//
void wtmApp::moveWidgetsBeneathDropdown(ofxUIDropDownList* widget, bool moveBack) {
    // calculate the size of the toggles
    float summand = 0;
    vector<ofxUILabelToggle*> toggles = widget->getToggles();
    for (vector<ofxUILabelToggle*>::iterator it = toggles.begin(); it != toggles.end(); ++it) {
        ofxUIRectangle* r = (*it)->getRect();
        summand += r->getHeight();
    }
    if (moveBack) summand = -summand;
    // now change the position of every widget beneath
    vector<ofxUIWidget*> widgets = gui->getWidgets();
    for (vector<ofxUIWidget*>::reverse_iterator rit = widgets.rbegin(); rit != widgets.rend(); ++rit) {
        if (*rit == widget) break; // everything above the widget won't get changed
        if ((*rit)->getParent() == gui) { // only top-level widgets get changed
            ofxUIRectangle* r = (*rit)->getRect();
            r->setY(r->getY(false)+summand);
        }
    }
    gui->setHeight(gui->getRect()->getHeight()+summand);
}

//--------------------------------------------------------------
void wtmApp::guiEvent(ofxUIEventArgs &e) {
    string widgetName = e.widget->getName();
    // cout << "widget activated: " << widgetName << endl;

    // interpolation type disclosure menu
    ofxUIDropDownList *dropDownlist = (ofxUIDropDownList *) gui->getWidget(kGUIInterpolationDropDownName);
    if (widgetName == kGUIInterpolationDropDownName) {
        moveWidgetsBeneathDropdown(dropDownlist, !dropDownlist->isOpen());
        return;
    }
    if (e.widget->getParent() == dropDownlist) {
        if (widgetName == kGUILinearName) this->interpolatorType = wtmInterpolatorTypeLinear;
        if (widgetName == kGUICatmullName) this->interpolatorType = wtmInterpolatorTypeCatmullRom;
        if (widgetName == kGUICosineName) this->interpolatorType = wtmInterpolatorTypeCosine;
        if (widgetName == kGUICubicName) this->interpolatorType = wtmInterpolatorTypeCubic;
        if (widgetName == kGUIHermiteName) this->interpolatorType = wtmInterpolatorTypeHermite;
        if (widgetName == kGUIWNNName) this->interpolatorType = wtmInterpolatorTypeWNN;
        if (widgetName == kGUILagrangeName) this->interpolatorType = wtmInterpolatorTypeLagrange;
        this->updateInterpolator();
        dropDownlist->setLabelText(widgetName.c_str());
        return;
    }
    
    // serial connection disclosure menu
    dropDownlist = (ofxUIDropDownList *) gui->getWidget(kGUISerialDropDownName);
    if (widgetName == kGUISerialDropDownName) {
        moveWidgetsBeneathDropdown(dropDownlist, !dropDownlist->isOpen());
        return;
    }
    if (e.widget->getParent() == dropDownlist) {
        if (serial.isInitialized()) {
            cout << "serial connection changed" << endl;
            if (this->state == wtmAppStateReceivingTouches) {
                stopSensor();
                ofxUILabelButton *startButton = (ofxUILabelButton *) gui->getWidget(kGUIStartName);
            }
            closeSerialConnection();
        }
        return;
    }
    
    // push buttons
    if (e.widget->getKind() == OFX_UI_WIDGET_LABELBUTTON) {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        if (button->getValue() == 1) { // we only use button-released, not button-pressed and not both
            /* TODO
            if (widgetName == kGUIGridName) {
                bDrawGrid = button->getValue();
                return;
            }*/
            if (widgetName == kGUIStartName) {
                cout << "start/stop button pressed" << endl;
                if (wtmAppStateNoSerialConnection == this->state) {
                    vector<string> selected = dropDownlist->getSelectedNames();
                    if (selected.empty()) {
                        cout << "no serial connection selected" << endl;
                    } else {
                        initAndStartSerialConnection(selected[0]);
                        button->setLabelText(kGUIStopName);
                    }
                    return;
                }
                if (wtmAppStateIdle == this->state) {
                    this->startSensor();
                    button->setLabelText(kGUIStopName);
                    return;
                }
                if (wtmAppStateReceivingTouches == this->state) {
                    this->stopSensor();
                    button->setLabelText(kGUIStartName);
                    return;
                }
                return;
            }
            if (widgetName == kGUICalibrateName) {
                if (wtmAppStateReceivingTouches == this->state) {
                    cout << "calibration button pressed" << endl;
                    this->stopSensor(); // TODO: serial buffer issue
                    ofSleepMillis(100); // this is a
                    this->stopSensor(); // lousy workaround
                    this->startCalibration();
                    button->setLabelText("CALIBRATING...");
                }
            }
        }
    }
    
    // toggle buttons
    if (e.widget->getKind() == OFX_UI_WIDGET_LABELTOGGLE) {
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        if (widgetName == kGUIBlobsName) {
            cout << toggle->getValue() << endl;
            bTrackBlobs = toggle->getValue();
            return;
        }
    }
    
    // sliders
    if (e.widget->getKind() == OFX_UI_WIDGET_SLIDER_H) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        float val = slider->getValue();
        if (widgetName == kGUIHalfwaveAmpName) {
            slider->setValue(round(val));
            sendSliderData(slider);
        } else if (widgetName == kGUIOutputAmpName) {
            slider->setValue(round(val));
            sendSliderData(slider);
        } else if (widgetName == kGUISampleDelayName) {
            slider->setValue(round(val));
            sendSliderData(slider);
        } else if (widgetName == kGUIUpSamplingName) {
            slider->setValue(round(val));
            this->interpolatorUpsampleX = val;
            this->interpolatorUpsampleY = val;
            this->updateInterpolator();
        } else if (widgetName == kGUISignalFrequencyName) {
            slider->setValue(round(val));
            this->updateFrequencyLabel();
            sendSliderData(slider);
        } else if (widgetName == kGUIBlobThresholdName) {
            slider->setValue(round(val));
            this->blobTracker->setThreshold(round(val));
        } else if (widgetName == kGUIBlobVisualizationName) {
            slider->setValue(round(val));
            this->thresholdImageAlpha = round(val);
        } else if (widgetName == kGUIBlobAdaptiveThresholdRangeName) {
            double value = (slider->getValue()/100.0) * this->interpolator->getOutputWidth();
            this->blobTracker->setAdaptiveThresholdRange(((int) value) | 1);
        } else if (widgetName == kGUIBlobGammaName) {
            this->inputGamma = slider->getValue();
        }
    }
}

//--------------------------------------------------------------
void wtmApp::keyReleased(int key) {
    
}

//--------------------------------------------------------------
void wtmApp::keyPressed(int key) {
    
}

//--------------------------------------------------------------
void wtmApp::mouseMoved(int x, int y ) {
    
}

//--------------------------------------------------------------
void wtmApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void wtmApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void wtmApp::mouseReleased(int x, int y, int button) {
    // the gui title bar shouldn't leave the app window
    if (gui->getRect()->getY() < 0) {
        gui->getRect()->setY((0.0));
    }
}

//--------------------------------------------------------------
void wtmApp::windowResized(int w, int h) {
    this->lastWindowResizeTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void wtmApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void wtmApp::dragEvent(ofDragInfo dragInfo) {
    
}
