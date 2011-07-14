import processing.serial.*;
import java.text.DecimalFormat;

Serial myPort;
Crosspoint[][] crosspoints;
float[][] pixelMatrix; 
PFont myFont;
ArrayList notes;
String textInformation;

// configuration
int verticalWires = 16;
int horizontalWires = 11;
int crosspointDistance = 50; // how many pixels between 2 crosspoints
float signalPixelRatio = 0.04*1024; // (see crosspoint.pde)

color textColor = color(60,60,60);
color backgroundColor = color(240,240,240);
color wireColor = color(180,180,180);
color signalColor = color(190,190,190);
color signalColorTouched = color(102,149,192);
float signalThreshold = 0.35;
static final int AVERAGESIGNALCOUNTERMAX = 150;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
boolean bDebug = false;
boolean bFakeData = true;
boolean bUpdate = true;
boolean bShowText = false;
boolean bShowPixelMatrix = false;

DecimalFormat df = new DecimalFormat("#.###");

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance+100, P2D);
  smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  textInformation = "starting";
  float note = 24.0;
  notes = new ArrayList();
  crosspoints = new Crosspoint[verticalWires][horizontalWires];
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(i+1),crosspointDistance*(j+1));
      // pixelMatrix[i][j] = 0.0;
    } 
  }   
  myPort = new Serial( this, Serial.list()[1], 115200 );
  myPort.clear(); // do we need this?
  myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent() 
}

void draw() {
  if (bFakeData) {
     // String fakeData = "250,406,519,238,185,29,60,15,46,7,3,263,552,688,368,481,223,467,231,243,135,22,297,682,732,443,647,364,604,372,419,248,130,387,668,714,448,643,345,595,372,408,230,133,353,539,715,403,568,270,504,252,241,101,59,304,496,693,377,446,174,275,120,102,15,3,252,444,663,341,368,131,161,54,24,4,2,252,405,620,313,286,98,127,24,16,3,2,263,359,506,217,153,22,65,12,14,3,2,244,345,343,95,98,31,79,13,12,3,2,246,368,491,212,121,17,58,10,11,3,1,252,384,508,225,103,15,45,8,14,3,2,257,406,595,299,120,17,46,7,71,41,27,291,444,664,345,309,118,146,88,143,78,48,304,445,668,361,369,174,226,160,185,87,48,291,355,596,316,244,71,121,79,92,13,3,");
  }
  background(backgroundColor);
  if (averageSignalCounter == 0) {
    // draw the crosspoint signals
    noStroke();
    for (int i = 0; i < verticalWires; i++) {
      for(int j = 0; j < horizontalWires; j++) {
        crosspoints[i][j].draw();
      }
    }
    // draw the grid
    stroke(wireColor);
    for (int i = 1; i <= horizontalWires; i++) {
      line(crosspointDistance, crosspointDistance*i, crosspointDistance*verticalWires, crosspointDistance*i);
    }
    for(int j = 1; j <= verticalWires; j++) {
      line(crosspointDistance*j, crosspointDistance, crosspointDistance*j, crosspointDistance*horizontalWires);
    }
  }
  drawTextInformation();
}

void drawTextInformation() {
  fill(textColor);
  text(textInformation, 10, height-110);
}

void serialEvent(Serial p) {
  parseData(p.readString());
}

void parseData(String myString) {
  if (bUpdate) {
    myString = trim(myString);
    int data[] = int(split(myString,','));
    int k = 0;
    for (int i = 0; i < verticalWires; i++) {
      for(int j = 0; j < horizontalWires; j++) {
        // calculate the average signal strength for every crosspoint
        if (averageSignalCounter > 0) {
          crosspoints[i][j].accumulateAvgSig(data[k]);  
        } else {
          crosspoints[i][j].setSignalStrength(data[k]);
        }
        k++;
      }
    }
    if (averageSignalCounter > 0) {
      averageSignalCounter--;
      textInformation = "calibrating: "+averageSignalCounter;
    }
    if (bDebug == true) {
      textInformation = "press 'b' to stop debugging mode";
      println("\nDEBUGGING:\n"+myString);
      bUpdate = false;
    }
  } 
}

void keyPressed() {
  // recalibrate
  if (key == 'r') {
    averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
  }
  // debug
  if (key == 'b') {
    bDebug = !bDebug;
    if (bDebug == false) bUpdate = true;
  }
}
