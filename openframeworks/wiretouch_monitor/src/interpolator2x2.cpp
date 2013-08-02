//
//  interpolator2x2.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

#include "interpolator2x2.h"

wtmInterpolator2x2::wtmInterpolator2x2(int sw, int sh, int wf, int hf)
    : wtmInterpolator(sw, sh, wf, hf)
{
}

double
wtmInterpolator2x2::interp_func(double a, double b, double f)
{
    return 0.0;
}

double
wtmInterpolator2x2::interpolate4(int x, int y, int ix, int iy, float fx, float fy)
{
    double i1 = this->interp_func(this->a, this->b, fx);
    double i2 = this->interp_func(this->c, this->d, fx);
    
    return this->interp_func(i1, i2, fy);
}

void
wtmInterpolator2x2::beginInterpolate4(int x, int y)
{
    this->a = this->sensorValueAt(x, y);
    this->b = this->sensorValueAt(x+1, y);
    this->c = this->sensorValueAt(x, y+1);
    this->d = this->sensorValueAt(x+1, y+1);
}

