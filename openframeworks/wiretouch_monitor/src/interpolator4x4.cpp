//
//  interpolator4x4.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

/* Wiretouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of Wiretouch
 *
 * Wiretouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wiretouch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wiretouch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "interpolator4x4.h"

wtmInterpolator4x4::wtmInterpolator4x4(int sw, int sh, int wf, int hf)
    : wtmInterpolator(sw, sh, wf, hf)
{
}

double
wtmInterpolator4x4::interp_func(double p0, double p1, double p2, double p3, double f)
{
    return 0.0;
}

double
wtmInterpolator4x4::interpolate4(int x, int y, int ix, int iy, float fx, float fy)
{
    double y0 = this->interp_func(this->t0, this->t1, this->t2,  this->t3,  fx);
    double y1 = this->interp_func(this->t4, this->a,  this->b,   this->t5,  fx);
    double y2 = this->interp_func(this->t6, this->c,  this->d,   this->t7,  fx);
    double y3 = this->interp_func(this->t8, this->t9, this->t10, this->t11, fx);
    
    return this->interp_func(y0, y1, y2, y3, fy);
}

void
wtmInterpolator4x4::beginInterpolate4(int x, int y)
{
    this->a     = this->sensorValueAt(x,   y);
    this->b     = this->sensorValueAt(x+1, y);
    this->c     = this->sensorValueAt(x,   y+1);
    this->d     = this->sensorValueAt(x+1, y+1);
    
    this->t0    = this->sensorValueAt(x-1, y-1);
    this->t1    = this->sensorValueAt(x,   y-1);
    this->t2    = this->sensorValueAt(x+1, y-1);
    this->t3    = this->sensorValueAt(x+2, y-1);
    this->t4    = this->sensorValueAt(x-1, y);
    this->t5    = this->sensorValueAt(x+2, y);
    this->t6    = this->sensorValueAt(x-1, y+1);
    this->t7    = this->sensorValueAt(x+2, y+1);
    this->t8    = this->sensorValueAt(x-1, y+1);
    this->t9    = this->sensorValueAt(x,   y+1);
    this->t10   = this->sensorValueAt(x+1, y+1);
    this->t11   = this->sensorValueAt(x+2, y+1);
}
