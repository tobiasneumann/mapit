/*******************************************************************************
 *
 * Copyright 2015-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
 *           2015-2016 Tobias Neumann	<t.neumann@fh-aachen.de>
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

#include "mapit/layertypes/grid2d.h"
#include <mapit/logging.h>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <mapit/msgs/datastructs.pb.h>
#include <iostream>
#include <fstream>

using namespace mapit::entitytypes;

const char *Grid2D::TYPENAME()
{
    return PROJECT_NAME;
}

Grid2D::Grid2D(std::shared_ptr<mapit::AbstractEntitydataProvider> streamProvider)
    :m_streamProvider( streamProvider ),
     grid2d_(nullptr)
{
}

const char *Grid2D::type() const
{
    return Grid2D::TYPENAME();
}
bool Grid2D::hasFixedGrid() const
{
    return false;
}

bool Grid2D::canSaveRegions() const
{
    return false;
}



std::shared_ptr<Grid2DHelper> Grid2D::getData(float x1, float y1, float z1, float x2, float y2, float z2, bool clipMode, int lod)
{

    if (grid2d_ == nullptr)
    {
        mapit::ReadWriteHandle handle;
        std::string filename = m_streamProvider->startReadFile(handle);
        {
            // protobuff searilaze as/from string
            // seems like direct parse to/from stream is possible
            std::fstream input(filename, std::ios::in | std::ios::binary);
            mapit::msgs::Grid2D msg_grid;
            msg_grid.ParseFromIstream(&input);

            grid2d_ = std::make_shared<Grid2DHelper>();
            grid2d_->setGrid(msg_grid);
        }
        m_streamProvider->endReadFile(handle);
    }

    return grid2d_;
}


int Grid2D::setData(float x1, float y1, float z1,
                    float x2, float y2, float z2, std::shared_ptr<Grid2DHelper> &data,
                    int lod)
{
    int result = -1;
    mapit::ReadWriteHandle handle;
    std::string filename = m_streamProvider->startWriteFile(handle);
    {
        std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
        result = data->getGrid().SerializeToOstream(&output);
    }
    m_streamProvider->endWriteFile(handle);
    return result;
}

std::shared_ptr<Grid2DHelper> Grid2D::getData(int lod)
{
    return getData(-std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                   false, lod);
}

int Grid2D::setData(std::shared_ptr<Grid2DHelper> &data, int lod)
{
    return setData(-std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                   -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                   data, lod);
}

void Grid2D::gridCellAt(float   x, float   y, float   z,
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

int Grid2D::getEntityBoundingBox(float &x1, float &y1, float &z1,
                                 float &x2, float &y2, float &z2)
{

    x1 = -std::numeric_limits<float>::infinity();
    y1 = -std::numeric_limits<float>::infinity();
    z1 = -std::numeric_limits<float>::infinity();
    x2 = +std::numeric_limits<float>::infinity();
    y2 = +std::numeric_limits<float>::infinity();
    z2 = +std::numeric_limits<float>::infinity();

    return 0;
}

mapit::istream *Grid2D::startReadBytes(mapit::uint64_t start, mapit::uint64_t len)
{
    return m_streamProvider->startRead(start, len);
}

void Grid2D::endRead(mapit::istream *&strm)
{
    m_streamProvider->endRead(strm);
}

mapit::ostream *Grid2D::startWriteBytes(mapit::uint64_t start, mapit::uint64_t len)
{
    return m_streamProvider->startWrite(start, len);
}

void Grid2D::endWrite(mapit::ostream *&strm)
{
    m_streamProvider->endWrite(strm);
}

size_t Grid2D::size() const
{
    m_streamProvider->getStreamSize();
}

// Win32 does not like anything but void pointers handled between libraries
// For Unix there would be a hack to use a "custom deleter" which is given to the library to clean up the created memory
// the common denominator is to build pointer with custom deleter in our main programm and just exchange void pointers and call delete when we are done
//std::shared_ptr<mapit::AbstractEntitydata> createEntitydata(std::shared_ptr<AbstractEntitydataProvider> streamProvider)
//void* createEntitydata(std::shared_ptr<mapit::AbstractEntitydataProvider> streamProvider)
void deleteEntitydataPcd(mapit::AbstractEntitydata *ld)
{
    Grid2D *p = dynamic_cast<Grid2D*>(ld);
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
    //return std::shared_ptr<AbstractEntitydata>(new Grid2D( streamProvider ), deleteWrappedLayerData);
    *out = std::shared_ptr<mapit::AbstractEntitydata>(new Grid2D( streamProvider ), deleteEntitydataPcd);
}

