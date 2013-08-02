//
//  interpolator-wnn.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#ifndef __wiretouch_monitor__interpolator_wnn__
#define __wiretouch_monitor__interpolator_wnn__

#include "interpolator4x4.h"

class wtmInterpolatorWNN : public wtmInterpolator4x4 {
public:
    wtmInterpolatorWNN(int sw, int sh, int wf, int hf);
    
    double exp;
    
protected:
    
    double interp_func(double p0, double p1, double p2, double p3, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_wnn__) */
