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


wtmInterpolator* wtmInterpolatorOfType(wtmInterpolatorType aType,
                                       int width,
                                       int height,
                                       int upsample_width,
                                       int upsample_height)
{
    wtmInterpolator* anInterpolator = NULL;
    
    switch (aType) {
        case wtmInterpolatorTypeTypeLinear:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorLinear(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeTypeCosine:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCosine(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeTypeCubic:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCubic(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeTypeHermite:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorHermite(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeTypeCatmullRom:
            anInterpolator =
                (wtmInterpolator*)new wtmInterpolatorCatmullRom(width, height, upsample_width, upsample_height);
            break;
        
        case wtmInterpolatorTypeTypeUnknown:
        default:
            break;
            
    }
    
    return anInterpolator;
}