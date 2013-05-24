class Crosspoint {
  int x, y;
  double measuredSignal;          // the actual received values
  double measuredSignalAverage;   // the calculates signal average without touch
  double signalStrength;          // the calculated signal strength (ca. 0.0-1.0)
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;
  GUISlider guiSlider;

  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.measuredSignalAverage = 0;
    this.signalStrength = 1;
    int sliderPosX = 0;
    if (this.x > sketchWidth - 240) {
      sliderPosX = sketchWidth - 240;
    }
    else {
      sliderPosX = this.x - 10;
    } 
    this.guiSlider = new GUISlider(sliderPosX, y+40, 200, 0, 15); // every crosspoint gets its own slider for the digital pot
    this.guiSlider.setValue(8);
    this.guiSlider.drawsBorder = true;
  }

  void draw(boolean bDrawText) {
    noFill();
    stroke(signalColor);
    /*if (configurator.bShowCalibrated) {
      ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    }
    else {
      ellipse(x, y, (float)(measuredSignal/1024)*signalPixelRatio, (float)(measuredSignal/1024)*signalPixelRatio);
    }*/
    fill(wireColor);
    if (bDrawText) {
      drawText();
    }
  }
  
  void drawText() {
      text((df.format(measuredSignal)), x+4, y-4);
      text((df.format(signalStrength)), x+4, y+12);      
      // text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.measuredSignalAverage)+"\n"+this.signalMin), x+4, y-4);
  }

  void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    this.signalStrength = (double) msr / (this.measuredSignalAverage + 1); 
    if (msr > this.signalMax) this.signalMax = msr;
    if (msr < this.signalMin) this.signalMin = msr;
    if (this.signalStrength > this.signalPMax) this.signalPMax = this.signalStrength;
    if (this.signalStrength < this.signalPMin) this.signalPMin = this.signalStrength;
    if (this.measuredSignal > this.measuredSignalAverage*signalCutOff) {
      this.signalStrength = 1.0;
    }
  }

  void accumulateAvgSig(int val) {
    this.measuredSignalAverage = this.measuredSignalAverage/2 + (val/2);
  }
  
  boolean isInside(int x, int y) {
    return ((x > this.x -13) && (x < this.x + 123) && (y > this.y-13) && (y < this.y + 13));
  }
}

