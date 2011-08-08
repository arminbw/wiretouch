class LinearInterpolator extends Interpolator2x2
{   
   LinearInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, float contrastLeft, float contrastRight)
   {
      super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, "linear interpolation", contrastLeft, contrastRight);
   }

   double interp_func(double a, double b, float f)
   {
      return a*(1.0-f) + b*f;
   }
}
