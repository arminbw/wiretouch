//
//  interpolator-hermite.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
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
 * along with rfm12b-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include "interpolator-hermite.h"

wtmInterpolatorHermite::wtmInterpolatorHermite(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
    this->bias = 0.0;
    this->tension = 0.0;
}

double
wtmInterpolatorHermite::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double m0, m1, f2 = f*f, f3 = f2*f;
    
    m0 = (p1-p0)*(1.0+bias)*(1.0-tension)/2;
    m1 = (p2-p1)*(1.0+bias)*(1.0-tension)/2;
    m0 += m1;
    m1 += (p3-p2)*(1.0+bias)*(1.0-tension)/2;
    
    double i0 = 2*f3 - 3*f2 + 1;
    double i1 = f3 - 2*f2 + f;
    double i2 = f3 - f2;
    double i3 = -2.0*f3 + 3*f2;
    
    return (i0*p1 + i1*m0 + i2*m1 + i3*p2);
}

