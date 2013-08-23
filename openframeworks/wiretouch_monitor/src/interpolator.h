//
//  interpolator.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 12/7/13.
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
