//
//  interpolator.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
//
//

#ifndef __wiretouch_monitor__interpolator__
#define __wiretouch_monitor__interpolator__

#include "ofMain.h"

class wtmInterpolator {
    public:
        wtmInterpolator(int sw, int sh, int wf, int hf);
        ~wtmInterpolator();
    
        void runInterpolation(uint16_t* sensorValues);
    
        ofTexture* currentTexture();
        const unsigned char* currentPixels();
    
        int getOutputWidth();
        int getOutputHeight();

    protected:
        int sourceWidth, sourceHeight;
        int widthUpsampling, heightUpsampling;
        int interpolatedWidth, interpolatedHeight;
        double _fx, _fy;
    
        double* interpolatedBuffer;
        unsigned char* pixels;
        uint16_t* values;
    
        ofTexture texture;
    
        double sensorValueAt(int x, int y);
    
        virtual void beginInterpolation();
        virtual void finishInterpolation();
        virtual void beginInterpolate4(int x, int y);
        virtual void finishInterpolate4(int x, int y);
        virtual double interpolate4(int x, int y, int ix, int iy, float fx, float fy);
};

#endif /* defined(__wiretouch_monitor__interpolator__) */
