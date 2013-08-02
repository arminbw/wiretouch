//
//  interpolator-cubic.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#include "interpolator-cubic.h"

wtmInterpolatorCubic::wtmInterpolatorCubic(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
}

double
wtmInterpolatorCubic::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double f2 = f*f;
    double i0 = p3 - p2 - p0 + p1;
    double i1 = p0 - p1 - i0;
    double i2 = p2 - p0;
    
    return i0*f*f2 + i1*f2 + i2*f + p1;
}

