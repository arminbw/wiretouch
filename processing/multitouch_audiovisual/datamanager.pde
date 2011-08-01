class DataManager {
  // TODO: use less global variables
  Serial myPort;
  String fakeData;
  
  DataManager(Serial port) {
    if (port == null) {
      // show the hand
      fakeData = "1.0177745286207143,1.0050976043162985,0.9585605789176244,0.8941506799202302,0.8650651940657635,0.884298759551044,0.8947846749322115,0.9124575753801073,1.0047182285210046,0.9647030372894828,0.9191608421866967,0.9789810968751189,0.9844003274440607,0.9851687094428786,0.922969100824321,1.009443651312452,0.9735378483122911,0.9686095543894632,0.9110524386236379,0.9686048452446128,0.9648341383159422,1.0205412768826585,1.0251208267825405,1.0059144025560045,1.0005026101186534,0.9989722507809714,1.0100921573115733,0.9954310973688439,0.9714061310248304,0.9931270484035027,0.981325879298619,0.9733844580061021,0.9705995148161609,1.038783804303891,1.0070619778631078,0.9990314965411643,0.998990368436823,1.037558028388462,0.987655061439728,0.9916469428820053,0.9965882219198338,1.0355259693010035,0.9688017547203368,0.9164579685796975,0.9587527627353662,0.9847549427450246,1.0155474146403385,1.0062227960620067,1.010160974326104,0.9652638623908719,0.9464381885579612,0.8550030589942593,0.8983240646781054,0.8864344044373021,0.9464997604981731,0.938721351963264,0.9783502929513068,0.9957267373761036,0.997948717948718,0.9897579277181204,0.9317413594432743,0.9304203613905092,0.8062432261629713,0.7105284640941446,0.9342819234610742,0.9638116118994091,0.9152584698941336,0.97672931435144,0.9148524364520079,0.7376944729254136,0.8322220919472263,0.8463860727279285,0.8794654232267904,0.9063097984314314,0.6431073757268135,0.871961790318799,0.8247738673542567,0.8805820464540486,0.9608660406468185,0.8022292066673297,0.5300757916456246,0.6190663480354044,0.6654834622215863,0.9366520487964672,0.8598587142085359,0.7271301044049499,0.6947959451311567,0.7626218089713576,0.8164685796723856,0.9377255330989938,0.8636137188863399,0.7406220490816099,0.6931395407512454,0.5991585079759845,0.5159905848912311,0.4787435183868114,0.4076142279455691,0.4378640028859967,0.49113620994349516,0.68275442897254,0.8753609800108506,0.6796989996730917,0.5312402484724268,0.6467026265274156,0.5960329291036097,0.5530313553653237,0.5454658572089327,0.5243887354248253,0.5188945633103584,0.3713647519160015,0.6372638686791348,0.8565957383964488,0.7727149707040717,0.6235909978382677,0.6353474205431039,0.5842311322110438,0.4956461889703299,0.49350165791312806,0.45401321182405396,0.4144564734872285,0.30212162319016805,0.6277545148089555,0.8545095786694222,0.7083457489151632,0.56,0.5529111972995743,0.5454162803771054,0.5184742618732632,0.4652017458863144,0.4459193713737242,0.4813887908085348,0.5494450576453916,0.7450459859400852,0.9045522996977688,0.8590763029756882,0.7035897435897436,0.6311733265536369,0.49008642113810613,0.745469122002028,0.726777378729984,0.7271989711449739,0.7318518693901848,0.8269207402724278,0.9161600927238321,0.9738914783139057,0.9382213813590334,0.7425641025641025,0.5832482882742936,0.6146323237206585,0.8524973295935283,0.9899617459892556,0.9168885373838322,0.8825615326897992,0.8493695892877027,0.9033747186411315,0.9683331563735934,0.9449763188978451,0.7794871794901578,0.7680091231493276,0.7721867888525363,0.8537904914994408,0.9579123103578531,0.9365536642588177,0.9183906203852398,0.8787914090272666,0.9418902777059078,0.9801515985845604,0.930953537194395,0.8784758628270157,0.8262044526966031,0.8474228662949302,0.9940062619561573,1.021780267249775,0.9021271542110293,0.9260769571675745,0.9529468115679817,";
      averageSignalCounter=0;
      this.parseFakeData();
      textInformation = helpText;
    }
    else {
      println(Serial.list());
      this.myPort = port;
      this.myPort.clear(); // // do we need this? (cargo cult programming)
      if (bReadBinary) {
        // TODO: better encapsulation
        serBuffer = new byte[2 * horizontalWires * verticalWires];
        this.myPort.buffer(2 * horizontalWires * verticalWires);
      }
      else {
        this.myPort.bufferUntil(32); // buffer everything until ASCII whitespace char triggers serialEvent()
      }
      textInformation = "starting to calibrate";
    }
  }
  
  int sb2ub(byte p) {
    return p < 0 ? 256+p : p;
  }
  
  void consumeSerialBuffer(Serial p) {
    p.readBytes(serBuffer);
    if (!bDebug) {
      for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
           int sigPos = i*horizontalWires + j;
           int sig = ((sb2ub(serBuffer[2*sigPos])) << 8) | (sb2ub(serBuffer[2*sigPos+1]));
           if (averageSignalCounter > 0) {
             // calculate the average signal strength for every crosspoint
             crosspoints[i][j].accumulateAvgSig(sig);
           } else {
              crosspoints[i][j].setSignalStrength(sig);
           }
        }
      }
      calibrationFeedback();
    }
  }
  
  // TODO: this code needs some serious refactoring...
  void calibrationFeedback() {
    if (averageSignalCounter > 0) {
      averageSignalCounter--;
      if (averageSignalCounter != 0) {
        textInformation = "calibrating: "+averageSignalCounter;
      }
      else {
        textInformation = helpText;
      }
    }    
  }
  
  void parseData(String myString) {
    if (!bDebug) {
      myString = trim(myString);
      int data[] = int(split(myString,','));
      int k = 0;
      for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
          if (averageSignalCounter > 0) {
            // calculate the average signal strength for every crosspoint
            // TODO: optimize
            crosspoints[i][j].accumulateAvgSig(data[k]);  
          } else {
            crosspoints[i][j].setSignalStrength(data[k]);
          }
          k++;
        }
      }
      calibrationFeedback();
    }
  }
  
  void parseFakeData() {
    fakeData = trim(fakeData);
    float data[] = float(split(fakeData,','));
    int k = 0;
    for (int i = 0; i < verticalWires; i++) {
      for(int j = 0; j < horizontalWires; j++) {
        crosspoints[i][j].signalStrength = data[k];
        k++;
      }
    } 
  }
  
  void printData() {
     println("measured signals:");
     String myString = "";
     for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
          myString = myString+(int) crosspoints[i][j].measuredSignal+",";
        }
     }
     println(myString);
     println("calculated signal strength:");
     myString = "";
     for (int i = 0; i < verticalWires; i++) {
        for(int j = 0; j < horizontalWires; j++) {
          myString = myString+crosspoints[i][j].signalStrength+",";
        }
     }
     println(myString);
  }
}
