//
//  interpolator-cosine.cpp
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

#include "interpolator-cosine.h"

wtmInterpolatorCosine::wtmInterpolatorCosine(int sw, int sh, int wf, int hf)
: wtmInterpolator2x2(sw, sh, wf, hf)
{
}

double
wtmInterpolatorCosine::interp_func(double a, double b, double f)
{
    double f2 = (1.0-cos(f*PI))/2.0;
    
    return a*(1.0-f2) + b*f2;
}
