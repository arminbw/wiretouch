//
//  interpolator.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

#ifndef __wiretouch_monitor__interpolator__
#define __wiretouch_monitor__interpolator__


class wtmInterpolator {
    public:
        wtmInterpolator(int sw, int sh, int wf, int hf);
    
        void runInterpolation(uint16_t* sensorValues);

    protected:
        int sourceWidth, sourceHeight;
        int widthUpsampling, heightUpsampling;
        int interpolatedWidth, interpolatedHeight;
        double _fx, _fy;
    
        double* interpolatedBuffer;
        uint16_t* values;
    
        double sensorValueAt(int x, int y);
    
        void beginInterpolation();
        void finishInterpolation();
        void beginInterpolate4(int x, int y);
        void finishInterpolate4(int x, int y);
        double interpolate4(int x, int y, int ix, int iy, float fx, float fy);
};

#endif /* defined(__wiretouch_monitor__interpolator__) */
