import processing.serial.*;

byte[] serBuffer = null;
Serial port;
static final int verticalWires = 30;
static final int horizontalWires = 22;

void setup() {
  size(4, 4);
  initSerial();
  serBuffer = new byte[(1024*10/8)+1];
  // serBuffer = new byte[(horizontalWires * verticalWires * 10 )/8];
  port.buffer(serBuffer.length);
  port.clear(); // // do we need this? (cargo cult programming)
}

void initSerial() {
  try {
    port = new Serial( this, Serial.list()[0], 9600 );
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
}


int sb2ub(byte p) {
  return (p < 0 ? 256+p : p) & 0xff;
}

void consumeSerialBuffer(Serial p) {
  p.readBytes(serBuffer);
  println("consume!");
  println("");
  int bs = 0, br = 0;
  // for (int i=0; i<serBuffer.length; i++) {
  for (int i=0; i<100; i++) {
    println("");
    println("i: "+i);
    println("binary(br):"+binary(br));
    println("bs:"+bs);
    println("sb2ub(serBuffer["+i+"])):"+binary(sb2ub(serBuffer[i])));
    br |= sb2ub(serBuffer[i]) << bs;
    println("sb2ub(serBuffer["+i+"]) << bs:"+binary(sb2ub(serBuffer[i]) << bs));
    println("br |= sb2ub(serBuffer["+i+"]) << bs:"+binary(br));
    bs += 8;
    while (bs >= 10) {          
      int sig = br & 0x3ff;
      println("sig:"+sig+"  binary(sig):"+binary(sig)+"  binary(br):"+binary(br)+"  bs:"+bs);
      br >>= 10;
      bs -= 10;
    }
    if (i==60) {
      println("");
      println("==========================");
      println("ATTENTION");
      println("==========================");
      println("");
    }
  }
  println("consumed");
}

void stop()
{
  port.clear();
  port.stop();
}

void keyPressed() {
  if (key != 0) {
      this.port.write('s');
      println("starting");
  }
}

