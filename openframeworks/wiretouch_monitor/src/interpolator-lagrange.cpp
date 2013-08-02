//
//  interpolator-lagrange.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

#include "interpolator-lagrange.h"

wtmInterpolatorLagrange::wtmInterpolatorLagrange(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
}

double
wtmInterpolatorLagrange::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double p[4] = { p0, p1, p2, p3 }, mp[4] = {-1.0, 0, 1.0, 2.0 }, s = 0.0, n, d;
    
    for (int i=0; i<4; i++) {
        n = d = 1.0;
        
        for (int j=0; j<4; j++) {
            if (j==i)
                continue;
            
            n *= f - mp[j];
            d *= mp[i] - mp[j];
        }
        
        s += (n / d) * p[i];
    }
    
    return s;
}

