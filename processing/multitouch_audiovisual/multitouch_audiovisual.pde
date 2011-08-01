import processing.opengl.*;
import processing.serial.*;
import java.text.DecimalFormat;

DecimalFormat df = new DecimalFormat("#.###");

Crosspoint[][] crosspoints;
DataManager dataManager = null;
PFont myFont;
String textInformation;
static byte[] serBuffer = null;
int sketchWidth, sketchHeight;

// interpolation
Interpolator interpolator = null;
static final int kInterpLinear = 0;
static final int kInterpCosine = 1;
static final int kInterpCubic = 2;
static final int kInterpCatmullRom = 3;
static final int kInterpHermite = 4;
static final int kNumInterp = 5;
int interpType = kInterpCatmullRom;
int interpolationResolution = 4;

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
static final int AVERAGESIGNALCOUNTERMAX = 150;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
boolean bDebug = false;               // stop updating and print out some debug data
int visualizationType = 0;            // which type of visualitazion should be used (0-2)
boolean bReadBinary = true;           // read binary data (instead of strings)
String helpText = "";

void setup() {
  sketchWidth = (verticalWires+1)*crosspointDistance;
  sketchHeight = (horizontalWires+1)*crosspointDistance+36;
   
  size(sketchWidth, sketchHeight, OPENGL);
  // smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  crosspoints = new Crosspoint[verticalWires][horizontalWires];
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(i+1),crosspointDistance*(j+1));
    } 
  }
  initInterpolator();
  textInformation = "[r]eceive real data   [f]ake data (static)";
}

void draw() {
  background(backgroundColor);
  switch (visualizationType) {
    case 0:
      interpolator.interpolate(crosspoints);
      interpolator.drawByWidthPreservingAspectRatio(15, 15, sketchWidth-15);
      break;
    case 1:
      drawSignalCircles(true);
      break;
    case 2:
      // TODO: This isn't working yet. Make it work.
      interpolator.drawByWidthPreservingAspectRatio(15, 15, sketchWidth-15);
      drawSignalCircles(true);
      break;
    default:
      break;
  }
  fill(textColor);
  text(textInformation, 15, height-24);
}

void drawSignalCircles(boolean bDrawText) {
    if (averageSignalCounter == 0) {
      // draw the crosspoint signal circles
      noStroke();
      for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
          crosspoints[i][j].draw(bDrawText);
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
  // receive real data
  if (key == 'r') {
    if (dataManager == null) {
      dataManager = new DataManager(new Serial( this, Serial.list()[0], 115200 ));
      delay(2000); // needed
      dataManager.myPort.write('s');
      helpText = "[c]alibrate   [d]ebug   [h]elp   [i]nterpolation   [o]/[p] interpolation resolution   [v]isualization";
    }
  }
  // use fake data
  if (key == 'f') {
    if (dataManager == null) {
      dataManager = new DataManager(null);
      helpText = "[h]elp   [i]nterpolation   [o]/[p] interpolation resolution   [v]isualization";
      textInformation = helpText;
    }
  }
  // help
  if (key == 'h') {
    textInformation = helpText;
  }
  // recalibrate
  if (key == 'c') {
    averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
  }
  // debug
  if (key == 'd') {
    bDebug = !bDebug;
    if (bDebug) {
       textInformation = "press [d] again to stop debugging mode";
       dataManager.printData();
    }
    else {
       textInformation = helpText;
    }
  }
  // change the type of visualization
  if (key == 'v') {
    visualizationType = (visualizationType+1)%3;
  }
  // change interpolation algorithm for pixel matrix
  if (key == 'i') {
    interpType = ++interpType % kNumInterp;
    initInterpolator();
    textInformation = interpolator.name() + " x" + interpolationResolution;
  }
  //change interpolation resolution
  if (key == 'o' || key == 'p') {
    interpolationResolution += ((key == 'o') ? -1 : 1);
    interpolationResolution = max(1, min(interpolationResolution, 15));
    initInterpolator();
    textInformation = interpolator.name() + " x" + interpolationResolution;
  }
  if (interpolator instanceof HermiteInterpolator && (key == 'k' || key == 'l')) {
     HermiteInterpolator ip = (HermiteInterpolator)interpolator;
     ip.tension += (key == 'k') ? -0.1 : 0.1;
     ip.tension = constrain((float)ip.tension, -2.0, 2.0);
     textInformation = interpolator.name() + " x" + interpolationResolution;
  }
}
