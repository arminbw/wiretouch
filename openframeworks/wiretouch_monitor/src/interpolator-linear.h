//
//  interpolator-linear.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#ifndef __wiretouch_monitor__interpolator_linear__
#define __wiretouch_monitor__interpolator_linear__

#include "interpolator2x2.h"

class wtmInterpolatorLinear : public wtmInterpolator2x2 {
public:
    wtmInterpolatorLinear(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double a, double b, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_linear__) */
