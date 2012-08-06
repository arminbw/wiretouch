class Crosspoint {
  int x, y;
  double measuredSignal;          // the actual received values
  double measuredSignalAverage;   // the calculates signal average without touch
  double signalStrength;          // the calculated signal strength (ca. 0.0-1.0)
  double signalMin = 1000000000, signalMax = -1000;
  double signalPMin = 1000000000, signalPMax = -1000;
  MyKalman myKalman;

  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
    this.measuredSignal = 1;
    this.measuredSignalAverage = 0;
    this.signalStrength = 1;
    this.myKalman = new MyKalman();
  }

  void draw(boolean bDrawText) {
    noFill();
    stroke(signalColor);
    if (configurator.bShowCalibrated) {
      ellipse(x, y, (float)signalStrength*signalPixelRatio, (float)signalStrength*signalPixelRatio);
    }
    else {
      ellipse(x, y, (float)(measuredSignal/1024)*signalPixelRatio, (float)(measuredSignal/1024)*signalPixelRatio);
    }
    fill(wireColor);
    if (bDrawText) {
      // text((df.format(measuredSignal)), x+4, y-4);
      // text((df.format(signalStrength)+"\n"+this.signalMax+"\n"+df.format(this.measuredSignalAverage)+"\n"+this.signalMin), x+4, y-4);
    }
  }

  void setSignalStrength(int msr) {
    this.measuredSignal = msr;
    // this.signalStrength = (double) msr / (this.measuredSignalAverage+1);
    // this.signalStrength = 1.0 - ((this.measuredSignalAverage - msr) / (double) this.measuredSignalAverage);
    this.signalStrength = (float) this.myKalman.update( (double) msr / (this.measuredSignalAverage+1));
    if (msr > this.signalMax) this.signalMax = msr;
    if (msr < this.signalMin) this.signalMin = msr;
    if (this.signalStrength > this.signalPMax) this.signalPMax = this.signalStrength;
    if (this.signalStrength < this.signalPMin) this.signalPMin = this.signalStrength;
  }

  void accumulateAvgSig(int val) {
    this.measuredSignalAverage = this.measuredSignalAverage/2 + (val/2);
  }
}


// TODO: 1/(1+exp(-(x-0.5)*10.0)) interval 0,1

public class MyKalman {
/*
  private double Q = 0.00001;
  private double R = 0.01;
  private double P = 1, X = 0, K;
*/
  private double Q = 0.005;
  private double R = 0.001;
  private double P = 1, X = 0, K;

  private void measurementUpdate(){
    K = (P + Q) / (P + Q + R);
    P = R * (P + Q) / (R + P + Q);
  }

  public double update(double measurement){
    measurementUpdate();
    double result = X + (measurement - X) * K;
    X = result;
    return result;
  }
}

