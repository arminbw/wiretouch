class Crosspoint {
  int x, y;
  double measuredSignal;
  double signalStrength;
  double signalStrengthAverage;
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;
  
  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.signalStrength = 1;
    this.signalStrengthAverage = 0;
  }
  
  void draw() {
    fill(signalColor);
    ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    fill(textColor);
    if (bDebug) {
      text(this.measuredSignal+"\n"+df.format(signalStrength), x+4, y-4);
    }
    else {
      if (bShowText) {
        text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.signalStrengthAverage)+"\n"+this.signalMin), x+4, y-4);
      }
    }
  }
  
  void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    this.signalStrength = (double)msr / (this.signalStrengthAverage+1);
    
    if (msr > this.signalMax)
      this.signalMax = msr;
    if (msr < this.signalMin)
      this.signalMin = msr;
      
    if (this.signalStrength > this.signalPMax)
      this.signalPMax = this.signalStrength;
    if (this.signalStrength < this.signalPMin)
      this.signalPMin = this.signalStrength;
  }
  
  void accumulateAvgSig(int val) {
    this.signalStrengthAverage = this.signalStrengthAverage/2 + (val/2);
  }
}

