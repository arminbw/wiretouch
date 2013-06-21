#include "wtmApp.h"

//--------------------------------------------------------------
void wtmApp::setup()
{
    this->sensorColumns = 32;
    this->sensorRows = 22;
	
    this->bytesPerFrame = (sensorColumns*sensorRows*10)/8;
    this->recvBuffer = (unsigned char*)malloc(this->bytesPerFrame * sizeof(unsigned char));
    this->recvPos = 0;
    
    ofSetVerticalSync(true);
	
	ofBackground(255);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 230400;
	serial.setup(0, baud);
    sleep(5);
}

//--------------------------------------------------------------
void wtmApp::update()
{    
    if (!didSend) {
        didSend = 1;
        
        for (int i=0; i<100; i++) {
            serial.writeByte('s');
            serial.writeByte('\n');
        }
    } else {
        while (serial.available()) {
            if (this->recvPos >= this->bytesPerFrame) {
                this->recvPos = 0;
                
                this->makeTexture();
            } else {
                this->recvBuffer[this->recvPos++] = serial.readByte();
            }
        }
    }
}

//--------------------------------------------------------------
void wtmApp::draw(){
    if (this->texture.isAllocated())
        this->texture.draw(10, 10, 640, 440);
}

void wtmApp::makeTexture()
{
    unsigned char* pixels = (unsigned char*)alloca(this->sensorColumns * this->sensorRows * sizeof(unsigned char));
    
    unsigned char* b = this->recvBuffer;
    int bs = 0, br = 0, cnt = 0;
    
    for (int i=0; i<this->bytesPerFrame; i++) {
        br |= b[i] << bs;
        bs += 8;
        while (bs >= 10) {
            int sig = br & 0x3ff;
            br >>= 10;
            bs -= 10;
            
            int px = cnt / this->sensorRows, py = cnt % this->sensorRows;
            
            pixels[py * this->sensorColumns + px] = (unsigned char)((float)sig/(float)1023.0 * 255);
            
            cnt++;
        }
    }
    
    if (!this->texture.isAllocated())
        this->texture.allocate(this->sensorColumns, this->sensorRows, GL_LUMINANCE);
    
    this->texture.loadData(pixels, this->sensorColumns, this->sensorRows, GL_LUMINANCE);    
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
}