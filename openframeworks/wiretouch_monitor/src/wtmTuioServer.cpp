/* Wiretouch: an open capacitive multi-touch tracker
 * Copyright (C) 2011-2013 Georg Kaindl and Armin Wagner
 *
 * This file is part of WireTouch
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
 * along with WireTouch. If not, see <http://www.gnu.org/licenses/>.
 */

#include "wtmTuioServer.h"

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
        }
    }
    
    this->_cursorUpdated.clear();
    
    this->_tuioServer->stopUntouchedMovingCursors();
	this->_tuioServer->commitFrame();
    
	this->_time = TUIO::TuioTime::getSessionTime();
    
	this->_tuioServer->initFrame(this->_time);
}

