//
//  interpolator-wnn.cpp
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

#include "interpolator-wnn.h"

wtmInterpolatorWNN::wtmInterpolatorWNN(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
    this->exp = 2.0;
}

double
wtmInterpolatorWNN::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double w, c = 0.0, p[4] = { p0, p1, p2, p3 }, e = 0.0, ppos;
    for (int i=0; i<4; i++) {
        ppos = i - 1.0;
        if (abs(ppos - f) > FLT_EPSILON) {
            w = pow(pow((ppos - f), 2.0), this->exp/2.0);
            e += p[i] / w;
            c += 1.0 / w;
        } else {
            return p[i];
        }
    }
    
    return e / c;
}
