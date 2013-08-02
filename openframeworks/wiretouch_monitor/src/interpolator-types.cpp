//
//  interpolator-types.c
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

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