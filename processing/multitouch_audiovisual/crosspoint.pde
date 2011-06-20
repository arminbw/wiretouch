class Crosspoint {
  int x, y;
  int measuredSignal;
  int signalStrength;
  int signalStrengthAverage;
  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.signalStrengthAverage = 1;
    this.measuredSignal = 1;
  }
  void draw() {
    if (signalStrength>signalThreshold) {
      fill(signalColor);
    }
    else {
      fill(signalColorTouched); 
    }
    ellipse(x, y, measuredSignal*signalPixelRatio, measuredSignal*signalPixelRatio);
    fill(textColor);
    text((measuredSignal+"\n"+signalStrength+"\n"+signalStrengthAverage), x+2, y-2);
  }
  
  void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    this.signalStrength = (1024/this.signalStrengthAverage) * msr;
  }
  
  void accumulateAvgSig(int val) {
    this.signalStrengthAverage = (this.signalStrengthAverage/2) + (val/2);
  }
}
