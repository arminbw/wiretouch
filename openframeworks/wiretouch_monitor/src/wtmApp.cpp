#include "wtmApp.h"

#include "cJSON.h"

#include "interpolator-catmull-rom.h"

#define kGUIHalfwaveAmpName         ("HALFWAVE AMP")
#define kGUIOutputAmpName           ("OUTPUT AMP")
#define kGUISampleDelayName         ("SAMPLE DELAY")
#define kGUISignalFrequencyName     ("SIGNAL FREQUENCY")
#define kGUIPostProcessingName      ("POST PROCESSING")
#define kGUIUpSamplingName          ("UPSAMPLING")
#define kGUIInterpolationTypeName   ("INTERPOLATION")
#define kGUIGridName                ("GRID")
#define kGUIBlobsName               ("BLOBS")
#define kGUIStartName               ("START")
#define kGUILinearName              ("LINEAR")
#define kGUICatmullName             ("CATMULL")
#define kGUICosineName              ("COSINE")
#define kGUICubicName               ("CUBIC")
#define kGUIHermiteName             ("HERMITE")

//--------------------------------------------------------------
void wtmApp::setup()
{
    this->sensorColumns = 32;
    this->sensorRows = 22;
    
    state = wtmAppStateIdle;
	
    this->bytesPerFrame = (sensorColumns*sensorRows*10)/8;
    this->recvBuffer = (unsigned char*)malloc(this->bytesPerFrame * sizeof(unsigned char));
    
    this->settingsString = NULL;
    
    this->capGridValues = (uint16_t*)malloc(sensorColumns * sensorRows * sizeof(uint16_t));
    
    ofSetVerticalSync(true);
	ofBackground(0);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    // setup MAIN GUI
    int guiWidth = 300;
    int widgetWidth = guiWidth - (2* OFX_UI_GLOBAL_WIDGET_SPACING);
    int widgetHeight = 22;
    gui = new ofxUICanvas(WINDOWWIDTH-(guiWidth+WINDOWBORDERDISTANCE),WINDOWBORDERDISTANCE,guiWidth,600);
    gui->addWidgetDown(new ofxUILabel("SENSOR PARAMETERS", OFX_UI_FONT_MEDIUM));
    gui->addSpacer();
    gui->addSlider(kGUIHalfwaveAmpName, 0.0, 255.0, 50, widgetWidth, widgetHeight);
    gui->addSlider(kGUIOutputAmpName, 0.0, 255.0, 50, widgetWidth, widgetHeight);
    gui->addSlider(kGUISampleDelayName, 0.0, 100.0, 50, widgetWidth, widgetHeight);
    gui->addSlider(kGUISignalFrequencyName, 1.0, 60.0, 50, widgetWidth, widgetHeight);
    
    gui->addWidgetDown(new ofxUILabel("INTERPOLATION", OFX_UI_FONT_MEDIUM));
    gui->addSpacer();
    // gui->addWidgetDown(new ofxUISpectrum(widgetWidth, widgetHeight, buffer, 256, 0.0, 1.0, "SPECTRUM"));
    vector<string> whatAType;
    whatAType.push_back(kGUILinearName);
    whatAType.push_back(kGUICatmullName);
    whatAType.push_back(kGUICosineName);
    whatAType.push_back(kGUICubicName);
    whatAType.push_back(kGUIHermiteName);
    ofxUIDropDownList *interpolationDropdownMenu = gui->addDropDownList("TYPE", whatAType, (widgetWidth/2)-(OFX_UI_GLOBAL_WIDGET_SPACING));
    gui->addSlider(kGUIUpSamplingName, 1.0, 8.0, 50, widgetWidth, widgetHeight);
    gui->addSpacer();
    gui->addLabelToggle(kGUIBlobsName, false,(widgetWidth/2)-(OFX_UI_GLOBAL_WIDGET_SPACING),widgetHeight);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addLabelToggle(kGUIGridName, false,(widgetWidth/2)-(OFX_UI_GLOBAL_WIDGET_SPACING/2),widgetHeight);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL));
    gui->addSpacer();
    gui->addLabelButton(kGUIStartName, false, widgetWidth, widgetHeight);
    
    
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(120));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL, ofColor(255, 120)); // font color
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_OUTLINE_HIGHLIGHT, ofColor(0,0,255));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL_HIGHLIGHT, ofColor(239, 171, 233));
    gui->setColorBack(ofColor(100, 80));
    
    gui->loadSettings("GUI/guiSettings.xml");
    ofAddListener(gui->newGUIEvent, this, &wtmApp::guiEvent);
    interpolationDropdownMenu->setAutoClose(true);
    interpolationDropdownMenu->setShowCurrentSelected(true);
    interpolationDropdownMenu->checkAndSetTitleLabel();
    
    this->interpolator = NULL;
    this->interpolatorType = wtmInterpolatorTypeCatmullRom;
    this->interpolatorUpsampleX = 8;
    this->interpolatorUpsampleY = 8;
    
    this->updateInterpolator();
    
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 300;
	serial.setup(0, baud);
}

//--------------------------------------------------------------
void wtmApp::update()
{
    switch (this->state) {
        case wtmAppStateReceivingSettings: {
            while(serial.available()) {
                if (NULL == this->settingsString)
                    this->settingsString = new string();
            
                int c = serial.readByte();
                
                *this->settingsString += (char)c;
                
                if ('\n' == c) {
                    this->state = wtmAppStateReceivingTouches;

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
                        
                        ofLog() << (1.0/(now - this->lastRecvFrameTime)) << " pkts/sec, dt: " << (now - this->lastRecvFrameTime);
                        
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
            break;
    }
}

//--------------------------------------------------------------
void wtmApp::draw()
{
    if (this->texture && this->texture->isAllocated())
        this->texture->draw(0, 0, ofGetWidth(), ofGetHeight());
}

void wtmApp::consumePacketData()
{
    unsigned char* b = this->recvBuffer;
    int bs = 0, br = 0, cnt = 0;
    
    for (int i=0; i<this->bytesPerFrame; i++) {
        br |= b[i] << bs;
        bs += 8;
        while (bs >= 10) {
            int px = cnt / this->sensorRows, py = cnt % this->sensorRows;
            
            this->capGridValues[py * this->sensorColumns + px] = br & 0x3ff;
            
            br >>= 10;
            bs -= 10;
            cnt++;
        }
    }
    
    this->interpolator->runInterpolation(this->capGridValues);
    this->texture = this->interpolator->currentTexture();
}

void wtmApp::consumeSettings(const char* json)
{
    if (NULL != json) {
        cJSON* root = cJSON_Parse(json);
        
        if (NULL != root) {
            cJSON* version = cJSON_GetObjectItem(root, "version"), *cur;
            printf("VERSION: %s\n", version->valuestring);
            
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

void
wtmApp::updateInterpolator()
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

//------------------------------------------------------------s--
void wtmApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void wtmApp::keyReleased(int key){

}

//--------------------------------------------------------------
void wtmApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void wtmApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void wtmApp::mousePressed(int x, int y, int button){
    // close all pullDown menus when click happens somewhere outside the menus
    
}

//--------------------------------------------------------------
void wtmApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void wtmApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void wtmApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void wtmApp::dragEvent(ofDragInfo dragInfo){ 

}

void wtmApp::exit()
{
    serial.close();
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}

void wtmApp::guiEvent(ofxUIEventArgs &e)
{
    char buf[32];
    string widgetName = e.widget->getName();
    // printf("guiEvent: %s\n", widgetName.c_str());
	if (widgetName == kGUIHalfwaveAmpName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        snprintf(buf, sizeof(buf), "h%d\n", val);
        serial.writeBytes((unsigned char*)buf, strlen(buf));
    } else if (widgetName == kGUIOutputAmpName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        snprintf(buf, sizeof(buf), "o%d\n", val);
        serial.writeBytes((unsigned char*)buf, strlen(buf));
    } else if (widgetName == kGUISampleDelayName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        snprintf(buf, sizeof(buf), "d%d\n", val);
        serial.writeBytes((unsigned char*)buf, strlen(buf));
    } else if (widgetName == kGUIUpSamplingName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        this->interpolatorUpsampleX = val;
        this->interpolatorUpsampleY = val;
        this->updateInterpolator();
    } else if (widgetName == kGUISignalFrequencyName) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        int val = round(slider->getScaledValue());
        slider->setValue(val);
        snprintf(buf, sizeof(buf), "f%d\n", val);
        serial.writeBytes((unsigned char*)buf, strlen(buf));
    } else if (widgetName == kGUILinearName) {
        this->interpolatorType = wtmInterpolatorTypeLinear;
        this->updateInterpolator();
    } else if (widgetName == kGUICatmullName) {
        this->interpolatorType = wtmInterpolatorTypeCatmullRom;
        this->updateInterpolator();
    } else if (widgetName == kGUICosineName) {
        this->interpolatorType = wtmInterpolatorTypeCosine;
        this->updateInterpolator();
    } else if (widgetName == kGUICubicName) {
        this->interpolatorType = wtmInterpolatorTypeCubic;
        this->updateInterpolator();
    } else if (widgetName == kGUIHermiteName) {
        this->interpolatorType = wtmInterpolatorTypeHermite;
        this->updateInterpolator();
    } else if (widgetName == kGUIBlobsName) {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawBlobs = button->getValue();
    } else if (widgetName == kGUIGridName) {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawGrid = button->getValue();
    } else if (widgetName == kGUIStartName) {
        if (wtmAppStateIdle == this->state) {
            serial.writeByte('c');
            serial.writeByte('\n');
            serial.writeByte('i');
            serial.writeByte('\n');
            serial.writeByte('s');
            serial.writeByte('\n');
            
            this->state = wtmAppStateReceivingSettings;
        }
    }
}
