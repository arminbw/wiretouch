//
//  interpolator2x2.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

#ifndef __wiretouch_monitor__interpolator2x2__
#define __wiretouch_monitor__interpolator2x2__

#include "interpolator.h"

class wtmInterpolator2x2 : wtmInterpolator {
    public:
        wtmInterpolator2x2(int sw, int sh, int wf, int hf);
    
    protected:
        double a, b, c, d;
    
        double interp_func(double a, double b, float f);
        double interpolate4(int x, int y, int ix, int iy, float fx, float fy);
        void beginInterpolate4(int x, int y);
};

#endif /* defined(__wiretouch_monitor__interpolator2x2__) */
