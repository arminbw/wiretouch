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
    this.signalStrengthAverage = 0;
    this.measuredSignal = 1;
    this.playingSound = false;
    this.note = 0;
  }
  
  void draw() {
    if (signalStrength>signalThreshold) {
      fill(signalColor);
      this.playingSound = false;
    }
    else {
      fill(signalColorTouched);
      if (this.playingSound == false) {
        this.playingSound = true;
        playNote(this.note);
      }
    }
    ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    fill(textColor);
    // text((measuredSignal+"\n"+signalStrength+"\n"+signalStrengthAverage), x+2, y-2);
    // text((this.signalMax + " (" + df.format(this.signalPMax) +")\n"+df.format(signalStrength)+"\n"+this.signalMin + " (" + df.format(this.signalPMin) + ")"), x-crosspointDistance/2.0, y-2);
    text((this.signalMax+"\n"+df.format(this.signalStrengthAverage)+"\n"+this.signalMin+"\n"+df.format(signalStrength)), x+4, y-4);
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
