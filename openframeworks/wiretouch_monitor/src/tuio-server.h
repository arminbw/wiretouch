//
//  tuio-server.h
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 23/8/13.
//
//

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
