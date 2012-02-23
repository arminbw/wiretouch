import processing.serial.*;

byte[] serBuffer = null;
Serial port;
boolean bNewFrame = true;
static final int verticalWires = 30;
static final int horizontalWires = 22;

void setup() {
   size(400, 400);
   initSerial();
   port.clear(); // // do we need this? (cargo cult programming)
   serBuffer = new byte[(1024*10/8)+1];
   // serBuffer = new byte[(horizontalWires * verticalWires * 10 )/8];
   port.buffer(serBuffer.length);
}

void initSerial() {
  try {
    port = new Serial( this, Serial.list()[0], 115200 );
  }
  catch (Exception e) {
    println(Serial.list());
    exit();
  }
}

void draw() {
  
}

void serialEvent(Serial p) {
  consumeSerialBuffer(p);
  bNewFrame = true;
}


int sb2ub(byte p) {
  return (p < 0 ? 256+p : p) & 0xff;
}

void consumeSerialBuffer(Serial p) {
    p.readBytes(serBuffer);
    int bs = 0, br = 0, cnt = 0;
    for (int i=0; i<serBuffer.length; i++) {
      br |= sb2ub(serBuffer[i]) << bs;
      bs += 8;
      while (bs >= 10) {          
        int sig = br & 0x3ff;
        println(sig+" "+binary(sig)+" "+binary(br)+" "+bs+" "+binary(sb2ub(serBuffer[i]))+"   "+i);
        br >>= 10;
        bs -= 10;          
        cnt++;
      }
    }
}
