/*******************************************************************************
 *
 * Copyright 2015-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
 *           2015-2017 Tobias Neumann	<t.neumann@fh-aachen.de>
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

#include "mapit/layertypes/boundingboxlayer.h"
#include <sstream>
#include <mapit/logging.h>

const char *BoundingboxEntitydata::TYPENAME()
{
    return PROJECT_NAME;
}


BoundingboxEntitydata::BoundingboxEntitydata(std::shared_ptr<mapit::AbstractEntitydataProvider> streamProvider)
    :m_streamProvider( streamProvider ),
     m_aabb( NULL )
{
}

const char* BoundingboxEntitydata::type() const
{
    return BoundingboxEntitydata::TYPENAME();
}

bool BoundingboxEntitydata::hasFixedGrid() const
{
    return false;
}

bool BoundingboxEntitydata::canSaveRegions() const
{
    return false;
}

mapit::msgs::BoundingboxPtr BoundingboxEntitydata::getData(float x1, float y1, float z1,
                                                float x2, float y2, float z2,
                                                bool clipMode,
                                                int lod)
{
    if(m_aabb == NULL)
    {
        m_aabb = mapit::msgs::BoundingboxPtr(new mapit::msgs::Boundingbox);
        mapit::istream *in = m_streamProvider->startRead();
        {
            if(!m_aabb->ParseFromIstream(in))
            {
                log_warn("Could not read boundingbox from stream. Proceeding with identity aabb (-1,1,1,-1)");
                m_aabb->set_x1(-1); // left
                m_aabb->set_y1(1); // top
                m_aabb->set_x2(1); // right
                m_aabb->set_y2(-1); // bottom
            }
        }
        m_streamProvider->endRead(in);
    }
    return m_aabb;
}

int BoundingboxEntitydata::setData(float x1, float y1, float z1,
                                 float x2, float y2, float z2,
                                 mapit::msgs::BoundingboxPtr &data,
                                 int lod)
{
    mapit::ostream *out = m_streamProvider->startWrite();
    {
        m_aabb->SerializePartialToOstream(out);
    }
    m_streamProvider->endWrite(out);
	return 0; //TODO: MSVC: What to return here?
}

mapit::msgs::BoundingboxPtr BoundingboxEntitydata::getData(int lod)
{
    return getData(-std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                   false, lod);
}

int BoundingboxEntitydata::setData(mapit::msgs::BoundingboxPtr &data, int lod)
{
    return setData(-std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                   data, lod);
}

void BoundingboxEntitydata::gridCellAt(float   x, float   y, float   z,
                                     float &x1, float &y1, float &z1,
                                     float &x2, float &y2, float &z2) const
{
    x1 = -std::numeric_limits<float>::infinity();
    y1 = -std::numeric_limits<float>::infinity();
    z1 = -std::numeric_limits<float>::infinity();
    x2 = +std::numeric_limits<float>::infinity();
    y2 = +std::numeric_limits<float>::infinity();
    z2 = +std::numeric_limits<float>::infinity();
}

int BoundingboxEntitydata::getEntityBoundingBox(float &x1, float &y1, float &z1,
                                              float &x2, float &y2, float &z2)
{
    //TODO
    return 0;
}

mapit::istream *BoundingboxEntitydata::startReadBytes(mapit::uint64_t start, mapit::uint64_t len)
{
    return m_streamProvider->startRead(start, len);
}

void BoundingboxEntitydata::endRead(mapit::istream *&strm)
{
    m_streamProvider->endRead(strm);
}

mapit::ostream *BoundingboxEntitydata::startWriteBytes(mapit::uint64_t start, mapit::uint64_t len)
{
    return m_streamProvider->startWrite(start, len);
}

void BoundingboxEntitydata::endWrite(mapit::ostream *&strm)
{
    m_streamProvider->endWrite(strm);
}

size_t BoundingboxEntitydata::size() const
{
    return m_streamProvider->getStreamSize();
}

// Win32 does not like anything but void pointers handled between libraries
// For Unix there would be a hack to use a "custom deleter" which is given to the library to clean up the created memory
// the common denominator is to build pointer with custom deleter in our main programm and just exchange void pointers.
//std::shared_ptr<mapit::AbstractEntitydata> createEntitydata(std::shared_ptr<AbstractEntitydataProvider> streamProvider)
//void* createEntitydata(std::shared_ptr<AbstractEntitydataProvider> streamProvider)
void deleteEntitydataBB(mapit::AbstractEntitydata *ld)
{
    BoundingboxEntitydata *p = dynamic_cast<BoundingboxEntitydata*>(ld);
    if(p)
    {
        delete p;
    }
    else
    {
        log_error("Wrong entitytype");
    }
}

void createEntitydata(std::shared_ptr<mapit::AbstractEntitydata> *out, std::shared_ptr<mapit::AbstractEntitydataProvider> streamProvider)
{
    //return std::shared_ptr<mapit::AbstractEntitydata>(new PointcloudEntitydata( streamProvider ), deleteWrappedLayerData);
    *out = std::shared_ptr<mapit::AbstractEntitydata>(new BoundingboxEntitydata( streamProvider ), deleteEntitydataBB);
}
