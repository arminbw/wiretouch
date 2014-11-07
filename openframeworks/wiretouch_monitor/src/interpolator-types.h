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

#ifndef wiretouch_monitor_interpolator_types_h
#define wiretouch_monitor_interpolator_types_h

#include "interpolator.h"

typedef enum _wtmInterpolatorType {
    wtmInterpolatorTypeUnknown = 0,
    wtmInterpolatorTypeLinear,
    wtmInterpolatorTypeCosine,
    wtmInterpolatorTypeCubic,
    wtmInterpolatorTypeHermite,
    wtmInterpolatorTypeCatmullRom,
    wtmInterpolatorTypeWNN,
    wtmInterpolatorTypeLagrange
} wtmInterpolatorType;

wtmInterpolator* wtmInterpolatorOfType(wtmInterpolatorType aType,
                                       int width,
                                       int height,
                                       int upsample_width,
                                       int upsample_height);

#endif
