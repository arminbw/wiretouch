class CatmullRomInterpolator extends Interpolator4x4
{
   CatmullRomInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, float contrastLeft, float contrastRight)
   {
      super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, "catmull-rom spline interpolation", contrastLeft, contrastRight);
   }

   double interp_func(double p0, double p1, double p2, double p3, double f)
   {
      double f2 = f*f;
      double i0 = -.5*p0 + 1.5*p1 - 1.5*p2 + .5*p3;
      double i1 = p0 - 2.5*p1 + 2*p2 - 0.5*p3;
      double i2 = -.5*p0 + 0.5*p2;
      
      return i0*f*f2 + i1*f2 + i2*f + p1;
   }
}
