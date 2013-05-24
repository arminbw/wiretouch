public class Pt {
  int x;
  int y;
}

class DataManager {
  Serial port;
  String fakeData;
  byte[] serBuffer = null;
  String settingsJson;
  boolean receivingSettings;

  DataManager() {
    this.serBuffer = new byte[(horizontalWires * verticalWires * 10)/8];
  }

  void calibrate(Serial port) {
    this.port = port;
    try {
      this.port.clear(); // // do we need this? (cargo cult programming)
      //this.port.buffer(serBuffer.length);
      delay(2000); // still needed?
      textInformation = "firmware calibration";
      this.port.write("c\n");  // calibrate
      this.port.write("i\n");  // get the port values
      this.receivingSettings = true;
      this.settingsJson = new String();    
      this.port.write("s\n");  // start

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
  
  void sendDotMatrixCorrectionData(int x, int y, int value) {
    textInformation = "e"+x+","+y+","+value+"\n";
    this.port.write("e"+x+","+y+","+value+"\n");
  }
  
  void sendValue(String command, int value) {
    this.port.write(command+""+value+"\n");
  }
  
  // save the values for the digital potentiometer
  void savePotValues() {
    String[] lines = new String[verticalWires*horizontalWires];
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        lines[(i*horizontalWires)+j] = ""+crosspoints[i][j].guiSlider.value;
      }
    }
    saveStrings("data/potvalues.txt", lines);
  }
  
  void loadPotValues() {
    String[] lines = new String[verticalWires*horizontalWires];
    lines = loadStrings("data/potvalues.txt");
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
        lines[(i*horizontalWires)+j] = ""+crosspoints[i][j].guiSlider.value;
      }
    }  
  }
  
  void appendPotValues(String aString) {
    this.settingsJson += aString;
  }
  
  // getting the autocalibration values from the mt motherboard
  void receivePotValues() {    
    JSON jsonObject = JSON.parse(this.settingsJson);
    guiExtraSliders.halfwave.setValue(int(jsonObject.getString("halfwave_amp")));
    guiExtraSliders.outputamp.setValue(int(jsonObject.getString("output_amp")));
    guiExtraSliders.delaySlider.setValue(int(jsonObject.getString("delay")));
    guiExtraSliders.waveFrequency.setValue(int(jsonObject.getString("freq")));
    
    firmwareVersion = jsonObject.getString("version");
    
    this.receivingSettings = false;
    this.settingsJson = null;
    
    for (int i = 0; i < verticalWires; i++) {
      for (int j = 0; j < horizontalWires; j++) {
         String crosspt = "tune_" + i + "_" + j;
         crosspoints[i][j].guiSlider.setValue(int(jsonObject.getString(crosspt)));
      }
    }
    this.savePotValues();
  } 
}

