class Interpolator
{
   static final int histogramBins = 256;
   
   boolean bContrastStretch; 
   float fStretchHistLeft, fStretchHistRight;
   
   int _x_samples, _y_samples;
   int _x, _y, _nx, _ny;
   float _fx, _fy;
   double[] _ip;
   int[] _hist;
   int _histMax;
   String _name;
   
   Interpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, String name)
   {
      this._x_samples = horizontalSamples;
      this._y_samples = verticalSamples;
      this._nx = horizontalMultiplier;
      this._ny = verticalMultiplier;
      this._x = (_x_samples - 1) * _nx;
      this._y = (_y_samples -1) * _ny;
      
      this._fx = 1.0 / (2.0 * (float)_nx);
      this._fy = 1.0 / (2.0 * (float)_ny);
      
      this._ip = new double[_x * _y];
      this._hist = new int[histogramBins];
      
      this.bContrastStretch = false;
      this.fStretchHistLeft = 0.0;
      this.fStretchHistRight = 0.0;
      
      this._name = name;
   }
   
   String name()
   {
     return _name;
   }
   
   int getPixelWidth()
   {
      return _x;
   }
   
   int getPixelHeight()
   {
      return _y;
   }
   
   double[] getPixels()
   {
      return _ip;
   }
   
   // includes value repetition at the borders
   double sensorVal(Crosspoint[][] cp, int x, int y)
   {
      x = constrain(x, 0, _x_samples-1);
      y = constrain(y, 0, _y_samples-1);
      
      return cp[x][y].signalStrength;
   }
   
   void drawInRect(int x1, int y1, int x2, int y2)
   {  
      noStroke();
            
      float sx = (float)(x2-x1) / (float)_x;
      float sy = (float)(y2-y1) / (float)_y;
      float tx = x1 * (-sx + 1);
      float ty = y1 * (-sy + 1);
      
      translate(tx, ty);
      scale(sx, sy);
      
      for (int i=0; i<_x; i++)
         for (int j=0; j<_y; j++) {
            fill(color((float)_ip[j * _x + i]*255.0));
            rect(x1 + i*1, y1 + j*1, 1, 1);
         }
         
      scale(1.0/sx, 1.0/sy);
      translate(-tx, -ty);
   }
   
   void drawByWidthPreservingAspectRatio(int x1, int y1, int x2)
   {
      int w = x2 - x1;
      float r = (float)_y / (float)_x;
      
      drawInRect(x1, y1, x2, y1 + (int)(w*r + 0.5));
   }
   
   void drawHistogramFromPoint(int x1, int y1, int maxY)
   {
      float step = (float)maxY / (float)log(this._histMax);
      
      strokeWeight(1.0);
      for (int i=0; i<histogramBins; i++) {
         int h = (int)(constrain(log(this._hist[i]) * step, 0.0, maxY));
         
         stroke(color(i));
         line(x1+i, y1, x1+i, y1-maxY);  
         
         stroke(color(239, 171, 233));
         line(x1+i, y1, x1+i, y1-h);
      }
   }
   
   void interpolate(Crosspoint[][] cp)
   {
      beginInterpolation(cp);

      Arrays.fill(this._hist, 0);
      this._histMax = 0;
      
      for (int i=0; i<_x_samples-1; i++)
         for (int j=0; j<_y_samples-1; j++) {
            beginInterpolate4(cp, i, j);
            
            for (int k=0; k<_nx; k++)
               for (int l=0; l<_ny; l++) {
                  double val =  interpolate4(cp, i, j, k, l, _fx * (1.0 + 2*k), _fy * (1.0 + 2*l));
                  val = constrain((float)val, 0.0, 1.0);

                  int p = (int)(val/(1.0/((double)histogramBins-1)));
                  _hist[p]++;
                  if (_hist[p] > this._histMax)
                     this._histMax = _hist[p];
                  
                  this._ip[((j * _ny + l) * _x) + i * _nx + k] = val;
               }
            
            finishInterpolate4(cp, i, j);
         }
      
      finishInterpolation(cp);
      
      if (this.bContrastStretch) {
         float vmin = 0.0, vmax = 1.0;
      
         int ss, nHist = (int)(this._x * this._y * this.fStretchHistLeft);      
         for (ss = 0; nHist > 0 && ss<histogramBins; nHist -= this._hist[ss++]);
         vmin = (1.0/histogramBins) * ss;
      
         nHist = (int)(this._x * this._y * this.fStretchHistRight);
         for (ss = histogramBins-1; nHist > 0 && ss>=0; nHist -= this._hist[ss--]);
         vmax = (1.0/histogramBins) * ss;
            
         for (int i=0; i<_x; i++)
            for (int j=0; j<_y; j++) {
               double p = _ip[j * _x + i];
               _ip[j * _x + i] = (p - vmin) / (vmax - vmin);
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
