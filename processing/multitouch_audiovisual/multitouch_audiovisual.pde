import processing.serial.*;
import java.text.DecimalFormat;

DecimalFormat df = new DecimalFormat("#.###");

Crosspoint[][] crosspoints;
double[][] pixelMatrix;
DataManager dataManager;
PFont myFont;
String textInformation;
static byte[] serBuffer = null;

// configuration
int verticalWires = 16;
int horizontalWires = 11;
static final int crosspointDistance=60; // how many pixels between 2 crosspoints
static final int pixelWidth =  crosspointDistance / 2;
float signalPixelRatio = 0.02*1024; // (see crosspoint.pde)

color textColor = color(60,60,60);
color backgroundColor = color(240,240,240);
color wireColor = color(180,180,180);
color signalColor = color(190,190,190);
color signalColorTouched = color(102,149,192);
float signalThreshold = 0.35;
static final int AVERAGESIGNALCOUNTERMAX = 150;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
boolean bDebug = false;               // stop updating and print out some debug data
boolean bUpdate = true;               // stop updating
boolean bFakeData = true;             // just show a single set of example data
boolean bShowCrosspointText = false;   // show detailed signal data on the crosspoints
boolean bShowPixelMatrix = true;      // show the pixel matrix (instead of the signal circles)
boolean bReadBinary = true;           // read binary data (instead of strings)

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance+80, P2D);
  smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  textInformation = "starting";
  crosspoints = new Crosspoint[verticalWires][horizontalWires];
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(i+1),crosspointDistance*(j+1));
    } 
  }
  pixelMatrix = new double[(verticalWires*2)+1][(horizontalWires*2)+1];
  wipePixelMatrix();
  if (bFakeData) 
    dataManager = new DataManager(null);
  else
    dataManager = new DataManager(new Serial( this, Serial.list()[0], 115200 ));
}

void wipePixelMatrix() {
  for (int i = 0; i < (verticalWires*2)+1; i++) {
    for(int j = 0; j < (horizontalWires*2)+1; j++) {
      pixelMatrix[i][j] = 0.0;
    } 
  }
}

void draw() {
  background(backgroundColor);
  if (bFakeData) {
    dataManager.parseFakeData(); 
  }
  if (bShowPixelMatrix) {
    drawPixelMatrix();
  }
  else {
    drawSignalCircles();
  }
  fill(textColor);
  text(textInformation, 15, height-70);
}

void drawSignalCircles() {
    if (averageSignalCounter == 0) {
      // draw the crosspoint signal circles
      noStroke();
      for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
          crosspoints[i][j].draw();
        }
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

void drawPixelMatrix() {
  wipePixelMatrix();    
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      pixelMatrix[(i*2)+1][(j*2)+1] = crosspoints[i][j].signalStrength;
      // interpolation
      pixelMatrix[(i*2)][(j*2)+1] += crosspoints[i][j].signalStrength/2;
      pixelMatrix[(i*2)+1][(j*2)] += crosspoints[i][j].signalStrength/2;
      pixelMatrix[(i*2)+1][(j*2)+2] += crosspoints[i][j].signalStrength/2;
      pixelMatrix[(i*2)+2][(j*2)+1] += crosspoints[i][j].signalStrength/2;
      // diagonal pixels
      pixelMatrix[(i*2)][(j*2)] += crosspoints[i][j].signalStrength/4;
      pixelMatrix[(i*2)+2][(j*2)] += crosspoints[i][j].signalStrength/4;
      pixelMatrix[(i*2)][(j*2)+2] += crosspoints[i][j].signalStrength/4;
      pixelMatrix[(i*2)+2][(j*2)+2] += crosspoints[i][j].signalStrength/4;
    }
  }
  // draw the pixel matrix
  noStroke();
  for (int i = 0; i < (verticalWires*2)+1; i++) {
    for (int j = 0; j < (horizontalWires*2)+1; j++) {
      fill(color((float) pixelMatrix[i][j]*255));
      rect(i*pixelWidth+15, 15+j*pixelWidth, pixelWidth, pixelWidth);
    } 
  }
}

void serialEvent(Serial p) {
  if (bReadBinary) {
    dataManager.consumeSerialBuffer(p);
  }
  else {
    dataManager.parseData(p.readString());
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
    if (bDebug) {
       textInformation = "press 'b' to stop debugging mode";
       dataManager.printData();
       bUpdate = false;
    }
    else {
       textInformation = "press 'b' to sto";
       bUpdate = true;
    }
  }
  // change the visualization (signal circles vs. pixel matrix)
  if (key == 'v') {
    bShowPixelMatrix = !bShowPixelMatrix;
  }
  // tell arduino: send data
  if (key == 's') {
    if (!bFakeData) {
      dataManager.myPort.write('s');
    }
  }
}
