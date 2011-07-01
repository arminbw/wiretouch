class Crosspoint {
  int x, y;
  double measuredSignal;
  double signalStrength;
  double signalStrengthAverage;
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;
  boolean playingSound;
  float note;
  
  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.signalStrength = 1;
    this.signalStrengthAverage = 0;
    this.playingSound = false;
    this.note = 0;
  }
  
  void draw() {
    fill(signalColor);
    if (signalStrength>signalThreshold) {
      this.playingSound = false;
    }
    else {
      if (this.playingSound == false) {
        this.playingSound = true;
        playNote(this.note);
      }
    }
    ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    fill(textColor);
    text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.signalStrengthAverage)+"\n"+this.signalMin), x+4, y-4);
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

