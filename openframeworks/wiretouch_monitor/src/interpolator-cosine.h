//
//  interpolator-cosine.h
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

#ifndef __wiretouch_monitor__interpolator_cosine__
#define __wiretouch_monitor__interpolator_cosine__

#include "interpolator2x2.h"

class wtmInterpolatorCosine : public wtmInterpolator2x2 {
public:
    wtmInterpolatorCosine(int sw, int sh, int wf, int hf);
    
protected:
    
    double interp_func(double a, double b, double f);
};

#endif /* defined(__wiretouch_monitor__interpolator_cosine__) */
