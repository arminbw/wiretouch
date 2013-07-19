//
//  interpolator-catmull-rom.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 19/7/13.
//
//

#include "interpolator-catmull-rom.h"

wtmInterpolatorCatmullRom::wtmInterpolatorCatmullRom(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
}

double wtmInterpolatorCatmullRom::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double f2 = f*f;
    double i0 = -.5*p0 + 1.5*p1 - 1.5*p2 + .5*p3;
    double i1 = p0 - 2.5*p1 + 2*p2 - 0.5*p3;
    double i2 = -.5*p0 + 0.5*p2;
    
    return i0*f*f2 + i1*f2 + i2*f + p1;
}
