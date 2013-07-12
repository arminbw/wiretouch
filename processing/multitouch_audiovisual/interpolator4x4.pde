class Interpolator4x4 extends Interpolator
{
  double a, b, c, d;
  double t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

  /*
   *    t0    t1    t2    t3
   *    t4    a     b     t5
   *    t6    c     d     t7
   *    t8    t9    t10   t11
   */

  Interpolator4x4(int horizontalSamples, int verticalSamples, int horizontalMultiplier, int verticalMultiplier, int imageWidth, int imageHeight, String name)
  {
    super(horizontalSamples, verticalSamples, horizontalMultiplier, verticalMultiplier, imageWidth, imageHeight, name);
  }

  void beginInterpolate4(Crosspoint[][] cp, int xm, int ym)
  {
    a = sensorVal(cp, xm, ym);      
    b = sensorVal(cp, xm+1, ym);      
    c = sensorVal(cp, xm, ym+1); 
    d = sensorVal(cp, xm+1, ym+1);

    t0 = sensorVal(cp, xm-1, ym-1);
    t1 = sensorVal(cp, xm, ym-1);
    t2 = sensorVal(cp, xm+1, ym-1);
    t3 = sensorVal(cp, xm+2, ym-1);
    t4 = sensorVal(cp, xm-1, ym);
    t5 = sensorVal(cp, xm+2, ym);
    t6 = sensorVal(cp, xm-1, ym+1);
    t7 = sensorVal(cp, xm+2, ym+1);
    t8 = sensorVal(cp, xm-1, ym+1);
    t9 = sensorVal(cp, xm, ym+1);
    t10 = sensorVal(cp, xm+1, ym+1);
    t11 = sensorVal(cp, xm+2, ym+1);
  }

  double interp_func(double p0, double p1, double p2, double p3, double f)
  {
    // implement in subclass
    return 0;
  }

  double interpolate4(Crosspoint[][] cp, int xm, int ym, int ix, int iy, float fx, float fy)
  {
    double y0 = interp_func(t0, t1, t2, t3, fx);
    double y1 = interp_func(t4, a, b, t5, fx);
    double y2 = interp_func(t6, c, d, t7, fx);
    double y3 = interp_func(t8, t9, t10, t11, fx);

    return interp_func(y0, y1, y2, y3, fy);
  }
}

