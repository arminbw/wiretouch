
import processing.serial.*;
import arb.soundcipher.*;

SoundCipher sc = new SoundCipher();

Serial myPort;
int lf = 10;
String myString;
color touchColor;
int touch, val, pos;

float myTime;

// import ddf.minim.*;
// import ddf.minim.signals.*;

// Minim minim;
// AudioOutput out;
// SineWave sine;

int[] touchSensors = new int[2];
int size1, size2;
color color1, color2, color3;

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
   color1 = color(0, 0, 0);
   color2 = color(102, 102, 102);
   color3 = color(255, 255, 255);
  // minim = new Minim(this);
  // get a line out from Minim, default bufferSize is 1024, default sample rate is 44100, bit depth is 16
  // out = minim.getLineOut(Minim.STEREO);
  // create a sine wave Oscillator, set to 440 Hz, at 0.5 amplitude, sample rate from line out
  // sine = new SineWave(440, 0.5, out.sampleRate());
  // set the portamento speed on the oscillator to 200 milliseconds
  // sine.portamento(200);
  // add the oscillator to the line out
  // out.addSignal(sine);
}

void draw ()
{
   background(200, 200, 200);
   smooth();
   while (myPort.available() > 0)
   {
      myString = myPort.readStringUntil(lf);  // assumes you are sending them with println() ... (change to byte?)
      if (myString != null) {
         int myInput = (int) int(myString.trim());  
         if (myInput/20000 == 0) {
             touchSensors[0] = myInput%10000;
             // size1 = (int) (size1*0.9 + 0.1*round((1024 - (val))));
             size1 = (int) (size1 * .5 + .5 * round(((touchSensors[0]*0.25))));
             // size1 = (int) round(1024-touchSensors[0]);
         }
         else {
             touchSensors[1] = myInput%10000;
             size2 = (int) (size2 * .5 + .5 * round(((touchSensors[1])*0.25)));
             // size2 = (int) round(1024-touchSensors[1]);
         }     
         // println(myInput + " -> " + touchSensors[0] + " " + size1+" : "+touchSensors[1]+ " " + size2);
      }
      // sine.setFreq(1464 - touch);
   }
   fill(color2);
   ellipse(180,150,size2,size2);
   ellipse(420,150,size1,size1);
   int x = 300+((size2-size1)*4);
   fill(color3);
   ellipse(x, 150, 100, 100);
   float delay = millis() - myTime;
   if (delay > 500) {
      delay = 0;
      myTime = millis();
      // tone should be between 0 and 127
      float tone = touchSensors[1] - touchSensors[0];
      println(" -> " + touchSensors[1] + " : "+touchSensors[0]);
      println(tone);
      tone = abs(((tone+320)/650)*10)+60;
      println(tone);
      sc.playNote(tone, 100, 200);
   }
}


