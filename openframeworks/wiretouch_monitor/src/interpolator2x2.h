//
//  interpolator2x2.h
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
 * along with rfm12b-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __wiretouch_monitor__interpolator2x2__
#define __wiretouch_monitor__interpolator2x2__

#include "interpolator.h"

class wtmInterpolator2x2 : public wtmInterpolator {
    public:
        wtmInterpolator2x2(int sw, int sh, int wf, int hf);
    
    protected:
        double a, b, c, d;
    
        virtual double interp_func(double a, double b, double f);
        double interpolate4(int x, int y, int ix, int iy, float fx, float fy);
        void beginInterpolate4(int x, int y);
};

#endif /* defined(__wiretouch_monitor__interpolator2x2__) */
