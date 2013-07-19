//
//  interpolator-catmull-rom.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 19/7/13.
//
//

#ifndef __wiretouch_monitor__interpolator_catmull_rom__
#define __wiretouch_monitor__interpolator_catmull_rom__

#include "interpolator4x4.h"

class wtmInterpolatorCatmullRom : public wtmInterpolator4x4 {
public:
    wtmInterpolatorCatmullRom(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double p0, double p1, double p2, double p3, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_catmull_rom__) */
