//
//  interpolator-wnn.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#include "interpolator-wnn.h"

wtmInterpolatorWNN::wtmInterpolatorWNN(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
    this->exp = 2.0;
}

double
wtmInterpolatorWNN::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double w, c = 0.0, p[4] = { p0, p1, p2, p3 }, e = 0.0, ppos;
    for (int i=0; i<4; i++) {
        ppos = i - 1.0;
        if (abs(ppos - f) > FLT_EPSILON) {
            w = pow(pow((ppos - f), 2.0), this->exp/2.0);
            e += p[i] / w;
            c += 1.0 / w;
        } else {
            return p[i];
        }
    }
    
    return e / c;
}
