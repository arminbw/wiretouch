/* WireTouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of WireTouch
 *
 * WireTouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WireTouch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WireTouch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "interpolator-lagrange.h"

wtmInterpolatorLagrange::wtmInterpolatorLagrange(int sw, int sh, int wf, int hf)
: wtmInterpolator4x4(sw, sh, wf, hf)
{
}

double
wtmInterpolatorLagrange::interp_func(double p0, double p1, double p2, double p3, double f)
{
    double p[4] = { p0, p1, p2, p3 }, mp[4] = {-1.0, 0, 1.0, 2.0 }, s = 0.0, n, d;
    
    for (int i=0; i<4; i++) {
        n = d = 1.0;
        
        for (int j=0; j<4; j++) {
            if (j==i)
                continue;
            
            n *= f - mp[j];
            d *= mp[i] - mp[j];
        }
        
        s += (n / d) * p[i];
    }
    
    return s;
}

