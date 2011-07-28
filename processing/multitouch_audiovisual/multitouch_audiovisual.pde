import processing.opengl.*;
import processing.serial.*;
import java.text.DecimalFormat;


DecimalFormat df = new DecimalFormat("#.###");

Crosspoint[][] crosspoints;
double[][] pixelMatrix;
DataManager dataManager;
PFont myFont;
String textInformation;
static byte[] serBuffer = null;

Interpolator interpolator = null;

static final int kInterpLinear = 0;
static final int kInterpCosine = 1;
static final int kInterpCubic = 2;
static final int kInterpCatmullRom = 3;
static final int kInterpHermite = 4;

static final int kNumInterp = 5;

int interpType = kInterpLinear;
int interpolationResolution = 4;

// configuration
int verticalWires = 16;
int horizontalWires = 11;
static final int crosspointDistance=60; // how many pixels between 2 crosspoints
static final int pixelWidth =  crosspointDistance / 2;
float signalPixelRatio = 0.02*1024; // (see crosspoint.pde)

int sketchWidth, sketchHeight;

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
  sketchWidth = (verticalWires+1)*crosspointDistance;
  sketchHeight = (horizontalWires+1)*crosspointDistance+80;
   
  size(sketchWidth, sketchHeight, OPENGL);
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
  
  initInterpolator();
  
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
  
  interpolator.interpolate(crosspoints);
  
  if (bShowPixelMatrix) {
    drawPixelMatrix();
    fill(textColor);
    text(interpolator.name() + " x" + interpolationResolution, 15, height-55);
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
  interpolator.drawByWidthPreservingAspectRatio(15, 15, sketchWidth-15);
}

void serialEvent(Serial p) {
  if (bReadBinary) {
    dataManager.consumeSerialBuffer(p);
  }
  else {
    dataManager.parseData(p.readString());
  }
}

void initInterpolator() {
  switch (interpType) {
    case kInterpHermite:
        interpolator = new HermiteInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution);
        break;
    case kInterpCatmullRom:
        interpolator = new CatmullRomInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution);
        break;
    case kInterpCubic:
        interpolator = new CubicInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution);
        break;
    case kInterpCosine:
        interpolator = new CosineInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution);
        break;
    case kInterpLinear:
    default:
        interpolator = new LinearInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution);
        break;
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
  
  // change interpolation algorithm for pixel matrix
  if (key == 'i') {
    interpType = ++interpType % kNumInterp;
    initInterpolator();
  }
  
  //change interpolation resolution
  if (key == 'o' || key == 'p') {
    interpolationResolution += ((key == 'o') ? -1 : 1);
    interpolationResolution = max(1, min(interpolationResolution, 15));
    initInterpolator();
  }
  
  if (interpolator instanceof HermiteInterpolator && (key == 'k' || key == 'l')) {
     HermiteInterpolator ip = (HermiteInterpolator)interpolator;
     ip.tension += (key == 'k') ? -0.1 : 0.1;
     ip.tension = constrain((float)ip.tension, -2.0, 2.0);
  }
}
