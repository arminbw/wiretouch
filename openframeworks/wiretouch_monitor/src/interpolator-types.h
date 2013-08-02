//
//  interpolator-types.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 2/8/13.
//
//

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
