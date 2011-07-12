import arb.soundcipher.*; // get it at http://soundcipher.org
import processing.serial.*;
import java.text.DecimalFormat;

Serial myPort;
Crosspoint[][] crosspoints;
PFont myFont;
SoundCipher sc;
ArrayList notes;
String textInformation;

// configuration
int verticalWires = 16;
int horizontalWires = 4;
int crosspointDistance = 80; // how many pixels between 2 crosspoints
float signalPixelRatio = 0.06*1024; // (see crosspoint.pde)

color textColor = color(60,60,60);
color backgroundColor = color(240,240,240);
color wireColor = color(180,180,180);
color signalColor = color(190,190,190);
color signalColorTouched = color(102,149,192);
float signalThreshold = 0.35;
int averageSignalCounter = 150;

DecimalFormat df = new DecimalFormat("#.###");

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance);
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
      crosspoints[i][j].note = note;
      note++; 
    } 
  }   
  sc = new SoundCipher(this);
  sc.instrument = 123;  
  myPort = new Serial( this, Serial.list()[1], 115200 );
  myPort.clear(); // do we need this?
  myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent() 
}

void draw() {
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
  // play the chord
  // playChord();
}

void drawTextInformation() {
  fill(textColor);
  text(textInformation, 10, height-10);
}

void serialEvent(Serial p) {
  String myString = p.readString();
  // println("START"+myString+"END");
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
      crosspoints[i][j].id = k;
      // println(k+": "+data[k]);
      k++;
    }
  }
  if (averageSignalCounter > 0) {
    averageSignalCounter--;
    textInformation = "calibrating: "+averageSignalCounter;
  }
}

void playNote(float pitch) {
  notes.add(pitch);
}

void playChord() {
  Object[] temp = notes.toArray();
  if (notes.size() > 0) {
    float[] pitches = new float[notes.size()];
    for (int i = 0; i < temp.length; i++) {
      pitches[i] = (Float) temp[i];
    }
    sc.playChord(pitches, 127.0, 0.8  );
    notes.clear();
  } 
}

void keyPressed() {
  if ((key == 'a') && (sc.instrument > 0)) {
    sc.instrument--;
    textInformation = "instrument: "+sc.instrument;
  }
  if ((key == 'd') && (sc.instrument < 127)) {
    sc.instrument++;
    textInformation = "instrument: "+sc.instrument; 
  }
  if ((key == 'w') && (sc.instrument > 0)) {
    signalThreshold = signalThreshold + 0.01;
    textInformation = "signal threshold: "+signalThreshold; 
  }
  if ((key == 's') && (sc.instrument < 127)) {
    signalThreshold = signalThreshold - 0.01;
    textInformation = "signal threshold: "+signalThreshold; 
  }
  // recalibrate
  if (key == 'r') {
    averageSignalCounter=150;
  }
}
