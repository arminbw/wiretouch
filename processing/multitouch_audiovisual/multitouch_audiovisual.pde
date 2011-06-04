import processing.serial.*;
import arb.soundcipher.*;

SoundCipher sc = new SoundCipher();

Serial myPort;
int lf = 10;
String myString;
int size1, size2;
float myTime;
int[] touchSensors = new int[2];
color color1, color2;

void setup ()
{
  myTime = millis();
  SoundCipher.getMidiDeviceInfo();
  sc.instrument(78);
  sc.setMidiDeviceOutput(0);
  size(600, 300);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[0], 9600 );
  noStroke();
  color1 = color(102, 102, 102);
  color2 = color(255, 255, 255);
}

void draw ()
{
  background(200);
  smooth();
  while (myPort.available() > 0)
  {
    myString = myPort.readStringUntil(lf);  // assumes you are sending them with println() ... (change to byte?)
    if (myString != null) {
      int myInput = (int) int(myString.trim());  
      if (myInput/20000 == 0) {
        touchSensors[0] = myInput%10000;
        size1 = (int) (size1 * .5 + .5 * round(((touchSensors[0]*0.25))));
      }
      else {
        touchSensors[1] = myInput%10000;
        size2 = (int) (size2 * .5 + .5 * round(((touchSensors[1])*0.25)));
      }     
      // println(myInput + " -> " + touchSensors[0] + " " + size1+" : "+touchSensors[1]+ " " + size2);
    }
  }
  fill(color1);
  ellipse(180,150,size2,size2);
  ellipse(420,150,size1,size1);
  int x = 300+((size2-size1)*4);
  fill(color2);
  ellipse(x, 150, 100, 100);
  float delay = millis() - myTime;
  if (delay > 500) {
    delay = 0;
    myTime = millis();
    // tone should be between 0 and 127
    float tone = touchSensors[1] - touchSensors[0];
    tone = abs(((tone+320)/650)*10)+60;
    println("sensors: " + touchSensors[1] + " : "+touchSensors[0] + "   tone: " + tone);
    sc.playNote(tone, 100, 200);
  }
}

