import arb.soundcipher.*; // get it at http://soundcipher.org
import processing.serial.*;

SoundCipher sc = new SoundCipher(this);
Serial myPort;
Crosspoint[][] crosspoints;
PFont myFont;

// configuration
int verticalWires = 2;
int horizontalWires = 1;
int crosspointDistance = 200; // how many pixels between 2 crosspoints
float signalPixelRatio = 0.2; // (see crosspoint.pde)
color backgroundColor = color(240,240,240);
color wireColor = color(200,200,200);
color signalColor = color(220,220,220);
color textColor = color(0,0,0);
int instrument = 23; // the famous General Midi tango accordion

void setup() {
  size((verticalWires+1)*crosspointDistance, (horizontalWires+1)*crosspointDistance);
  smooth();
  myFont = loadFont("Consolas-12.vlw");
  textFont(myFont, 12);
  // sc.getMidiDeviceInfo();
  // sc.setMidiDeviceOutput(0);
  // sc.instrument(instrument);
  // println(Serial.list());
  myPort = new Serial( this, Serial.list()[0], 115200 );
  myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent() 
  
  crosspoints = new Crosspoint[horizontalWires][verticalWires];
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j] = new Crosspoint(crosspointDistance*(j+1),crosspointDistance*(i+1));
    } 
  }
}

void draw() {
  background(backgroundColor);
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

void serialEvent(Serial p) {
  String myString = trim(p.readString());
  int data[] = int(split(myString,','));
  int k = 0;
  for (int i = 0; i < horizontalWires; i++) {
    for(int j = 0; j < verticalWires; j++) {
      crosspoints[i][j].signalStrength = data[k];
      k++;
    }
  }
}
/*
void mousePressed() {
  sc.sendMidi(sc.PROGRAM_CHANGE, 0, instrument, 0);
  sc.sendMidi(sc.NOTE_ON, 0, 60, 100);
}

void mouseDragged() {
  sc.sendMidi(sc.PITCH_BEND, 0, 0, 100 - mouseY + 14);
  sc.sendMidi(sc.CONTROL_CHANGE, 0, 7, mouseX);
}

void mouseReleased() {
  sc.sendMidi(sc.NOTE_OFF, 0, 60, 0);
}
*/
