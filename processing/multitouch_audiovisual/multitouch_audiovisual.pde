import processing.opengl.*;
import processing.serial.*;
import java.text.DecimalFormat;
import blobDetection.*;
import codeanticode.glgraphics.*; // http://glgraphics.sourceforge.net
import s373.flob.*; // http://s373.net/code/flob/flob.html
import oscP5.*;
import netP5.*;
import org.json.*;

String firmwareVersion;

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
BlobManager blobManager;
FlobManager flobManager;
TuioServer tuioServer;

// configuration
Configurator configurator;
static final int verticalWires = 32;
static final int horizontalWires = 22;
static final int crosspointDistance=25; // 25; // how many pixels between 2 crosspoints
static final int borderDistance=20; // how many pixel distance to the borderDistance
static final int sketchWidth = (borderDistance*2)+((verticalWires-1)*crosspointDistance);
static final int sketchHeight = ((horizontalWires+1)*crosspointDistance)+300;
static final int pictureWidth = (verticalWires-1)*crosspointDistance;
static final int pictureHeight = (horizontalWires-1)*crosspointDistance;
static final float signalPixelRatio = 0.02*1024; // (see crosspoint.pde)
final color textColor = color(60, 60, 60);
final color guiColor = color(180, 180, 180);
final color backgroundColor = color(240, 240, 240);
final color histogramColor = color(239, 171, 233);
final color signalColor = color(255, 0, 222);
final color wireColor = color(153, 233, 240);      // also used for blobs and histogram GUI fs
static final int AVERAGESIGNALCOUNTERMAX = 40;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
float contrastLeft = 0.0;
float contrastRight = 0.0;
// float contrastRight = 0.203125;
float blobThreshold = 0.535156;
float signalCutOff = 0.0;

int gridCrosspointX, gridCrosspointY;
GUIExtraSliders guiExtraSliders;
Crosspoint selectedCrosspoint;


int lastMillis, frames, packets, fps, pps;
String serialDebugger, serialDebuggerText; // used to monitor the serial communication in relation to draw() invocations
boolean bNewFrame; // only draw if there's new information (but at least every second)

void setup() {
  size(sketchWidth, sketchHeight, GLConstants.GLGRAPHICS);
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
  firmwareVersion = "unknown";
  dataManager = new DataManager();
  configurator = new Configurator(dataManager);
  initInterpolator();
  histogramGUI = new HistogramGUI(borderDistance, sketchHeight-160, 256);
  histogramGUI.setMarkerPositions(contrastLeft, contrastRight, blobThreshold);
  // guiEqualizer = new GUIEqualizer((borderDistance*2)+pictureWidth, borderDistance, verticalWires);
  configurator.helpText = "[r]eceive real data   [f]ake data (static)";
  textInformation = configurator.helpText;
  blobManager = new BlobManager(interpolator.pixelWidth, interpolator.pixelHeight, blobThreshold);
  flobManager = new FlobManager(this, interpolator.pixelWidth, interpolator.pixelHeight, blobThreshold);
  tuioServer = new TuioServer(new NetAddress("128.130.182.86", 3333), 12000);
  lastMillis = millis();
  bNewFrame = true;
  serialDebugger = "";
  serialDebuggerText = "";
  selectedCrosspoint = null;
  gridCrosspointX = 0;
  gridCrosspointY = 0;
  guiExtraSliders = new GUIExtraSliders(560, 620, 220);
}

int count = 0;
String georg = "start";

void drawSignals() {
  if (configurator.bFakeData) {
    interpolator.drawPicture(borderDistance, borderDistance);
    interpolator.drawHistogramFromPoint(sketchWidth-256-borderDistance, sketchHeight-30, 65);
    histogramGUI.draw();
  } 
  switch (configurator.visualizationType) {
  case 0:
    interpolator.interpolate(crosspoints);
    interpolator.drawPicture(borderDistance, borderDistance);
    interpolator.drawHistogramFromPoint(borderDistance, sketchHeight-160, 65);
    histogramGUI.draw();
    break;
  case 1:
    interpolator.interpolate(crosspoints);
    interpolator.drawPicture(borderDistance, borderDistance);
    interpolator.drawHistogramFromPoint(borderDistance, sketchHeight-160, 65);
    histogramGUI.draw();
    drawSignalCircles(true);
    drawGrid();
    break;
  default:
    break;
  }
  if (configurator.bShowBlobs) {
    blobManager.drawBlobs();
  }
  if (configurator.bShowFlobs) {
    flobManager.tuioServer = this.tuioServer;
    flobManager.drawFlobs();
  }
}

void draw() {
  frames++;
  serialDebugger = serialDebugger + ".";
  if ((millis() - lastMillis) > 1000) {
    lastMillis = millis();
    fps = frames;
    pps = packets;
    frames = 0;
    packets = 0;
    serialDebuggerText = serialDebugger;
    serialDebugger="";
  }
  background(backgroundColor);
  fill(textColor);
  text(textInformation, borderDistance, sketchHeight-60);
  text(fps+" fps", borderDistance, pictureHeight+(borderDistance*2));
  text(pps+" packets per second", 80, pictureHeight+(borderDistance*2));
  text(serialDebuggerText, borderDistance, pictureHeight+(borderDistance*2)+20);
  textAlign(RIGHT);
  text("firmware version: " + firmwareVersion, sketchWidth-borderDistance, pictureHeight+(borderDistance*2));
  textAlign(LEFT);
  if (configurator.bFakeData) {
    drawSignals();
  }
  if (null != dataManager && null != dataManager.port) {
    if (dataManager.port.available() != 0) {
      while (dataManager.port.available () > 0) {
        dataManager.serBuffer[count++] = (byte)dataManager.port.read();
        
        if (dataManager.receivingSettings) {
          if ('\n' == dataManager.serBuffer[count-1]) {
            dataManager.appendPotValues(new String(dataManager.serBuffer, 0, count-2));
            dataManager.receivePotValues();
            count = 0;
            break;
          } else if (count > 0 && 0 == count % dataManager.serBuffer.length) {
            dataManager.appendPotValues(new String(dataManager.serBuffer, 0, count));
            count = 0;
          }
        } else {
          if (count > 0 && 0 == count % dataManager.serBuffer.length) {
            packets++;
            dataManager.consumeSerialBuffer(null);
            count = 0;
            serialDebugger = serialDebugger + "|";
            break;
          }
        }
      }
    }
    drawSignals();
  }
  guiExtraSliders.draw();
  // draw the selected crosspoint
  if (selectedCrosspoint != null) {
    selectedCrosspoint.guiSlider.draw();
    stroke(signalColor);
    fill(signalColor);
    line(borderDistance, selectedCrosspoint.y, borderDistance+(crosspointDistance*(verticalWires-1)), selectedCrosspoint.y);
    line(selectedCrosspoint.x, borderDistance, selectedCrosspoint.x, borderDistance+(crosspointDistance*(horizontalWires-1)));    
    selectedCrosspoint.drawText();
    noStroke();
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
  noStroke();
}

void initSerial() {
  println(Serial.list());
  try {
    dataManager.calibrate(new Serial( this, Serial.list()[5], 300 ));
  }
  catch (Exception e) {
    textInformation = "error opening Serial connection: "+e;
    exit();
  }
}

void showHelpText() {
  textInformation = configurator.helpText;
}

void initInterpolator() {
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
  blobManager = new BlobManager(interpolator.pixelWidth, interpolator.pixelHeight, blobThreshold);
  flobManager = new FlobManager(this, interpolator.pixelWidth, interpolator.pixelHeight, blobThreshold);
}

void mousePressed() {
  if (selectedCrosspoint != null) {
    if (selectedCrosspoint.guiSlider.mousePressed() == false) {
      selectedCrosspoint = null; 
    }
  }
  if (histogramGUI.mousePressed()) return;
  if (guiExtraSliders.mousePressed()) return;
  if (averageSignalCounter == 0) {
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        if (crosspoints[i][j].isInside(mouseX, mouseY)) {
          textInformation = "digital pot for crosspoint: "+(i+1)+" "+(j+1);
          selectedCrosspoint = crosspoints[i][j];
          gridCrosspointX = i;
          gridCrosspointY = j;
        }
      }
    }
  }
}

void mouseDragged() {
  if (histogramGUI.mouseDragged(mouseX, mouseY)) {
    contrastLeft = histogramGUI.getValLeft();
    contrastRight = histogramGUI.getValRight();
    blobThreshold = histogramGUI.getValBlob();
    signalCutOff = histogramGUI.getValSignalCutOff();
      
    blobManager.setThreshold(blobThreshold);
    flobManager.setThreshold(blobThreshold);
    textInformation = "contrast stretch:   " + contrastLeft + "   " + contrastRight + "\nblob threshold: "+ blobThreshold + "   signalCutOff: " + signalCutOff + "\nback to the main [m]enu";
    bNewFrame = true;
    return;
  }
  if (guiExtraSliders.mouseDragged(mouseX, mouseY)) {
    bNewFrame = true;
    return; 
  }
  if (selectedCrosspoint != null) {
    if (selectedCrosspoint.guiSlider.mouseDragged(mouseX, mouseY)) {
      dataManager.sendDotMatrixCorrectionData(gridCrosspointX, gridCrosspointY, selectedCrosspoint.guiSlider.value);
      dataManager.bPotValueHasChanged = true;
    }
    bNewFrame = true;
    return;
  }
}

void mouseReleased() {
  histogramGUI.mouseReleased();
  guiExtraSliders.mouseReleased();
  dataManager.mouseReleased(); // storing pot values
  bNewFrame = true;
}

void keyPressed() {
  configurator.changeConfiguration(key);
  bNewFrame = true;
}

void stop() {
  dataManager.port.clear();
  dataManager.port.stop();
  super.stop();
}

