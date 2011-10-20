class HermiteInterpolator extends Interpolator4x4
{
   public double tension, bias;
   DecimalFormat _df;
   
   HermiteInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
   {
      super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "hermite interpolation [k][l]");
      this.bias = 0;
      this.tension = 0;
      this._df = new DecimalFormat("0.0");
   }

   String name()
   {
     return this.name + " (t=" + _df.format(tension) + ")";
   }

   double interp_func(double p0, double p1, double p2, double p3, double f)
   {
      double m0, m1, f2 = f*f, f3 = f2*f;
      
      m0 = (p1-p0)*(1.0+bias)*(1.0-tension)/2;
      m1 = (p2-p1)*(1.0+bias)*(1.0-tension)/2;
      m0 += m1;
      m1 += (p3-p2)*(1.0+bias)*(1.0-tension)/2;
      
      double i0 = 2*f3 - 3*f2 + 1;
      double i1 = f3 - 2*f2 + f;
      double i2 = f3 - f2;
      double i3 = -2.0*f3 + 3*f2;
      
      return (i0*p1 + i1*m0 + i2*m1 + i3*p2);
   }
}
