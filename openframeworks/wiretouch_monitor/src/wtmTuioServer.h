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
 * along with Wiretouch. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __wiretouch_monitor__tuio_server__
#define __wiretouch_monitor__tuio_server__

#include <map>

#include "ofMain.h"

#include "TuioServer.h"
#include "TuioCursor.h"

class wtmTuioServer {
public:
    wtmTuioServer();
    ~wtmTuioServer();
    
    void start(const char* host, unsigned port);
    
    TUIO::TuioTime getCurrentTime();
    
    TUIO::TuioCursor* registerCursorPosition(int identifier, float x, float y);
    
    void update();
    
protected:
    TUIO::TuioServer*     _tuioServer;
    TUIO::TuioTime        _time;
    
    std::map<int, TUIO::TuioCursor*> _cursors;
    std::map<int, bool> _cursorUpdated;
};

#endif /* defined(__wiretouch_monitor__tuio_server__) */
