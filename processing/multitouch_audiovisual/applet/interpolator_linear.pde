class LinearInterpolator extends Interpolator2x2
{   
  LinearInterpolator(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, "linear interpolation");
  }

  double interp_func(double a, double b, float f)
  {
    return a*(1.0-f) + b*f;
  }
}

