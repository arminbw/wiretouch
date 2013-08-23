//
//  tuio-server.cpp
//  wiretouch_monitor
//
//  Created by Georg Kaindl on 23/8/13.
//
//

#include "tuio-server.h"


wtmTuioServer::wtmTuioServer()
{
}

wtmTuioServer::~wtmTuioServer()
{
    delete this->_tuioServer;
}

void
wtmTuioServer::start(const char* host, unsigned port)
{
    this->_tuioServer   = new TUIO::TuioServer(host, port);
    this->_time         = TUIO::TuioTime::getSessionTime();
}

TUIO::TuioCursor*
wtmTuioServer::registerCursorPosition(int identifier, float x, float y)
{
    TUIO::TuioCursor* aCursor = NULL;
    
    if (this->_cursors.count(identifier)) {
        aCursor = this->_cursors[identifier];
        this->_tuioServer->updateTuioCursor(aCursor, x, y);
    } else {
        aCursor = this->_tuioServer->addTuioCursor(x, y);
        this->_cursors[identifier] = aCursor;
    }
    
    this->_cursorUpdated[identifier] = true;
    
    return aCursor;
}

TUIO::TuioTime
wtmTuioServer::getCurrentTime()
{
    return this->_tuioServer->getFrameTime();
}

void
wtmTuioServer::update()
{
    for(map<int, TUIO::TuioCursor*>::iterator ii=this->_cursors.begin(); ii!=this->_cursors.end(); ++ii) {
        int identifier = (*ii).first;
        
        if (!this->_cursorUpdated.count(identifier) && this->_cursors.count(identifier)) {
            TUIO::TuioCursor* aCursor = this->_cursors[identifier];
            this->_tuioServer->removeTuioCursor(aCursor);
            this->_cursors.erase(identifier);
            
            printf("remove cursor : %d\n", identifier);
        }
    }
    
    this->_cursorUpdated.clear();
    
    this->_tuioServer->stopUntouchedMovingCursors();
	this->_tuioServer->commitFrame();
    
	this->_time = TUIO::TuioTime::getSessionTime();
    
	this->_tuioServer->initFrame(this->_time);
}

