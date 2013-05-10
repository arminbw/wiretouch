public class Pt {
  int x;
  int y;
}

class DataManager {
  Serial port;
  String fakeData;
  byte[] serBuffer = null;

  DataManager() {
    this.serBuffer = new byte[(horizontalWires * verticalWires * 10)/8];
  }

  void calibrate(Serial port) {
    this.port = port;
    try {
      this.port.clear(); // // do we need this? (cargo cult programming)
      //this.port.buffer(serBuffer.length);
      delay(2000); // still needed?
      this.port.write('s');
      textInformation = "starting to calibrate";
    } 
    catch (Exception e) {
      textInformation = "error with Serial connection: "+e;
      println(Serial.list());
    }
  }

  void initFakeData() {
    // show the hand
    String lines[] = loadStrings("fakehand.txt");    
    float m[] = float(split(lines[0], ','));
    float n[] = float(split(lines[1], ','));    
    int k = 0;
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        crosspoints[i][j].measuredSignalAverage = n[k];      
        crosspoints[i][j].setSignalStrength((int) m[k]);
        k++;
      }
    }    
  }

  int sb2ub(byte p) {
    return p < 0 ? 256+p : p;
  }

  Pt convertCoords(int x, int y) {
    Pt pt = new Pt();
  
    // int a = x * horizontalWires + y;
    // int b = (59*a + 13) % (verticalWires*horizontalWires);
  
    // pt.x = b / horizontalWires;
    // pt.y = b - pt.x * horizontalWires;
    pt.x = x;
    pt.y = y;
    return pt;
  }

  void consumeSerialBuffer(byte[] b) {
    byte[] buf = null != b ? b : serBuffer;
    
    int bs = 0, br = 0, cnt = 0;
    for (int i=0; i<buf.length; i++) {
      br |= sb2ub(buf[i]) << bs;
      bs += 8;
      while (bs >= 10) {          
        int sig = br & 0x3ff;
        br >>= 10;
        bs -= 10;          
        int px = cnt / horizontalWires, py = cnt % horizontalWires;
        Pt pt = convertCoords(px, py);
        if (averageSignalCounter > 0) {
          // calculate the average signal strength for every crosspoint
          crosspoints[pt.x][pt.y].accumulateAvgSig(sig);
        }
        else {
          crosspoints[pt.x][pt.y].setSignalStrength(sig);
        }
        cnt++;
      }
    }
    calibrationFeedback();
  }

  // TODO: this code needs some serious refactoring...
  void calibrationFeedback() {
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

  void printData() {
    println("measured signals:");
    String myString = "";
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        myString = myString+(int) crosspoints[i][j].measuredSignal+",";
      }
    }
    println(myString);
    println("measured signal average:");
    myString = "";
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        myString = myString+crosspoints[i][j].measuredSignalAverage+",";
      }
    }
    println(myString);
  }
  
  void writeColumnCorrectionData(int column, int value) {
    this.port.write("e"+column+","+value+"\n");
  }
}

