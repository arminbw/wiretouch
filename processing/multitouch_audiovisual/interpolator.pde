class Interpolator
{
   int _x_samples, _y_samples;
   int _x, _y, _nx, _ny;
   float _fx, _fy;
   double[] _ip;
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
   
   void interpolate(Crosspoint[][] cp)
   {
      beginInterpolation(cp);
      
      for (int i=0; i<_x_samples-1; i++)
         for (int j=0; j<_y_samples-1; j++) {
            beginInterpolate4(cp, i, j);
            
            for (int k=0; k<_nx; k++)
               for (int l=0; l<_ny; l++) {
                  //this._ip[(j + l) * _x + i * _nx + k] = 
                  this._ip[((j * _ny + l) * _x) + i * _nx + k] = interpolate4(cp, i, j, k, l, _fx * (1.0 + 2*k), _fy * (1.0 + 2*l));
               }
            
            beginInterpolate4(cp, i, j);
         }
      
      finishInterpolation(cp);
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
   
   void finishInterpolate4(Crosspoint[] cp, int xm, int ym)
   {
      // override in subclass if necessary
   }
   
   double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
   {
      // override in subclass to implement 2x2 interpolation
      return 0;
   }
}
