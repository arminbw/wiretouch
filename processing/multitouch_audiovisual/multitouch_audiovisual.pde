import arb.soundcipher.*; // get it at http://soundcipher.org
import processing.serial.*;
import java.text.DecimalFormat;

//SoundCipher sc = new SoundCipher(this);
Serial myPort;
Crosspoint[][] crosspoints;
PFont myFont;
SoundCipher sc = new SoundCipher(this);
ArrayList notes;

// configuration
int verticalWires = 16;
int horizontalWires = 1;
int crosspointDistance = 80; // how many pixels between 2 crosspoints
float signalPixelRatio = 0.06*1024; // (see crosspoint.pde)

color textColor = color(0,0,0);
color backgroundColor = color(240,240,240);
color wireColor = color(200,200,200);
color signalColor = color(220,220,220);
color signalColorTouched = color(180,180,180);
float signalThreshold = 0.65;
int averageSignalCounter = 150;

DecimalFormat df = new DecimalFormat("#.###");

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance);
  smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  myPort = new Serial( this, Serial.list()[0], 115200 );
  myPort.clear(); // do we need this?
  myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent() 
  
  crosspoints = new Crosspoint[horizontalWires][verticalWires];
  float note = 0.0;
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(j+1),crosspointDistance*(i+1));
      // c major chord
      /* int turn = ((i+j) % 3);
      if (turn == 0) note = note + 4;
      if (turn == 1) note = note + 3;
      if (turn == 2) note = note + 5; */
      note++; 
      crosspoints[i][j].note = note;
    } 
  } 
  notes = new ArrayList();
  sc.instrument = sc.DRUM;
}

void draw() {
  background(backgroundColor);
  if (averageSignalCounter == 0) {
    // draw the crosspoint signals
    noStroke();
    for (int i = 0; i < horizontalWires; i++) {
      for(int j = 0; j < verticalWires; j++) {
        crosspoints[i][j].draw();
      }
    }
    // draw the grid
    stroke(wireColor);
    for (int i = 1; i <= horizontalWires; i++) {
      line(crosspointDistance, crosspointDistance*i, crosspointDistance*verticalWires, crosspointDistance*i);
      for(int j = 1; j <= verticalWires; j++) {
        line(crosspointDistance*j, crosspointDistance, crosspointDistance*j, crosspointDistance*horizontalWires);
      } 
    }
  }
  // play the chord
  playChord();
}

void serialEvent(Serial p) {
  String myString = trim(p.readString());
  int data[] = int(split(myString,','));
  int k = 0;
  for (int i = 0; i < verticalWires; i++) {
    for(int j = 0; j < horizontalWires; j++) {
      // calculate the average signal strength for every crosspoint
      if (averageSignalCounter > 0) {
        crosspoints[j][i].accumulateAvgSig(data[k]);
      } else {
        crosspoints[j][i].setSignalStrength(data[k]);
      }
      k++;
    }
  }
  if (averageSignalCounter > 0) {
    averageSignalCounter--;
    println(averageSignalCounter);
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
    sc.playChord(pitches, 127.0, 2.0);
    notes.clear();
  } 
}
