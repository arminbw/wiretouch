import processing.opengl.*;
import processing.serial.*;
import java.text.DecimalFormat;
import blobDetection.*;

DecimalFormat df = new DecimalFormat("#.###");

Crosspoint[][] crosspoints;
DataManager dataManager = null;
PFont myFont;
String textInformation;
static byte[] serBuffer = null;

// interpolation, image post-production, blob detection
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
BlobManager blobManager;

// configuration
static final int verticalWires = 32;
static final int horizontalWires = 22;
static final int crosspointDistance=25; // how many pixels between 2 crosspoints
static final int borderDistance=15; // how many pixel distance to the borderDistance
static final int sketchWidth = (borderDistance*2)+((verticalWires-1)*crosspointDistance);
static final int sketchHeight = (horizontalWires+1)*crosspointDistance+90;
static final int pictureWidth = (verticalWires-1)*crosspointDistance;
static final int pictureHeight = (horizontalWires-1)*crosspointDistance;

static final float signalPixelRatio = 0.02*1024; // (see crosspoint.pde)
final color textColor = color(60,60,60);
final color guiColor = color(180,180,180);
final color backgroundColor = color(240,240,240);
final color histogramColor = color(239, 171, 233);
final color signalColor = color(153,233,240);
final color wireColor = color(0,222,255);
static final int AVERAGESIGNALCOUNTERMAX = 150;
float contrastLeft = 0.0;
float contrastRight = 0.0;

int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
int visualizationType = 0;            // which type of visualitazion should be used (0-2)
boolean bDebug = false;               // stop updating and print out some debug data
boolean bReadBinary = true;           // read binary data (instead of strings)
boolean bContrastStretch = true;
String helpText = "";
int lastMillis, frames, packets, fps, pps;

void setup() {
  size(sketchWidth, sketchHeight, P2D); // TODO : change to OPENGL2?
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
  textInformation = "[r]eceive real data   [f]ake data (static)";
  helpText = textInformation;
  blobManager = new BlobManager(interpolator.pixelWidth, interpolator.pixelHeight);
  lastMillis = millis();
}

void draw() {
  background(backgroundColor);
  switch (visualizationType) {
    case 0:
      interpolator.interpolate(crosspoints);
      interpolator.drawPicture(borderDistance, borderDistance);
      blobManager.drawBlobs();
      interpolator.drawHistogramFromPoint(sketchWidth-256-borderDistance, sketchHeight-30, 65);
      histogramGUI.draw();
      break;
    case 1:
      drawSignalCircles(true);
      drawGrid();
      break;
    case 2:
      interpolator.interpolate(crosspoints);
      interpolator.drawPicture(borderDistance, borderDistance);
      interpolator.drawHistogramFromPoint(sketchWidth-256-borderDistance, sketchHeight-30, 65);
      histogramGUI.draw();
      drawSignalCircles(false);
      drawGrid();
      break;
    default:
      break;
  }
  fill(textColor);
  text(textInformation, borderDistance, height-60);
  if ((millis() - lastMillis) > 1000) {
    lastMillis = millis();
    fps = frames+1;
    pps = packets;
    frames = 0;
    packets = 0;
  } else {
    frames++;
  }
  text(fps+" fps", borderDistance,10);
  text(pps+" packets per second", 80, 10);
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
}

void drawGrid() {
    stroke(wireColor);
    fill(wireColor);
    textAlign(RIGHT);
    for (int i = 0; i < horizontalWires; i++) {
      line(borderDistance, borderDistance+(crosspointDistance*i), borderDistance+(crosspointDistance*(verticalWires-1)), borderDistance+(crosspointDistance*i));
      text(i+1, borderDistance-4, borderDistance+(crosspointDistance*i)+4);
    }
    textAlign(CENTER);
    for(int j = 0; j < verticalWires; j++) {
      line(borderDistance+(crosspointDistance*j), borderDistance, borderDistance+(crosspointDistance*j), borderDistance+(crosspointDistance*(horizontalWires-1)));
      text(j+1, borderDistance+(crosspointDistance*j), borderDistance-4);
    }
    textAlign(LEFT);
}

void serialEvent(Serial p) {
  if (bReadBinary) {
    dataManager.consumeSerialBuffer(p);
    packets++;
  }
  else {
    dataManager.parseData(p.readString());
  }
}

void initInterpolator() {
  switch (interpType) {
    case kInterpHermite:
        interpolator = new HermiteInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, pictureWidth, pictureHeight);
        break;
    case kInterpCatmullRom:
        interpolator = new CatmullRomInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, pictureWidth, pictureHeight);
        break;
    case kInterpCubic:
        interpolator = new CubicInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, pictureWidth, pictureHeight);
        break;
    case kInterpCosine:
        interpolator = new CosineInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, pictureWidth, pictureHeight);
        break;
    case kInterpLinear:
    default:
        interpolator = new LinearInterpolator(verticalWires, horizontalWires, interpolationResolution, interpolationResolution, pictureWidth, pictureHeight);
        break;
  }
  interpolator.bContrastStretch = bContrastStretch;
  // blobManager = new BlobManager(pictureWidth, pictureHeight);
}

void mousePressed() {
  histogramGUI.mousePressed();  
}

void mouseDragged() {
  histogramGUI.mouseDragged(mouseX, mouseY);
  contrastLeft = histogramGUI.getValLeft();
  contrastRight = histogramGUI.getValRight();
  textInformation = "contrast stretch:   " + contrastLeft + "   " + contrastRight;
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
        dataManager = new DataManager(new Serial( this, Serial.list()[0], 230400 ));
      } catch (Exception e) {
        textInformation = "error opening Serial connection: "+e;
        break;
      }
      delay(2000); // needed
      dataManager.myPort.write('s');
      helpText = "[c]ontrast stretch   [d]ebug   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution\n[r]ecalibrate   [v]isualization";
    }
    break;
  case 'f':
    // use fake data
    if (dataManager == null) {
      dataManager = new DataManager(null);
      helpText = "[c]ontrast stretch   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution   [v]isualization";
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
    textInformation = interpolator.name + " x" + interpolationResolution;
    break;
  case 'o':
    // decrease interpolation resolution
    if (interpolationResolution>1) interpolationResolution--;
    initInterpolator();
    textInformation = interpolator.name + " x" + interpolationResolution;
    break;
  case 'p':
    // increase interpolation resolution
    if (interpolationResolution<15) interpolationResolution++;
    initInterpolator();
    textInformation = interpolator.name + " x" + interpolationResolution;
    break;
  case 'k':
    if (interpolator instanceof HermiteInterpolator) {
      HermiteInterpolator ip = (HermiteInterpolator)interpolator;
      ip.tension += 0.1;
      ip.tension = constrain((float)ip.tension, -2.0, 2.0);
      textInformation = interpolator.name + " x" + interpolationResolution;
    }
    break;
  case 'l':
    if (interpolator instanceof HermiteInterpolator) {
      HermiteInterpolator ip = (HermiteInterpolator)interpolator;
      ip.tension -= 0.1;
      ip.tension = constrain((float)ip.tension, -2.0, 2.0);
      textInformation = interpolator.name + " x" + interpolationResolution;
    }
    break;
  }
}

String getOnOffString(boolean b) {
  String s = "OFF";
  if (b) s = "ON";
  return s;
}
