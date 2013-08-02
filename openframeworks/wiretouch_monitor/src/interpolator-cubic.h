//
//  interpolator-cubic.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#ifndef __wiretouch_monitor__interpolator_cubic__
#define __wiretouch_monitor__interpolator_cubic__

#include "interpolator4x4.h"

class wtmInterpolatorCubic : public wtmInterpolator4x4 {
public:
    wtmInterpolatorCubic(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double p0, double p1, double p2, double p3, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_cubic__) */
