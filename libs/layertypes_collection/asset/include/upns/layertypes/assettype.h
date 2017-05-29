#ifndef ASSETTYPE_H
#define ASSETTYPE_H

#include <upns/entitydata.h>
#include <upns/operators/serialization/abstractentitydataprovider.h>
#include "tinyply.h"

using namespace upns;

// TODO: Is import case needed? (dynamic linking?)
#ifdef _WIN32
#define MODULE_EXPORT __declspec(dllexport)
#else
#define MODULE_EXPORT // empty
#endif

// Ply File is dump and does not know anything about our model except for the header
// Because of this, the stream has to be used
// The stream is closed by a custom deleter for this shared pointer.
typedef std::pair<tinyply::PlyFile, std::istream*> AssetDataPair;
typedef std::shared_ptr< AssetDataPair > AssetPtr;

extern "C"
{
MODULE_EXPORT void createEntitydata(std::shared_ptr<AbstractEntitydata> *out, std::shared_ptr<AbstractEntitydataProvider> streamProvider);
}

class AssetEntitydata : public Entitydata< AssetDataPair >
{
public:
    static const char* TYPENAME();

    AssetEntitydata(std::shared_ptr<AbstractEntitydataProvider> streamProvider);

    const char*         type() const;
    bool                hasFixedGrid() const;
    bool                canSaveRegions() const;
    AssetPtr  getData(upnsReal x1, upnsReal y1, upnsReal z1,
                                upnsReal x2, upnsReal y2, upnsReal z2,
                                bool clipMode,
                                int lod = 0);
    int                 setData(upnsReal x1, upnsReal y1, upnsReal z1,
                                upnsReal x2, upnsReal y2, upnsReal z2,
                                AssetPtr &data,
                                int lod = 0);

    AssetPtr  getData(int lod = 0);
    //TODO: doesn't fit because of stream pair here. second = nullptr. Use AbstractEntitydata directly
    int                 setData(AssetPtr &data, int lod = 0);

    void gridCellAt(upnsReal   x, upnsReal   y, upnsReal   z,
                    upnsReal &x1, upnsReal &y1, upnsReal &z1,
                    upnsReal &x2, upnsReal &y2, upnsReal &z2) const;

    int getEntityBoundingBox(upnsReal &x1, upnsReal &y1, upnsReal &z1,
                             upnsReal &x2, upnsReal &y2, upnsReal &z2);

    upnsIStream *startReadBytes(upnsuint64 start, upnsuint64 len);
    void endRead(upnsIStream *&strm);

    upnsOStream *startWriteBytes(upnsuint64 start, upnsuint64 len);
    void endWrite(upnsOStream *&strm);

    size_t size() const;
private:
    std::shared_ptr<AbstractEntitydataProvider> m_streamProvider;
    AssetPtr m_asset;
};
#endif
