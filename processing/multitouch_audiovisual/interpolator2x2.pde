class Interpolator2x2 extends Interpolator
{
   double a, b, c, d;
   
   Interpolator2x2(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, String name, float contrastLeft, float contrastRight)
   {
      super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, name, contrastLeft, contrastRight);
   }
  
   double interp_func(double a, double b, float f)
   {
      // implement in subclass
      return 0;
   }
  
   void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
   {
      a = sensorVal(cp, xm, ym);
      b = sensorVal(cp, xm+1, ym);
      c = sensorVal(cp, xm, ym+1);
      d = sensorVal(cp, xm+1, ym+1);
   }
  
   double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
   {
      double i1 = interp_func(a, b, fx);
      double i2 = interp_func(c, d, fx);
            
      return interp_func(i1, i2, fy);
   }
}
