import processing.opengl.*;
import processing.serial.*;
import java.text.DecimalFormat;
import blobDetection.*;
import codeanticode.glgraphics.*; // http://glgraphics.sourceforge.net

DecimalFormat df = new DecimalFormat("#.###");

Crosspoint[][] crosspoints;
DataManager dataManager = null;
PFont myFont;
String textInformation;

// interpolation, image post-production, blob detection
Interpolator interpolator = null;
static final int kInterpLinear = 0;
static final int kInterpCosine = 1;
static final int kInterpCubic = 2;
static final int kInterpCatmullRom = 3;
static final int kInterpHermite = 4;
static final int kNumInterp = 5;
int interpType = kInterpCatmullRom;
int interpolationResolution = 3;
HistogramGUI histogramGUI;
GLTexture picture;
// BlobManager blobManager;

// configuration
Configurator configurator;
static final int verticalWires = 32;
static final int horizontalWires = 22;
static final int crosspointDistance=25; // how many pixels between 2 crosspoints
static final int borderDistance=15; // how many pixel distance to the borderDistance
static final int sketchWidth = (borderDistance*2)+((verticalWires-1)*crosspointDistance);
static final int sketchHeight = (horizontalWires+1)*crosspointDistance+90;
static final int pictureWidth = (verticalWires-1)*crosspointDistance;
static final int pictureHeight = (horizontalWires-1)*crosspointDistance;
static final float signalPixelRatio = 0.02*1024; // (see crosspoint.pde)
final color textColor = color(60, 60, 60);
final color guiColor = color(180, 180, 180);
final color backgroundColor = color(240, 240, 240);
final color histogramColor = color(239, 171, 233);
final color signalColor = color(153, 233, 240);
final color wireColor = color(0, 222, 255);
static final int AVERAGESIGNALCOUNTERMAX = 150;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
float contrastLeft = 0.0;
float contrastRight = 0.0;

int lastMillis, frames, packets, fps, pps;
boolean bNewFrame; // only draw if there's new information

void setup() {
  size(sketchWidth, sketchHeight, GLConstants.GLGRAPHICS);
  // testing GLGRAPHICS
  GLTextureParameters gp = new GLTextureParameters();
  picture = new GLTexture(this, (verticalWires - 1)*interpolationResolution, (horizontalWires - 1)*interpolationResolution, gp);    
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  crosspoints = new Crosspoint[verticalWires][horizontalWires];
  for (int i = 0; i < verticalWires; i++) {
    for (int j = 0; j < horizontalWires; j++) {
      crosspoints[i][j] = new Crosspoint(borderDistance+(crosspointDistance*i), borderDistance+(crosspointDistance*j));
    }
  }
  dataManager = new DataManager();
  configurator = new Configurator(dataManager);
  initInterpolator();
  histogramGUI = new HistogramGUI(sketchWidth-256-borderDistance, sketchHeight-30, 256);
  histogramGUI.setMarkerPositions(contrastLeft, contrastRight);
  configurator.helpText = "[r]eceive real data   [f]ake data (static)";
  textInformation = configurator.helpText;
  // blobManager = new BlobManager(interpolator.pixelWidth, interpolator.pixelHeight);
  lastMillis = millis();
  bNewFrame = true;
}

void draw() {
  if ((millis() - lastMillis) > 1000) {
      lastMillis = millis();
      fps = frames+1;
      pps = packets;
      frames = 0;
      packets = 0;
      bNewFrame = true;
  } 
  else {
      frames++;
  }
  if (bNewFrame) {
    bNewFrame = false;
    background(backgroundColor);
    switch (configurator.visualizationType) {
    case 0:
      interpolator.interpolate(crosspoints);
      interpolator.drawPicture(borderDistance, borderDistance);
      // rblobManager.drawBlobs();
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
    text(fps+" fps", borderDistance, 10);
    text(pps+" packets per second", 80, 10);
  }
}

void drawSignalCircles(boolean bDrawText) {
  if (averageSignalCounter == 0) {
    // draw the crosspoint signal circles
    noStroke();
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
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
  for (int j = 0; j < verticalWires; j++) {
    line(borderDistance+(crosspointDistance*j), borderDistance, borderDistance+(crosspointDistance*j), borderDistance+(crosspointDistance*(horizontalWires-1)));
    text(j+1, borderDistance+(crosspointDistance*j), borderDistance-4);
  }
  textAlign(LEFT);
}

void initSerial() {
  try {
    dataManager.initSerial(new Serial( this, Serial.list()[0], 230400 ));
  }
  catch (Exception e) {
    textInformation = "error opening Serial connection: "+e;
    println(Serial.list());
    exit();
  }
}

void serialEvent(Serial p) {
  dataManager.consumeSerialBuffer(p);
  packets++;
  bNewFrame = true;
}

void showHelpText() {
  textInformation = configurator.helpText;
}

void initInterpolator() {
  // testing GLGRAPHICS
  GLTextureParameters gp = new GLTextureParameters();
  gp.minFilter = GLConstants.NEAREST_SAMPLING;
  gp.magFilter = GLConstants.NEAREST_SAMPLING;
  picture = new GLTexture(this, (verticalWires - 1)*interpolationResolution, (horizontalWires - 1)*interpolationResolution, gp);    
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
  interpolator.bContrastStretch = configurator.bContrastStretch;
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
  bNewFrame = true;
}

void mouseReleased() {
  histogramGUI.mouseReleased();
  bNewFrame = true;
}

void keyPressed() {
  configurator.changeConfiguration(key);
  bNewFrame = true;
}

