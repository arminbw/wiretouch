import arb.soundcipher.*; // get it at http://soundcipher.org
import processing.serial.*;

SoundCipher sc = new SoundCipher(this);
Serial myPort;
Crosspoint[][] crosspoints;
PFont myFont;

// configuration
int verticalWires = 16;
int horizontalWires = 2;
int crosspointDistance = 80; // how many pixels between 2 crosspoints
float signalPixelRatio = 0.07; // (see crosspoint.pde)
color textColor = color(0,0,0);
color backgroundColor = color(240,240,240);
color wireColor = color(200,200,200);
color signalColor = color(220,220,220);
color signalColorTouched = color(180,180,180);
float signalThreshold = 400;
int averageSignalCounter = 100;

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance);
  smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  myPort = new Serial( this, Serial.list()[0], 115200 );
  myPort.clear(); // do we need this?
  myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent() 
  
  crosspoints = new Crosspoint[horizontalWires][verticalWires];
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(j+1),crosspointDistance*(i+1));
    } 
  }
}

void draw() {
  background(backgroundColor);
  // draw the crosspoint signals
  noStroke();
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j].draw();
    }
  }
  // draw the grid
  stroke(wireColor);
  for (int i = 1; i <= horizontalWires; i++) {
    line(crosspointDistance, crosspointDistance*i, crosspointDistance*verticalWires, crosspointDistance*i);
    for(int j = 1; j <= verticalWires; j++) {
      line(crosspointDistance*j, crosspointDistance, crosspointDistance*j, crosspointDistance*horizontalWires);
    } 
  }
}

void serialEvent(Serial p) {
  String myString = trim(p.readString());
  int data[] = int(split(myString,','));
  int k = 0;
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j].setSignalStrength(data[k]);
      // calculate the average signal strength for every crosspoint
      if (averageSignalCounter != 0) {
        crosspoints[i][j].accumulateAvgSig(data[k]);
      }
      k++;
    }
  }
  if (averageSignalCounter != 0) {
    averageSignalCounter--;
  }
}
