class Interpolator
{
  static final int histogramBins = 256;

  boolean bContrastStretch; 

  int horizontalSamples, verticalSamples;
  int pixelWidth, pixelHeight;   // the actual pixel count of the interpolated image
  int horizontalMultiplier, verticalMultiplier;   // used to calculate pixelWidth and pixelHeight
  int resizedWidth, resizedHeight;  // the resized, blown up image
  float _fx, _fy;
  double[] interpolPixels;
  int[] _hist;
  int _histMax;
  String name;
  
  Interpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight, String name)
  {
    // TODO: simplify (use picture)
    this.horizontalSamples = horizontalSamples;
    this.verticalSamples = verticalSamples;
    this.horizontalMultiplier = horizontalMultiplier;
    this.verticalMultiplier = verticalMultiplier;
    this.pixelWidth = (horizontalSamples - 1) * horizontalMultiplier;
    this.pixelHeight = (verticalSamples - 1) * verticalMultiplier;
    this.resizedWidth = imageWidth;
    this.resizedHeight = imageHeight;

    this._fx = 1.0 / (2.0 * (float)horizontalMultiplier);
    this._fy = 1.0 / (2.0 * (float)verticalMultiplier);

    this.interpolPixels = new double[pixelWidth * pixelHeight];
    this._hist = new int[histogramBins];

    this.bContrastStretch = false;
    this.name = name;
  }

  // includes value repetition at the borders
  double sensorVal(Crosspoint[][] cp, int x, int y)
  {
    x = constrain(x, 0, horizontalSamples-1);
    y = constrain(y, 0, verticalSamples-1);

    return cp[x][y].signalStrength;
  }
  
  void updatePicture() {
    picture.loadPixels();
    for (int i=0; i<interpolPixels.length; i++) {
      picture.pixels[i] = color((float)interpolPixels[i]*255.0);
    }
    picture.loadTexture();
  }

  void drawPicture(int x, int y) {
    updatePicture();
    picture.render(x, y, resizedWidth, resizedHeight);
  }

  void drawHistogramFromPoint(int x1, int y1, int maxY)
  {
    float step = (float)maxY / (float)log(this._histMax);

    strokeWeight(1.0);
    for (int i=0; i<histogramBins; i++) {
      int h = (int)(constrain(log(this._hist[i]) * step, 0.0, maxY));

      stroke(color(i));
      line(x1+i, y1, x1+i, y1-maxY);  

      stroke(histogramColor);
      line(x1+i, y1, x1+i, y1-h);
    }
  }

  void interpolate(Crosspoint[][] cp)
  {
    beginInterpolation(cp);

    Arrays.fill(this._hist, 0);
    this._histMax = 0;
    for (int i=0; i<horizontalSamples-1; i++)
      for (int j=0; j<verticalSamples-1; j++) {
        beginInterpolate4(cp, i, j);

        for (int k=0; k<horizontalMultiplier; k++)
          for (int l=0; l<verticalMultiplier; l++) {
            double val =  interpolate4(cp, i, j, k, l, _fx * (1.0 + 2*k), _fy * (1.0 + 2*l));
            val = constrain((float)val, 0.0, 1.0);

            int p = (int)(val/(1.0/((double)histogramBins-1)));
            _hist[p]++;
            if (_hist[p] > this._histMax)
              this._histMax = _hist[p];

            this.interpolPixels[((j * verticalMultiplier + l) * pixelWidth) + i * horizontalMultiplier + k] = val;
          }

        finishInterpolate4(cp, i, j);
      }

    finishInterpolation(cp);

    if (this.bContrastStretch) {
      float vmin = 0.0, vmax = 1.0;

      int ss, nHist = (int) (this.pixelWidth * this.pixelHeight * contrastLeft);
      for (ss = 0; nHist > 0 && ss<histogramBins; nHist -= this._hist[ss++]);
      vmin = (1.0/histogramBins) * ss;

      nHist = (int)(this.pixelWidth * this.pixelHeight * contrastRight);
      for (ss = histogramBins-1; nHist > 0 && ss>=0; nHist -= this._hist[ss--]);
      vmax = (1.0/histogramBins) * ss;

      for (int i=0; i<pixelWidth; i++)
        for (int j=0; j<pixelHeight; j++) {
          double p = interpolPixels[j * pixelWidth + i];
          interpolPixels[j * pixelWidth + i] = (p - vmin) / (vmax - vmin);
        }
    }
  }

  void beginInterpolation(Crosspoint[][] cp)
  {
    // override in subclass if necessary
  }

  void finishInterpolation(Crosspoint[][] cp)
  {
    // override in subclass if necessary
  }

  void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    // override in subclass if necessary
  }

  void finishInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    // override in subclass if necessary
  }

  double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
  {
    // override in subclass to implement 2x2 interpolation
    return 0;
  }
}

