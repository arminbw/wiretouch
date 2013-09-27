//
//  interpolator-cubic.cpp
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
 * along with Wiretouch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "interpolator-cubic.h"

wtmInterpolatorCubic::wtmInterpolatorCubic(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
}

double
wtmInterpolatorCubic::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double f2 = f*f;
    double i0 = p3 - p2 - p0 + p1;
    double i1 = p0 - p1 - i0;
    double i2 = p2 - p0;
    
    return i0*f*f2 + i1*f2 + i2*f + p1;
}

