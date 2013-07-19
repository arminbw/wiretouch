//
//  interpolator4x4.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

#ifndef __wiretouch_monitor__interpolator4x4__
#define __wiretouch_monitor__interpolator4x4__

#include "interpolator.h"

class wtmInterpolator4x4 : public wtmInterpolator {
    public:
        wtmInterpolator4x4(int sw, int sh, int wf, int hf);
    
    protected:
        /*
         *    t0    t1    t2    t3
         *    t4    a     b     t5
         *    t6    c     d     t7
         *    t8    t9    t10   t11
         */
        double a, b, c, d, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;
    
        virtual double interp_func(double p0, double p1, double p2, double p3, double f);
        double interpolate4(int x, int y, int ix, int iy, float fx, float fy);
        void beginInterpolate4(int x, int y);
};

#endif /* defined(__wiretouch_monitor__interpolator4x4__) */
