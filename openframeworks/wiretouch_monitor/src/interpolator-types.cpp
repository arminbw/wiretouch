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

#include "interpolator-types.h"

#include "interpolator-linear.h"
#include "interpolator-cosine.h"
#include "interpolator-cubic.h"
#include "interpolator-hermite.h"
#include "interpolator-catmull-rom.h"
#include "interpolator-wnn.h"
#include "interpolator-lagrange.h"


wtmInterpolator* wtmInterpolatorOfType(wtmInterpolatorType aType,
                                       int width,
                                       int height,
                                       int upsample_width,
                                       int upsample_height)
{
    wtmInterpolator* anInterpolator = NULL;
    
    switch (aType) {
        case wtmInterpolatorTypeLinear:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorLinear(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeCosine:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCosine(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeCubic:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCubic(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeHermite:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorHermite(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeCatmullRom:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCatmullRom(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeWNN:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorWNN(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeLagrange:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorLagrange(width, height, upsample_width, upsample_height);
            break;
            
        case wtmInterpolatorTypeUnknown:
        default:
            break;
            
    }
    
    return anInterpolator;
}
