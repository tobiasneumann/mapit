/*******************************************************************************
 *
 * Copyright 2016-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
 *
******************************************************************************/

/*  This file is part of mapit.
 *
 *  Mapit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Mapit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mapit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ZMQENTITYDATASTREAMPROVIDER_H
#define ZMQENTITYDATASTREAMPROVIDER_H

#include <upns/typedefs.h>
#include <upns/versioning/repository.h>
#include "zmqprotobufnode.h"
#include <upns/operators/serialization/abstractentitydataprovider.h>

namespace upns
{

/**
 * @brief ZmqEntitydataStreamProvider sends binary data over network
 */

class ZmqEntitydataStreamProvider : public AbstractEntitydataProvider
{
    // AbstractEntitydataProvider interface
public:
    ZmqEntitydataStreamProvider(std::string checkoutName, std::string pathOrOid, ZmqProtobufNode *node);
    bool isCached();
    bool isReadWriteSame();
    upnsIStream *startRead(upnsuint64 start, upnsuint64 length);
    void endRead(upnsIStream *&strm);
    upnsOStream *startWrite(upnsuint64 start, upnsuint64 len);
    void endWrite(upnsOStream *&strm);
    upnsuint64 getStreamSize() const;
    void setStreamSize(upnsuint64 entitylength);
    LockHandle lock();
    void unlock(LockHandle);

    const void *startReadPointer(ReadWriteHandle &handle, upnsuint64 start, upnsuint64 len);
    void endReadPointer(const void *ptr, ReadWriteHandle &handle);
    void *startWritePointer(ReadWriteHandle &handle, upnsuint64 start, upnsuint64 len);
    void endWritePointer(void *ptr, ReadWriteHandle &handle);
    char *startRead(upnsuint64 start, upnsuint64 length, upnsuint64 &outLength);
    //void endRead();
    void endWrite(const char *memory, const upns::upnsuint64 &length, const upnsuint64 &offset);

    std::string startReadFile(ReadWriteHandle &handle);
    void endReadFile(ReadWriteHandle &handle);
    std::string startWriteFile(ReadWriteHandle &handle);
    void endWriteFile(ReadWriteHandle &handle);

    ReadWriteType preferredReadType();
    ReadWriteType preferredWriteType();
private:
    std::string m_checkoutName;
    std::string m_pathOrOid;
    ZmqProtobufNode *m_node;
    mutable upnsuint64 m_entityLength;
};

}
#endif // ZMQENTITYDATASTREAMPROVIDER_H
