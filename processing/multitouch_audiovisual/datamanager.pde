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
    fakeData = "924,319,896,540,319,195,240,195,192,384,56,28,79,127,312,248,327,287,639,575,227,960,896,536,963,944,527,448,463,508,504,568,318,192,448,414,572,504,543,572,960,783,399,960,903,768,960,960,799,543,600,574,508,768,508,335,568,540,639,575,768,632,960,960,551,960,963,768,963,961,960,768,775,768,768,960,771,608,769,768,796,768,768,896,960,961,639,960,961,799,960,961,960,768,768,775,799,961,896,768,896,896,899,896,960,960,960,961,896,960,896,608,960,961,899,512,527,451,632,896,768,636,769,768,896,768,903,896,961,961,632,960,961,896,961,961,960,655,504,415,604,961,896,799,960,960,961,960,961,961,960,961,960,960,896,768,960,961,960,768,768,527,608,896,768,608,768,636,768,768,896,896,961,961,799,963,896,615,896,960,896,783,769,796,768,896,924,480,511,319,319,286,624,768,960,961,768,960,935,591,799,961,960,783,960,896,799,960,527,412,255,63,103,152,344,527,963,961,639,963,768,560,768,911,799,768,960,796,768,896,508,287,207,30,96,56,127,319,896,960,543,960,768,527,783,960,960,768,960,896,768,768,455,312,448,227,199,96,127,127,639,896,519,960,639,543,771,899,903,639,799,768,480,352,97,255,575,540,504,99,60,48,519,796,510,960,399,319,512,768,572,319,480,448,127,35,28,248,632,519,513,71,28,28,515,768,448,960,391,192,391,487,255,30,384,415,224,192,31,127,540,481,399,167,158,30,768,911,519,960,28,28,28,48,99,56,335,304,143,96,28,28,28,28,28,28,28,28,543,639,224,960,31,28,30,304,224,99,224,120,112,224,28,28,30,28,28,28,96,207,768,783,255,963,519,286,527,624,455,255,248,252,384,543,225,48,31,30,127,327,575,632,963,961,568,963,608,295,448,504,192,31,384,319,319,448,335,199,319,159,384,632,799,768,961,961,504,963,768,448,352,254,248,287,451,384,255,384,384,384,449,449,775,960,961,896,961,961,783,961,575,319,88,391,448,254,312,112,192,504,504,192,255,527,896,961,960,896,961,961,768,961,768,415,508,639,575,127,79,79,391,543,255,30,224,512,896,960,960,771,961,961,768,961,896,639,896,799,636,156,384,508,768,768,319,30,542,768,961,961,961,910,961,960,799,963,768,448,515,560,412,63,318,255,304,327,28,28,304,448,924,768,768,575,961,960,519,961,796,560,636,639,575,448,608,496,448,415,63,199,510,504,636,632,624,608,960,960,415,961,960,768,944,896,768,600,639,543,504,575,284,384,768,655,896,796,896,768,961,961,591,961,961,896,896,960,896,591,768,638,560,575,448,408,768,799,896,768,783,896,961,963,575,960,963,768,768,896,768,487,543,508,448,481,312,415,768,768,799,632,768,768,960,961,455,960,963,960,961,961,961,960,960,960,960,961,963,961,963,961,961,963,961,960,960,963,961,960,961,960,961,961,961,963,961,960,960,963,961,960,960,961,960,960,960,961,960,960,960,961,";
    this.parseFakeData();
  }

  int sb2ub(byte p) {
    return p < 0 ? 256+p : p;
  }

  Pt convertCoords(int x, int y) {
    Pt pt = new Pt();
  
    int a = x * H + y;
    int b = (63*a + 13) % (W*H);
  
    pt.x = b / H;
    pt.y = b - pt.x * H;
  
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

  void parseFakeData() {
    fakeData = trim(fakeData);
    float data[] = float(split(fakeData, ','));
    int k = 0;
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        crosspoints[i][j].setSignalStrength((int) data[k]);
        k++;
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

