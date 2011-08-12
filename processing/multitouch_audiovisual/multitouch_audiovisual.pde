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

// interpolation, image post-production
Interpolator interpolator = null;
static final int kInterpLinear = 0;
static final int kInterpCosine = 1;
static final int kInterpCubic = 2;
static final int kInterpCatmullRom = 3;
static final int kInterpHermite = 4;
static final int kNumInterp = 5;
int interpType = kInterpCatmullRom;
int interpolationResolution = 4;
HistogramGUI histogramGUI;

// configuration
static final int verticalWires = 16;
static final int horizontalWires = 11;
static final int crosspointDistance=60; // how many pixels between 2 crosspoints
static final int borderDistance=60; // how many pixel distance to the borderDistance
static final float signalPixelRatio = 0.03*1024; // (see crosspoint.pde)
final color textColor = color(60,60,60);
final color guiColor = color(180,180,180);
final color backgroundColor = color(240,240,240);
final color histogramColor = color(239, 171, 233);
final color signalColor = color(153,233,240);
final color wireColor = color(0,222,255);
static final int AVERAGESIGNALCOUNTERMAX = 150;
float contrastLeft = 0.04;
float contrastRight = 0.5;

int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
int visualizationType = 0;            // which type of visualitazion should be used (0-2)
boolean bDebug = false;               // stop updating and print out some debug data
boolean bReadBinary = true;           // read binary data (instead of strings)
boolean bContrastStretch = true;
String helpText = "";

void setup() {
  sketchWidth = (borderDistance*2)+((verticalWires-1)*crosspointDistance);
  sketchHeight = (horizontalWires+1)*crosspointDistance+90;
  size(sketchWidth, sketchHeight, OPENGL);
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  crosspoints = new Crosspoint[verticalWires][horizontalWires];
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      crosspoints[i][j] = new Crosspoint(borderDistance+(crosspointDistance*i),borderDistance+(crosspointDistance*j));
    } 
  }
  initInterpolator();
  histogramGUI = new HistogramGUI(sketchWidth-256-borderDistance, sketchHeight-30, 256);
  histogramGUI.setMarkerPositions(contrastLeft, contrastRight);
  interpolator.fStretchHistLeft = contrastLeft;
  interpolator.fStretchHistRight = contrastRight;
  textInformation = "[r]eceive real data   [f]ake data (static)";
  helpText = textInformation;
}

void draw() {
  background(backgroundColor);
  switch (visualizationType) {
    case 0:
      interpolator.interpolate(crosspoints);
      interpolator.drawByWidthPreservingAspectRatio(borderDistance, borderDistance, sketchWidth-borderDistance);
      interpolator.drawHistogramFromPoint(sketchWidth-256-borderDistance, sketchHeight-30, 65);
      histogramGUI.draw();
      break;
    case 1:
      drawSignalCircles(true);
      break;
    case 2:
      // TODO: This isn't working yet. Make it work.
      interpolator.interpolate(crosspoints);
      interpolator.drawByWidthPreservingAspectRatio(borderDistance, borderDistance, sketchWidth-borderDistance);
      interpolator.drawHistogramFromPoint(sketchWidth-256-borderDistance, sketchHeight-30, 65);
      histogramGUI.draw();
      drawSignalCircles(true);
      break;
    default:
      break;
  }
  fill(textColor);
  text(textInformation, borderDistance, height-60);
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
    for (int i = 0; i < horizontalWires; i++) {
      line(borderDistance, borderDistance+(crosspointDistance*i), borderDistance+(crosspointDistance*(verticalWires-1)), borderDistance+(crosspointDistance*i));
    }
    for(int j = 0; j < verticalWires; j++) {
      line(borderDistance+(crosspointDistance*j), borderDistance, borderDistance+(crosspointDistance*j), borderDistance+(crosspointDistance*(horizontalWires-1)));
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
        interpolator = new HermiteInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, contrastLeft, contrastRight);
        break;
    case kInterpCatmullRom:
        interpolator = new CatmullRomInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, contrastLeft, contrastRight);
        break;
    case kInterpCubic:
        interpolator = new CubicInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, contrastLeft, contrastRight);
        break;
    case kInterpCosine:
        interpolator = new CosineInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, contrastLeft, contrastRight);
        break;
    case kInterpLinear:
    default:
        interpolator = new LinearInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, contrastLeft, contrastRight);
        break;
  }
  interpolator.bContrastStretch = bContrastStretch;
}

void mousePressed() {
  histogramGUI.mousePressed();  
}

void mouseDragged() {
  histogramGUI.mouseDragged(mouseX, mouseY);
  interpolator.fStretchHistLeft = histogramGUI.getValLeft();
  interpolator.fStretchHistRight = histogramGUI.getValRight();
  textInformation = "contrast stretch:   " + interpolator.fStretchHistLeft + "   " + interpolator.fStretchHistRight;
}

void mouseReleased() {
  histogramGUI.mouseReleased();  
}

void keyPressed() {
  switch(key) {
  case 'r':
    // recalibrate
    averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
    if (dataManager == null) {
      // send signal to arduino to receive data
      try {
        dataManager = new DataManager(new Serial( this, Serial.list()[0], 115200 ));
      } catch (Exception e) {
        textInformation = "error opening Serial connection: "+e;
        break;
      }
      delay(2000); // needed
      dataManager.myPort.write('s');
      helpText = "[c]ontrast stretch   [d]ebug   [h]elp   [i]nterpolation   [o]/[p] interpolation resolution\n[r]ecalibrate   [v]isualization";
    }
    break;
  case 'f':
    // use fake data
    if (dataManager == null) {
      dataManager = new DataManager(null);
      helpText = "[c]ontrast stretch   [h]elp   [i]nterpolation   [o]/[p] interpolation resolution\n[v]isualization";
      textInformation = helpText;
    }
    break;
  case 'h':
    // help
    textInformation = helpText;
    break;
  case 'd':
    // debug
    bDebug = !bDebug;
    if (bDebug) {
      textInformation = "[d]ebug: " + getOnOffString(bDebug);
      dataManager.printData();
    }
    else {
      textInformation = helpText;
    }
    break;
  case 'v':
    // change the type of visualization
    visualizationType = (visualizationType+1)%3;
    break;
  case 'c':
    bContrastStretch = !bContrastStretch;
    interpolator.bContrastStretch = bContrastStretch;
    textInformation = "[c]ontrast stretch: " + getOnOffString(bContrastStretch);
    break;
  case 'i':
    // change interpolation algorithm for pixel matrix
    interpType = ++interpType % kNumInterp;
    initInterpolator();
    textInformation = interpolator.name() + " x" + interpolationResolution;
    break;
  case 'o':
    // decrease interpolation resolution
    if (interpolationResolution>1) interpolationResolution--;
    initInterpolator();
    textInformation = interpolator.name() + " x" + interpolationResolution;
    break;
  case 'p':
    // increase interpolation resolution
    if (interpolationResolution<15) interpolationResolution++;
    initInterpolator();
    textInformation = interpolator.name() + " x" + interpolationResolution;
    break;
  case 'k':
    if (interpolator instanceof HermiteInterpolator) {
      HermiteInterpolator ip = (HermiteInterpolator)interpolator;
      ip.tension += 0.1;
      ip.tension = constrain((float)ip.tension, -2.0, 2.0);
      textInformation = interpolator.name() + " x" + interpolationResolution;
    }
    break;
  case 'l':
    if (interpolator instanceof HermiteInterpolator) {
      HermiteInterpolator ip = (HermiteInterpolator)interpolator;
      ip.tension -= 0.1;
      ip.tension = constrain((float)ip.tension, -2.0, 2.0);
      textInformation = interpolator.name() + " x" + interpolationResolution;
    }
    break;
  }
}

String getOnOffString(boolean b) {
  String s = "OFF";
  if (b) s = "ON";
  return s;
}
