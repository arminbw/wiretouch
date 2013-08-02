//
//  interpolator-linear.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#include "interpolator-linear.h"

wtmInterpolatorLinear::wtmInterpolatorLinear(int sw, int sh, int wf, int hf)
: wtmInterpolator2x2(sw, sh, wf, hf)
{
}

double
wtmInterpolatorLinear::interp_func(double a, double b, double f)
{
    return a*(1.0-f) + b*f;
}