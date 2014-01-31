//
//  interpolator-hermite.h
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

#ifndef __wiretouch_monitor__interpolator_hermite__
#define __wiretouch_monitor__interpolator_hermite__

#include "interpolator4x4.h"

class wtmInterpolatorHermite : public wtmInterpolator4x4 {
public:
    double tension, bias;
    
    wtmInterpolatorHermite(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double p0, double p1, double p2, double p3, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_hermite__) */
