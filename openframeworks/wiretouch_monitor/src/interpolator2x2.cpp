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

