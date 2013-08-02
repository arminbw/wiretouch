//
//  interpolator-cosine.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#include "interpolator-cosine.h"

wtmInterpolatorCosine::wtmInterpolatorCosine(int sw, int sh, int wf, int hf)
: wtmInterpolator2x2(sw, sh, wf, hf)
{
}

double
wtmInterpolatorCosine::interp_func(double a, double b, double f)
{
    double f2 = (1.0-cos(f*PI))/2.0;
    
    return a*(1.0-f2) + b*f2;
}
