#include "wtmApp.h"

#include "cJSON.h"

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
    
    // setup GUI
    int guiWidth = 300;
    int widgetLength = guiWidth - (2* OFX_UI_GLOBAL_WIDGET_SPACING);
    int widgetHeight = 22;
    gui = new ofxUICanvas(WINDOWWIDTH-(guiWidth+WINDOWBORDERDISTANCE),WINDOWBORDERDISTANCE,guiWidth,600);
    gui->addWidgetDown(new ofxUILabel("SENSOR PARAMETERS", OFX_UI_FONT_MEDIUM));
    gui->addSpacer();
    gui->addSlider("HALFWAVE AMP", 0.0, 255.0, 50, widgetLength, widgetHeight);
    gui->addSlider("OUTPUT AMP", 0.0, 255.0, 50, widgetLength, widgetHeight);
    gui->addSlider("SAMPLE DELAY", 0.0, 100.0, 50, widgetLength, widgetHeight);
    gui->addSlider("SIGNAL FREQUENCY", 1.0, 60.0, 50, widgetLength, widgetHeight);
    
    gui->addWidgetDown(new ofxUILabel("POST PROCESSING", OFX_UI_FONT_MEDIUM));
    gui->addSpacer();
    gui->addSlider("UPSAMPLING", 1.0, 8.0, 50, widgetLength, widgetHeight);
    gui->addLabelToggle("BLOBS", false,(widgetLength/2)-(OFX_UI_GLOBAL_WIDGET_SPACING),widgetHeight);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addLabelToggle("GRID", false,(widgetLength/2)-(OFX_UI_GLOBAL_WIDGET_SPACING/2),widgetHeight);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL));
    gui->addSpacer();
    gui->addButton("START", false, widgetLength, widgetHeight);
    
    ofAddListener(gui->newGUIEvent, this, &wtmApp::guiEvent);
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, ofColor(160));
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL, ofColor(255)); // also: font color
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_PADDED, ofColor(255,0,0)); // also: font color
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL_HIGHLIGHT, ofColor(60));
    gui->setColorBack(ofColor(100, 80));
    gui->loadSettings("GUI/guiSettings.xml");
    
    this->interpolator = NULL;
    this->interpolatorType = wtmInterpolatorTypeTypeCatmullRom;
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
            ofxUISlider* slider = (ofxUISlider*)gui->getWidget("HALFWAVE AMP");
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "output_amp");
            slider = (ofxUISlider*)gui->getWidget("OUTPUT AMP");
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "delay");
            slider = (ofxUISlider*)gui->getWidget("SAMPLE DELAY");
            slider->setValue(atoi(cur->valuestring));
            
            cur = cJSON_GetObjectItem(root, "freq");
            slider = (ofxUISlider*)gui->getWidget("SIGNAL FREQUENCY");
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
    string widgetName = e.widget->getName();
	if (widgetName == "HALFWAVE AMP") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        // slider->getScaledValue()
    }
    else if (widgetName == "OUTPUT AMP") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;        
    }
    else if (widgetName == "SAMPLE DELAY") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
    }
    else if (widgetName == "SIGNAL FREQUENCY") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
    }
    else if (widgetName == "BLOBS") {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawBlobs = button->getValue();
    }
    else if (widgetName == "GRID") {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawGrid = button->getValue();
    }
    else if (widgetName == "START") {
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
