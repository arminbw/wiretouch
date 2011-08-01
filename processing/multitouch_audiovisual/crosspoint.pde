class Crosspoint {
  int x, y;
  double measuredSignal;          // the actual received values
  double measuredSignalAverage;
  double signalStrength;          // the calculated signal strength (ca. 0.0-1.0)
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;
  
  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.measuredSignalAverage = 0;
    this.signalStrength = 1;
  }
  
  void draw(boolean bDrawText) {
    fill(signalColor);
    ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    fill(textColor);
    if (bDrawText) {
      text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.measuredSignalAverage)+"\n"+this.signalMin), x+4, y-4);
    }
  }
  
  void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    // this.signalStrength = (double)msr / 1024; // without calibration
    this.signalStrength = (double) msr / (this.measuredSignalAverage+1);
    
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
    this.measuredSignalAverage = this.measuredSignalAverage/2 + (val/2);
  }
}
