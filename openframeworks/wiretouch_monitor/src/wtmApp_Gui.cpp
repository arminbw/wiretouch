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

//--------------------------------------------------------------
void wtmApp::initGUI() {
    // setup the graphical user interface
    
    gui = new ofxUISuperCanvas(WIRETOUCHVERSION, WINDOWWIDTH-(GUIWIDTH+WINDOWBORDERDISTANCE), WINDOWBORDERDISTANCE, GUIWIDTH, GUIHEIGHT);
    
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
    interpolationDropDownMenu->setShowCurrentSelected(true);
    
    gui->addSlider(kGUIUpSamplingName, 1.0, 12.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSpacer();
    
    gui->addWidgetDown(new ofxUILabel("BLOB DETECTION", OFX_UI_FONT_MEDIUM));
    ofxUILabelToggle* toggle = gui->addLabelToggle(kGUIBlobsName, false, (WIDGETWIDTH/2)-OFX_UI_GLOBAL_WIDGET_SPACING, WIDGETHEIGHT);
    toggle->setLabelVisible(true); // doesn't get set by default! TODO
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    // toggle = gui->addLabelToggle(kGUIGridName, false, (WIDGETWIDTH/2)-(OFX_UI_GLOBAL_WIDGET_SPACING/2), WIDGETHEIGHT);
    // toggle->setLabelVisible(true);
    
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider(kGUIBlobThresholdName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIBlobVisualizationName, 0.0, 255.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(0);
    gui->addSlider(kGUIBlobGammaName, 0.0, 16.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(2);
    gui->addSlider(kGUIBlobAdaptiveThresholdRangeName, 0.0, 100.0, 50, WIDGETWIDTH, WIDGETHEIGHT)->setLabelPrecision(2);
    gui->addSpacer();
    
    ofxUILabel* firmwareLabel = new ofxUILabel(kGUIFirmwareName, OFX_UI_FONT_SMALL);
    gui->addWidgetDown(firmwareLabel);
    this->updateFirmwareVersionLabel("unknown");
    
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    for(int i=0; i<deviceList.size();i++) {
        this->serialDevicesNames.push_back(deviceList[i].getDeviceName().c_str());
        cout << serialDevicesNames.at(i)<<endl;
    }
    ofxUIDropDownList *serialDeviceDropDownMenu = gui->addDropDownList(kGUISerialDropDownName, serialDevicesNames, WIDGETWIDTH);
    serialDeviceDropDownMenu->setShowCurrentSelected(true);
    serialDeviceDropDownMenu->setAutoClose(true);
       
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
}

//--------------------------------------------------------------
void wtmApp::updateFPSLabelWithValue(float fps) {
    ofxUILabel* fpsLabel = (ofxUILabel*)gui->getWidget(kGUIFPS);
    
    if (NULL != fpsLabel) {
        char buf[16];
        
        snprintf(buf, 16, "FPS: %.2f", fps);
        
        fpsLabel->setLabel(buf);
    }
}

//--------------------------------------------------------------
// The current implementation of ofxUIDropDownList is problematic
// as it triggers events of widgets positioned underneath the dropdown menu.
// This workaround changes the dropdown menu into a "disclosure menu".
void wtmApp::moveWidgetsBeneathDropdown(ofxUIDropDownList* widget, bool moveBack)
{
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
    cout << "widget activated: " << widgetName << endl;

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
        // TODO: changing this should stop the serial connection
        if (widgetName != dropDownlist->getSelectedNames()[0]) {
            cout << "stopping serial (TODO)" << endl;
        }
        return;
    }
    
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
     } else if (widgetName == kGUIBlobsName) {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bTrackBlobs = button->getValue();
    } else if (widgetName == kGUIGridName) {
        // TODO
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
        this->blobTracker.setAdaptiveThresholdRange(((int) value) | 1);
    } else if (widgetName == kGUIBlobGammaName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        this->inputGamma = slider->getScaledValue();
    } else if (widgetName == kGUICalibrateName) {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        
        // TODO: !!!! uncomment and fix
        /*if (wtmAppStateReceivingSettings != this->state) {
         bool wasRunning = (wtmAppStateReceivingTouches == this->state);
         
         if (wasRunning)
         this->stopSensor();
         
         this->state = wtmAppStateReceivingSettings;
         cout << "trying to receive settings" << endl; // TODO: correct behavior?
         serial.writeBytes((unsigned char*)"c\ni\n", 4);
         
         this->resumeAfterSettingsReceipt = wasRunning;
         
         button->setLabelText("CALIBRATING...");
         }*/
    } else if (widgetName == kGUIStartName) {
        cout << "Start/Stop Button pressed.";
        cout << "dropdown was: " << this->bGUISerialPortDroppedDown << endl;
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        cout << "button value: " << button->getValue() << endl;
        if (button->getValue() == 1) {
            cout << "Button pressed.";
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
void wtmApp::initSerialConnection(char* serialDeviceName) {
    cout << "initalizing serial connection: " << serialDeviceName << endl;
    int baud = 300;
    bSerialConnectionAvailable = serial.setup(serialDeviceName, baud);
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
    // TODO: close all pullDown menus when click happens somewhere outside the menus
    
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
