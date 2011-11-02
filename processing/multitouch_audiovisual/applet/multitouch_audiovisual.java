import processing.core.*; 
import processing.xml.*; 

import processing.opengl.*; 
import processing.serial.*; 
import java.text.DecimalFormat; 
import blobDetection.*; 
import codeanticode.glgraphics.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class multitouch_audiovisual extends PApplet {





 // http://glgraphics.sourceforge.net

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
static final float signalPixelRatio = 0.02f*1024; // (see crosspoint.pde)
final int textColor = color(60, 60, 60);
final int guiColor = color(180, 180, 180);
final int backgroundColor = color(240, 240, 240);
final int histogramColor = color(239, 171, 233);
final int signalColor = color(153, 233, 240);
final int wireColor = color(0, 222, 255);
static final int AVERAGESIGNALCOUNTERMAX = 150;
int averageSignalCounter = AVERAGESIGNALCOUNTERMAX;
float contrastLeft = 0.0f;
float contrastRight = 0.0f;

int lastMillis, frames, packets, fps, pps;
boolean bNewFrame; // only draw if there's new information

public void setup() {
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

public void draw() {
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

public void drawSignalCircles(boolean bDrawText) {
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

public void drawGrid() {
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

public void initSerial() {
  try {
    dataManager.calibrate(new Serial( this, Serial.list()[0], 230400 ));
  }
  catch (Exception e) {
    textInformation = "error opening Serial connection: "+e;
    println(Serial.list());
    exit();
  }
  textInformation = "Serial connection initalized.";
}

public void serialEvent(Serial p) {
  dataManager.consumeSerialBuffer(p);
  packets++;
  bNewFrame = true;
}

public void showHelpText() {
  textInformation = configurator.helpText;
}

public void initInterpolator() {
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

public void mousePressed() {
  histogramGUI.mousePressed();
}

public void mouseDragged() {
  histogramGUI.mouseDragged(mouseX, mouseY);
  contrastLeft = histogramGUI.getValLeft();
  contrastRight = histogramGUI.getValRight();
  textInformation = "contrast stretch:   " + contrastLeft + "   " + contrastRight;
  bNewFrame = true;
}

public void mouseReleased() {
  histogramGUI.mouseReleased();
  bNewFrame = true;
}

public void keyPressed() {
  configurator.changeConfiguration(key);
  bNewFrame = true;
}

class BlobManager {
  BlobDetection blobDetection;

  BlobManager(int pixelWidth, int pixelHeight) {
    this.blobDetection = new BlobDetection(pixelWidth, pixelHeight);
    this.blobDetection.setPosDiscrimination(false); // detect dark areas
    this.blobDetection.setThreshold(0.9f); // will detect dark areas whose luminosity > 0.2f;
  }

  public void drawBlobs() {
    this.blobDetection.computeBlobs(picture.pixels);
    drawBlobsAndEdges(true, true, interpolator.resizedWidth, interpolator.resizedHeight, borderDistance, borderDistance);
  }

  // this is copied from the blobDetection bd_webcam example
  // some details have been changed
  public void drawBlobsAndEdges(boolean drawBlobs, boolean drawEdges, int w, int h, int x, int y) {
    noFill();
    Blob b;
    EdgeVertex eA, eB;
    for (int n=0 ; n<blobDetection.getBlobNb() ; n++) {
      b=blobDetection.getBlob(n);
      if (b!=null) {
        // Edges
        if (drawEdges) {
          strokeWeight(3);
          stroke(0, 255, 0);
          for (int m=0;m<b.getEdgeNb();m++) {
            eA = b.getEdgeVertexA(m);
            eB = b.getEdgeVertexB(m);
            if (eA !=null && eB !=null)
              line(
              x+ (eA.x*w), y+ (eA.y*h), 
              x+ (eB.x*w), y+ (eB.y*h)
                );
          }
        }
        // Blobs
        if (drawBlobs) {
          strokeWeight(1);
          stroke(255, 0, 0);
          rect(x+ (b.xMin*w), y+ (b.yMin*h), b.w*w, b.h*h);
        }
      }
    }
  }
}
class Configurator {
  int visualizationType;
  boolean bDebug, bContrastStretch, bShowCalibrated, bFakeData;
  String helpText;
  DataManager dataManager;

  Configurator(DataManager dataManager) {
    visualizationType = 0;            // which type of visualitazion should be used (0-2)
    bDebug = false;                       // stop updating and print out some debug data
    bContrastStretch = true;              // show signal strength after contrast stretch?
    bShowCalibrated = true;               // show signal strength after calibration?
    bFakeData = false;                    // use fake data for "offline" testing?
    helpText = "";
    this.dataManager = dataManager;
  }

  public void changeConfiguration(char key) {
  switch(key) {
    case 'r':
      if (averageSignalCounter == 0) {
        // recalibrate
        textInformation = "recalibrating";
        averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
      }
      else {
        initSerial();
        configurator.helpText = "[c]ontrast stretch   [d]ebug   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution\n[r]ecalibrate   [u]calibrate(ON/OFF)   [v]isualization";
      }
      break;
    case 'f':
      // use fake data
      dataManager.initFakeData();
      configurator.helpText = "[c]ontrast stretch   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution   [u]calibrate(ON/OFF)   [v]isualization";
      textInformation = configurator.helpText;
      break;
    case 'h':
      // help
      textInformation = helpText;
      break;
    case 'd':
      // debug
      this.bDebug = !bDebug;
      if (this.bDebug) {
        textInformation = "[d]ebug: " + getOnOffString(this.bDebug);
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
      this.bContrastStretch = !this.bContrastStretch;
      interpolator.bContrastStretch = this.bContrastStretch;
      textInformation = "[c]ontrast stretch: " + getOnOffString(this.bContrastStretch);
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
        ip.tension += 0.1f;
        ip.tension = constrain((float)ip.tension, -2.0f, 2.0f);
        textInformation = interpolator.name + " x" + interpolationResolution;
      }
      break;
    case 'l':
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        ip.tension -= 0.1f;
        ip.tension = constrain((float)ip.tension, -2.0f, 2.0f);
        textInformation = interpolator.name + " x" + interpolationResolution;
      }
      break;
    case 'u':
      this.bShowCalibrated = !this.bShowCalibrated;
      break;
    default:
      break;
    }
  }

  public String getOnOffString(boolean b) {
    String s = "OFF";
    if (b) s = "ON";
    return s;
  }
}

class Crosspoint {
  int x, y;
  double measuredSignal;          // the actual received values
  double measuredSignalAverage;
  double signalStrength;          // the calculated signal strength (ca. 0.0-1.0)
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;

  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.measuredSignalAverage = 0;
    this.signalStrength = 1;
  }

  public void draw(boolean bDrawText) {
    noFill();
    stroke(signalColor);
    ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    fill(wireColor);
    if (bDrawText) {
      // text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.measuredSignalAverage)+"\n"+this.signalMin), x+4, y-4);
    }
  }

  public void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    this.signalStrength = (double) msr / (this.measuredSignalAverage+1);
    if (msr > this.signalMax) this.signalMax = msr;
    if (msr < this.signalMin) this.signalMin = msr;
    if (this.signalStrength > this.signalPMax) this.signalPMax = this.signalStrength;
    if (this.signalStrength < this.signalPMin) this.signalPMin = this.signalStrength;
  }

  public void accumulateAvgSig(int val) {
    this.measuredSignalAverage = this.measuredSignalAverage/2 + (val/2);
  }
}

class DataManager {
  Serial port;
  String fakeData;
  byte[] serBuffer = null;

  DataManager() {
  }

  public void calibrate(Serial port) {
    this.port = port;
    try {
      this.port.clear(); // // do we need this? (cargo cult programming)
      this.serBuffer = new byte[(horizontalWires * verticalWires * 10)/8];
      this.port.buffer(serBuffer.length);
      this.port.write('s');
      textInformation = "starting to calibrate";
    } 
    catch (Exception e) {
      textInformation = "error with Serial connection: "+e;
      println(Serial.list());
    }
    delay(2000); // still needed?
  }

  public void initFakeData() {
    // show the hand
    fakeData = "0.36960868562487353,0.46774542827389526,0.6732497277525261,0.9212308538357995,1.020992674024366,0.9399522953272663,0.9021279625806475,0.8446045696630836,1.1322533338303469,0.9999868783556622,0.888888900846611,0.9658200583893635,0.8241063240218811,0.9820028696458293,0.9905032707792029,1.034973467503874,0.9201551927119068,0.9188082292871105,0.9570814869583913,0.9340903032041523,1.0731269153333134,0.6142824478453107,0.9459488296442711,0.9303598080809076,0.8863086570056429,0.9271283606651384,0.9541916538376554,0.8908459943095389,0.991523310749273,0.9196138009895852,0.8333219703201196,0.9994353065364702,0.8058432833418463,0.7815923381241355,0.8504345008558043,1.0020658954156796,0.8691511511217467,0.8944908094368557,0.852500344558712,0.8959052212384784,0.9830217878163239,0.8829006579802932,0.8608219599617144,0.9997421820194012,0.8882882583781516,0.9478525188410322,0.904716590339513,0.896548858581317,0.9216495568285111,0.8877376715609621,0.9505727381495639,0.899421766460953,0.8497721261089168,0.9994513352041766,0.7769242404415374,0.7625798808301101,0.8215473358359605,1.0,0.8952447311509031,0.8773706442140097,0.8543393255120649,0.8826091595733587,0.9804947208136635,0.9120762228579737,0.9027369491700432,1.0005114659853986,0.879602631977946,0.9443078275192274,0.9099265295073203,0.9144884799686608,0.923537079260172,0.9007796688351212,0.9572411415643645,0.9142618385443764,0.9007907480504662,0.9997391809272195,0.857967038149308,0.7863913344208515,0.8225520880861159,1.0000033909052521,0.8735713259440537,0.8918384934891963,0.8777647014245605,0.8826833743946191,0.9724277644396313,0.9094410578805873,0.8776498138355286,0.9999665161184014,0.8749756747536301,0.9562082520037104,0.9088392580433974,0.8983558697313884,0.9422673525887513,0.9205373802750462,0.9115203566990939,0.9294880790303416,0.8717641693380525,0.9995483654424419,0.8072741033536549,0.7338984444549274,0.754320496394619,1.0,0.886943538634356,0.9089541153577443,0.9178057692050281,0.911315043509777,0.9577872944975071,0.9366931382396856,0.9129894535048177,0.998909678881118,0.8558953796251224,0.9201123829753735,0.8545113955723982,0.8963184826846944,0.9901226209641754,0.9201747617204662,0.9126174601330138,0.9253989066258651,0.8486707545441182,0.9998656743704054,0.6976107296277774,0.5517675407845422,0.6470539805391369,0.8888888888888888,0.8430070073052914,0.9078339799491641,0.8781794073923592,0.9117714562640502,0.9639058921553996,0.9584461416981009,0.9757254010124055,1.0005025190820838,0.9196806050319043,0.9405826875101455,0.9291007490427069,0.9274546727022908,0.9827275882600427,0.9638401152976926,0.9824710671829661,0.9797540701770329,0.9287011851618419,0.9999714271586662,0.6655918195799396,0.44229230676204606,0.40840427536155943,0.8888888888888888,0.795910760367298,0.9076236220035596,0.9155893772514881,0.9618488315400324,1.0711053596255917,1.0161049793935586,1.0006144607878904,0.99897498692394,0.889148940416442,0.9303344262032124,0.9260643441693533,0.91442336756135,0.9694612918097382,0.972020156017196,0.9544627891262474,0.9579553404195281,0.8822469959893608,1.0009273319522098,0.7144969203219261,0.5380266331214071,0.4727114504933809,0.8888888888889788,0.6743414047783495,0.8176199517270972,0.8383075656504596,0.9105602944773314,1.0003830150061206,0.9437889468772408,0.9554527608208556,0.9997379351470869,0.855996037677496,0.9201707448799206,0.8689545387616274,0.8922559631143816,0.9779132416978886,0.9606346764350696,0.9800465183645317,0.9454866456664506,0.9348087774573413,0.9991985022606488,0.7784906655010355,0.7251585118589485,0.7042537110950944,1.0000000033113834,0.7254521189176923,0.7989503771749241,0.687312328119331,0.7680160779944278,0.8525702092959925,0.8895016878329022,0.923390611046648,0.9997386854522264,0.8781553514788695,0.920654542268379,0.988598565222416,0.9284295831688858,0.9648089457779332,0.9740587634480292,0.9577218094952525,0.969217166093919,0.9876379257647631,1.000877664651178,0.8841214215385347,0.8164918985251739,0.8053025039015019,1.0000001192093038,0.7614707549365608,0.7514627147285415,0.524557816040623,0.5846450282210566,0.6679981530889201,0.78467628673231,0.9069220102216674,0.9997403088269823,0.8892480847544296,0.9166752198227123,0.9693236708638477,0.9256592597349597,0.9753107365733733,0.9872570950698446,0.9781993651482629,0.9873913765963542,0.9199594241871311,1.0004125255174827,0.8605004086110114,0.8507755097907734,0.8234521957937644,1.0000001059638242,0.7079614179749909,0.6140589600554559,0.2589985095909098,0.4337680678747311,0.4462623529876468,0.5816669654469033,0.7052755703910805,1.0004469995210457,0.8560520869639991,0.9307254011385846,0.9314227217250164,0.9108665571985747,0.9899198936058684,0.9805823242826993,0.910414313465182,0.9186112969198096,0.8246484840277106,1.000002663820343,0.8260729885353033,0.8210753800674353,0.8335275402672333,0.7777777777878384,0.6052071492942211,0.5436993841616077,0.2606419797314072,0.4712725040087454,0.48675361124739674,0.5433183157976523,0.557754331168211,0.9999587686765293,0.8361200978645221,0.9122422938016039,0.8033772546920883,0.724096920684,0.7371385660275575,0.763883448632014,0.704377820240384,0.7604966688049812,0.5708315487236398,0.999998291682716,0.7314428798090273,0.7086670316709633,0.6665677747661692,0.8888888888888888,0.5049173000526893,0.629483739950451,0.4602471973303917,0.581362766128676,0.6124881202659733,0.6080355911595157,0.6070661465086938,1.0010235258040534,0.8118575218026722,0.89860911473258,0.7951328847548672,0.633533382867114,0.5709880106729605,0.606909998143764,0.5423964444961572,0.6276920823729992,0.469157068803108,0.9994863262553493,0.7734937400352268,0.6248206344272663,0.5564915694784803,1.0286001283517956,0.6266587432746239,0.8308004602420541,0.8385843221494175,0.7856294364053211,0.7298314260604714,0.6592487388636094,0.6164366272347571,0.9998081030424532,0.8114873620630245,0.9048350572225464,0.860908503392999,0.8155883252623901,0.8654860990510876,0.8760146908106419,0.79068213542631,0.819207217273512,0.7071371409593143,0.9999977426199577,0.6624637404640237,0.5654684453249202,0.4708530446466791,1.0000000000004041,0.7654294915171879,0.8881856524189387,0.9143236760474824,0.9186197897736975,0.8406204299701048,0.6154155670141676,0.4274547456783718,0.994850471531004,0.8546523987113285,0.9173777215786385,0.8417211513016947,0.9218078046928359,1.0170571152272916,0.9790708668473264,0.9120392105262142,0.8659531424026491,0.7181589175319583,0.999727836184789,0.7457329526953502,0.7394180179043301,0.6254513972929929,0.8888888888889002,0.73716084601881,0.9089733685399409,0.871677035853764,0.8422694228048372,0.7390783124512438,0.5284742257709417,0.3167584240453014,0.86283772265719,0.8932262647179822,0.9249368017767313,0.9464637168086946,0.9224267819747844,0.9682816818194957,0.803469750280345,0.7930386141072804,0.7649666022842004,0.41576439656419273,0.9996168100758878,0.7298111920903924,0.8575476921328337,0.8120768888022477,1.0000067817397207,0.802445105430961,0.8600531026316065,0.7448759679298878,0.6943707329635986,0.5748261560081486,0.5830145074338784,0.4630371022005298,0.9380087750348659,0.8207782545307098,0.9030120465602509,0.6854210334503211,0.6720582230152989,0.5800430477096333,0.587630586316244,0.6908055033117749,0.7743297150847118,0.683172697537966,1.000349254282474,0.8167080468572003,0.8829566553571653,0.8719744193284449,0.8888888888888888,0.6830212825571603,0.7437195833896807,0.46852773873569253,0.576878579912552,0.6050340421351815,0.6908157197535695,0.7981417800333498,0.9999838979507661,0.805316568984665,0.8601467510096137,0.6403172789354574,0.6008821725367438,0.6467235427253013,0.75030466887234,0.8289221315085237,0.8603200898630857,0.7449327389227426,0.999838971125764,0.58474513420197,0.7473353212954419,0.7527472981403043,0.7777777777777778,0.5769796807864783,0.6025404482762734,0.3527332363026334,0.562185798966178,0.660010660342149,0.8687756332441248,0.9594669489887229,0.9989775031603942,0.8320913208978944,0.9283627987675258,0.795894067469099,0.8357288035990785,0.8720550411524712,0.8808631915911095,0.7453904724840789,0.698954828093218,0.4188218646127741,0.9988817532469779,0.4112043749976807,0.6804392412942731,0.7829201099070069,1.0000000000032339,0.5538725535999098,0.5875705591290813,0.4568053854450052,0.7514614223086922,0.9843136769084588,1.0598181714037536,1.0794967238860085,0.999928035590475,0.8851023949659044,0.9590878547467627,0.9510069298835286,0.9669092542277145,0.9620929526344443,0.765808471013959,0.5624895153138614,0.5868453392589018,0.3511511207512118,0.9986886636261832,0.6428094232416971,0.7566510612287068,0.7002481265975672,0.7777777777777778,0.7269905329914176,0.7596419167062284,0.8650309924094072,0.9773567449651639,1.115188376672888,1.146030015033741,1.1620549123050221,0.9999960013188824,0.8936179443161478,0.9744347059054916,0.9403962046642881,0.9643724042626398,0.8881770644000827,0.6941477376840787,0.6475219241300848,0.769231456333306,0.7470437137594872,1.0010177763845352,0.6693075141120873,0.5365732592371759,0.4570092746916653,0.7777777777777778,0.8555505641225328,0.8933081124272894,0.9290053212512722,0.9748482123325652,1.119672760282315,1.0978502272610813,1.0950718479802264,0.9998730729791037,0.8730760696012985,0.9381611097259847,0.8703045662652785,0.933414709442204,0.9440195457850985,0.8580751911556825,0.8267227412306426,0.8709286885056514,0.8307315463889512,0.9999319587887986,0.443426771253839,0.39257549363101235,0.5355031412160596,0.888889265649452,0.8602611230753473,0.9224804084258161,0.9526893519397189,0.9870394753004373,1.109001195318501,1.0507382233658535,1.0076827660964705,1.000959321082342,0.9182426989992233,0.9507176669393299,0.9628098235058203,0.9471886550315345,1.0717898535121773,0.9920732346275669,0.9864663318551484,0.9588152820280607,0.8323133501117057,0.9997352762491427,0.16510733494194096,0.4913629258024472,0.7535365046980689,0.8888888888888945,0.9470987930093953,0.9149717547105587,0.9856534039314776,1.007719811782475,1.1168009606749063,1.136605335108648,1.05614432075498,0.9999340130897388,0.9032542709913405,0.9139656252158258,0.9105480365894457,0.8954465301141727,0.9569977807481284,0.9238701318722797,1.0152657360790531,0.9031464223521563,0.7018043854788955,0.9991251581990398,0.5204230444421052,0.6990569360283879,0.8248979566754856,0.8900629153016781,0.8569767274418666,0.8755615115477705,0.9385204454090301,0.9992988496189472,1.021072253476836,1.0270587945920087,1.0903063965716535,0.9393985091634313,0.8985305368892321,0.916145458472142,0.8181572429897588,0.9010945915345633,0.9553743255774064,0.8885687332244311,0.9580212297258655,0.9127273120791193,0.7808992679816972,0.9998395870375576,0.7378423816493284,0.8073922823947233,0.890586641011195,1.0000016954238815,0.8878870126127496,0.8945305695782453,0.9195567765068772,0.9856403306690017,1.0140970399530447,0.9642870335514789,0.9905116633155843,0.9998718707020352,0.9157217718726443,0.960966495554902,0.9001892533634386,0.9177262882216173,0.984824767078031,0.9126666670024026,0.9025460711349842,0.9244967946880606,0.8629862597947364,0.9999578946579578,0.8123628489953075,0.8105484019779602,0.877267761383836,0.8889009457475159,0.8909428153453841,0.9181043235561348,0.9248032142534349,0.9252812040229837,1.0196119644111663,0.9455041232874282,0.9424339426433459,0.9997122117501809,0.8776107111046152,0.9334180725276906,0.8785126371128849,0.894803502000817,0.9072275389474019,0.9059748182377988,0.8481238481972916,0.8624276843975509,0.8213652831223341,1.0003190851485995,0.7949445256744327,0.7865659013780425,0.8504709675928881,1.0,0.8628447306887217,0.8598585071274539,0.8904443472410796,0.8669045065707882,0.9353527164221634,0.9012046462353939,0.880863125691765,1.0008954666054384,0.8673177951241839,0.9825644231870241,1.0000342352228893,0.9088590143015443,0.9113798812707498,0.9217278664340093,0.8876197529520463,0.8905034676987185,0.8916397661763961,1.0009990356523322,0.8394763578306567,0.8399970455944694,0.874471647406231,1.0,0.943135566799324,0.8798513922649595,0.8835863940355918,0.8986816096322592,0.8873052737242818,0.9384399316619743,0.9311200464198681,0.999581346743933,0.8128336480714088,0.9997110877907838,0.9989674679644456,0.8927234455222405,0.5827778244327517,0.9213043173334404,0.9218927839008455,0.8585538915379471,0.8313920346799145,0.9997279957444274,0.7957658856812208,0.8252495585367967,0.8409957266444008,1.0,0.9151422041140843,0.8786370876450351,0.880561701033367,0.8933693025580636,0.9306703125288728,0.9200874449218709,0.9270743910537963,1.0009894566099444,0.8906634473561609,1.001005185899239,0.9994569697363003,0.9303345433380923,0.7499683159478285,0.9999999997550747,0.882419744370565,0.8061349595401165,0.8681075117693465,0.9995963240777986,0.8113665282086069,1.0000004528917896,0.9989754808590958,1.0,0.9999200422167391,0.9992168924422109,1.0010075325449965,1.0008315904118097,0.9994004877442484,0.9992129090361772,0.9997441798528287,1.000890872985855,0.8164247063411938,0.9989774094877895,0.9987020151675039,0.9275421033375901,0.7534815566573798,0.9997381819845135,0.9324466135689699,0.8195927127646784,0.8352366401948194,0.9997076908491044,0.8369847904763701,0.9999993517901344,0.9999335329747214,1.0,0.9999979331792767,0.9999873612335757,1.0010215244095708,0.9982159577716166,0.9999840072999694,0.9989452992038124,0.9999999990238669,0.9999770039645924,";
    this.parseFakeData();
  }

  public int sb2ub(byte p) {
    return p < 0 ? 256+p : p;
  }

  public void consumeSerialBuffer(Serial p) {
    p.readBytes(serBuffer);
    int bs = 0, br = 0, cnt = 0;
    for (int i=0; i<serBuffer.length; i++) {
      br |= sb2ub(serBuffer[i]) << bs;
      bs += 8;
      while (bs >= 10) {          
        int sig = br & 0x3ff;
        br >>= 10;
        bs -= 10;          
        int px = cnt / horizontalWires, py = cnt % horizontalWires;  
        if (averageSignalCounter > 0) {
          // calculate the average signal strength for every crosspoint
          crosspoints[px][py].accumulateAvgSig(sig);
        } 
        else {
          crosspoints[px][py].setSignalStrength(sig);
        }
        cnt++;
      }
      calibrationFeedback();
    }
  }

  // TODO: this code needs some serious refactoring...
  public void calibrationFeedback() {
    if (averageSignalCounter > 0) {
      averageSignalCounter--;
      if (averageSignalCounter != 0) {
        textInformation = "calibrating: "+averageSignalCounter;
      }
      else {
        showHelpText();
      }
    }
  }

  public void parseFakeData() {
    fakeData = trim(fakeData);
    float data[] = PApplet.parseFloat(split(fakeData, ','));
    int k = 0;
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        crosspoints[i][j].signalStrength = data[k];
        k++;
      }
    }
  }

  public void printData() {
    println("measured signals:");
    String myString = "";
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        myString = myString+(int) crosspoints[i][j].measuredSignal+",";
      }
    }
    println(myString);
    println("calculated signal strength:");
    myString = "";
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        myString = myString+crosspoints[i][j].signalStrength+",";
      }
    }
    println(myString);
  }
}

class HistogramGUI {
  private int x, y;
  private int width;
  public GUITriangle triangleLeft, triangleRight;

  HistogramGUI (int x, int y, int width) {
    this.x = x;
    this.y = y;
    this.width = width;
    triangleLeft = new GUITriangle(this.x, this.y);
    triangleRight = new GUITriangle(this.x+this.width, this.y);
  }

  public void mousePressed() {
    triangleLeft.mousePressed();
    triangleRight.mousePressed();
  }

  public void mouseDragged(int mX, int mY) {
    triangleLeft.mouseDragged(constrain(mX, this.x, triangleRight.x-13), mY);
    triangleRight.mouseDragged(constrain(mX, triangleLeft.x+13, this.x+this.width), mY);
  }

  public void mouseReleased() {
    triangleLeft.mouseReleased();
    triangleRight.mouseReleased();
  }

  public void draw() {        
    triangleLeft.draw();
    triangleRight.draw();
  }

  public void setMarkerPositions(float valLeft, float valRight) {
    triangleLeft.x = this.x + (int) (valLeft * this.width);
    triangleRight.x = this.x + this.width - (int) (valRight * this.width);
  }

  public float getValLeft() {
    return (float) (triangleLeft.x - this.x) / (float) this.width;
  }

  public float getValRight() {
    return 1.0f - (float) (triangleRight.x - this.x) / (float) this.width;
  }
}

class GUITriangle {
  // a triangle inside a 13x12 pixel rectangle
  int x, y;  // the position of the edge pointing to the histogram
  boolean bTracked;

  GUITriangle (int x, int y) {
    this.x = x;
    this.y = y;
    this.bTracked = false;
  }

  public void draw() {
    fill(wireColor);
    noStroke();
    triangle(this.x-6, this.y+12, this.x, this.y, this.x+6, this.y+12);
  }

  public boolean isInside(int x, int y) {
    return ((x > this.x - 6) && (x < this.x + 6) && (y > this.y) && (y < this.y + 12));
  }

  public void mousePressed() {
    this.bTracked = this.isInside(mouseX, mouseY);
  }

  public void mouseDragged(int mX, int mY) {
    if (this.bTracked) {
      this.x = mX;
    }
  }

  public void mouseReleased() {
    this.bTracked = false;
  }
}

class Interpolator
{
  static final int histogramBins = 256;

  boolean bContrastStretch; 

  int horizontalSamples, verticalSamples;
  int pixelWidth, pixelHeight;   // the actual pixel count of the interpolated image
  int horizontalMultiplier, verticalMultiplier;   // used to calculate pixelWidth and pixelHeight
  int resizedWidth, resizedHeight;  // the resized, blown up image
  float _fx, _fy;
  double[] interpolPixels;
  int[] _hist;
  int _histMax;
  String name;
  
  Interpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight, String name)
  {
    // TODO: simplify (use picture)
    this.horizontalSamples = horizontalSamples;
    this.verticalSamples = verticalSamples;
    this.horizontalMultiplier = horizontalMultiplier;
    this.verticalMultiplier = verticalMultiplier;
    this.pixelWidth = (horizontalSamples - 1) * horizontalMultiplier;
    this.pixelHeight = (verticalSamples - 1) * verticalMultiplier;
    this.resizedWidth = imageWidth;
    this.resizedHeight = imageHeight;

    this._fx = 1.0f / (2.0f * (float)horizontalMultiplier);
    this._fy = 1.0f / (2.0f * (float)verticalMultiplier);

    this.interpolPixels = new double[pixelWidth * pixelHeight];
    this._hist = new int[histogramBins];

    this.bContrastStretch = false;
    this.name = name;
  }

  // includes value repetition at the borders
  public double sensorVal(Crosspoint[][] cp, int x, int y)
  {
    x = constrain(x, 0, horizontalSamples-1);
    y = constrain(y, 0, verticalSamples-1);

    return cp[x][y].signalStrength;
  }
  
  public void updatePicture() {
    picture.loadPixels();
    for (int i=0; i<interpolPixels.length; i++) {
      picture.pixels[i] = color((float)interpolPixels[i]*255.0f);
    }
    picture.loadTexture();
  }

  public void drawPicture(int x, int y) {
    updatePicture();
    picture.render(x, y, resizedWidth, resizedHeight);
  }

  public void drawHistogramFromPoint(int x1, int y1, int maxY)
  {
    float step = (float)maxY / (float)log(this._histMax);

    strokeWeight(1.0f);
    for (int i=0; i<histogramBins; i++) {
      int h = (int)(constrain(log(this._hist[i]) * step, 0.0f, maxY));

      stroke(color(i));
      line(x1+i, y1, x1+i, y1-maxY);  

      stroke(histogramColor);
      line(x1+i, y1, x1+i, y1-h);
    }
  }

  public void interpolate(Crosspoint[][] cp)
  {
    beginInterpolation(cp);

    Arrays.fill(this._hist, 0);
    this._histMax = 0;
    for (int i=0; i<horizontalSamples-1; i++)
      for (int j=0; j<verticalSamples-1; j++) {
        beginInterpolate4(cp, i, j);

        for (int k=0; k<horizontalMultiplier; k++)
          for (int l=0; l<verticalMultiplier; l++) {
            double val =  interpolate4(cp, i, j, k, l, _fx * (1.0f + 2*k), _fy * (1.0f + 2*l));
            val = constrain((float)val, 0.0f, 1.0f);

            int p = (int)(val/(1.0f/((double)histogramBins-1)));
            _hist[p]++;
            if (_hist[p] > this._histMax)
              this._histMax = _hist[p];

            this.interpolPixels[((j * verticalMultiplier + l) * pixelWidth) + i * horizontalMultiplier + k] = val;
          }

        finishInterpolate4(cp, i, j);
      }

    finishInterpolation(cp);

    if (this.bContrastStretch) {
      float vmin = 0.0f, vmax = 1.0f;

      int ss, nHist = (int) (this.pixelWidth * this.pixelHeight * contrastLeft);
      for (ss = 0; nHist > 0 && ss<histogramBins; nHist -= this._hist[ss++]);
      vmin = (1.0f/histogramBins) * ss;

      nHist = (int)(this.pixelWidth * this.pixelHeight * contrastRight);
      for (ss = histogramBins-1; nHist > 0 && ss>=0; nHist -= this._hist[ss--]);
      vmax = (1.0f/histogramBins) * ss;

      for (int i=0; i<pixelWidth; i++)
        for (int j=0; j<pixelHeight; j++) {
          double p = interpolPixels[j * pixelWidth + i];
          interpolPixels[j * pixelWidth + i] = (p - vmin) / (vmax - vmin);
        }
    }
  }

  public void beginInterpolation(Crosspoint[][] cp)
  {
    // override in subclass if necessary
  }

  public void finishInterpolation(Crosspoint[][] cp)
  {
    // override in subclass if necessary
  }

  public void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    // override in subclass if necessary
  }

  public void finishInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    // override in subclass if necessary
  }

  public double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
  {
    // override in subclass to implement 2x2 interpolation
    return 0;
  }
}

class Interpolator2x2 extends Interpolator
{
  double a, b, c, d;

  Interpolator2x2(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight, String name)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, name);
  }

  public double interp_func(double a, double b, float f)
  {
    // implement in subclass
    return 0;
  }

  public void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    a = sensorVal(cp, xm, ym);
    b = sensorVal(cp, xm+1, ym);
    c = sensorVal(cp, xm, ym+1);
    d = sensorVal(cp, xm+1, ym+1);
  }

  public double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
  {
    double i1 = interp_func(a, b, fx);
    double i2 = interp_func(c, d, fx);

    return interp_func(i1, i2, fy);
  }
}

class Interpolator4x4 extends Interpolator
{
  double a, b, c, d;
  double t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

  /*
    *    t0    t1    t2    t3
   *    t4    a     b     t5
   *    t6    c     d     t7
   *    t8    t9    t10   t11
   */

  Interpolator4x4(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight, String name)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, name);
  }

  public void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    a = sensorVal(cp, xm, ym);      
    b = sensorVal(cp, xm+1, ym);      
    c = sensorVal(cp, xm, ym+1); 
    d = sensorVal(cp, xm+1, ym+1);

    t0 = sensorVal(cp, xm-1, ym-1);
    t1 = sensorVal(cp, xm, ym-1);
    t2 = sensorVal(cp, xm+1, ym-1);
    t3 = sensorVal(cp, xm+2, ym-1);
    t4 = sensorVal(cp, xm-1, ym);
    t5 = sensorVal(cp, xm+2, ym);
    t6 = sensorVal(cp, xm-1, ym+1);
    t7 = sensorVal(cp, xm+2, ym+1);
    t8 = sensorVal(cp, xm-1, ym+1);
    t9 = sensorVal(cp, xm, ym+1);
    t10 = sensorVal(cp, xm+1, ym+1);
    t11 = sensorVal(cp, xm+2, ym+1);
  }

  public double interp_func(double p0, double p1, double p2, double p3, double f)
  {
    // implement in subclass
    return 0;
  }

  public double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
  {
    double y0 = interp_func(t0, t1, t2, t3, fx);
    double y1 = interp_func(t4, a, b, t5, fx);
    double y2 = interp_func(t6, c, d, t7, fx);
    double y3 = interp_func(t8, t9, t10, t11, fx);

    return interp_func(y0, y1, y2, y3, fy);
  }
}

class CatmullRomInterpolator extends Interpolator4x4
{
  CatmullRomInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "catmull-rom spline interpolation");
  }

  public double interp_func(double p0, double p1, double p2, double p3, double f)
  {
    double f2 = f*f;
    double i0 = -.5f*p0 + 1.5f*p1 - 1.5f*p2 + .5f*p3;
    double i1 = p0 - 2.5f*p1 + 2*p2 - 0.5f*p3;
    double i2 = -.5f*p0 + 0.5f*p2;

    return i0*f*f2 + i1*f2 + i2*f + p1;
  }
}

class CosineInterpolator extends Interpolator2x2
{   
  CosineInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "cosine interpolation");
  }

  public double interp_func(double a, double b, float f)
  {
    double f2 = (1.0f-cos(f*PI))/2.0f;
    return a*(1.0f-f2) + b*f2;
  }
}

class CubicInterpolator extends Interpolator4x4
{
  CubicInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "cubic interpolation");
  }

  public double interp_func(double p0, double p1, double p2, double p3, double f)
  {
    double f2 = f*f;
    double i0 = p3 - p2 - p0 + p1;
    double i1 = p0 - p1 - i0;
    double i2 = p2 - p0;

    return i0*f*f2 + i1*f2 + i2*f + p1;
  }
}

class HermiteInterpolator extends Interpolator4x4
{
  public double tension, bias;
  DecimalFormat _df;

  HermiteInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "hermite interpolation [k][l]");
    this.bias = 0;
    this.tension = 0;
    this._df = new DecimalFormat("0.0");
  }

  public String name()
  {
    return this.name + " (t=" + _df.format(tension) + ")";
  }

  public double interp_func(double p0, double p1, double p2, double p3, double f)
  {
    double m0, m1, f2 = f*f, f3 = f2*f;

    m0 = (p1-p0)*(1.0f+bias)*(1.0f-tension)/2;
    m1 = (p2-p1)*(1.0f+bias)*(1.0f-tension)/2;
    m0 += m1;
    m1 += (p3-p2)*(1.0f+bias)*(1.0f-tension)/2;

    double i0 = 2*f3 - 3*f2 + 1;
    double i1 = f3 - 2*f2 + f;
    double i2 = f3 - f2;
    double i3 = -2.0f*f3 + 3*f2;

    return (i0*p1 + i1*m0 + i2*m1 + i3*p2);
  }
}

class LinearInterpolator extends Interpolator2x2
{   
  LinearInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "linear interpolation");
  }

  public double interp_func(double a, double b, float f)
  {
    return a*(1.0f-f) + b*f;
  }
}

  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#FFFFFF", "multitouch_audiovisual" });
  }
}
