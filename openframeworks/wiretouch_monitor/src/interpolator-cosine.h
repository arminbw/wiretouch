//
//  interpolator-cosine.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#ifndef __wiretouch_monitor__interpolator_cosine__
#define __wiretouch_monitor__interpolator_cosine__

#include "interpolator2x2.h"

class wtmInterpolatorCosine : public wtmInterpolator2x2 {
public:
    wtmInterpolatorCosine(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double a, double b, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_cosine__) */
