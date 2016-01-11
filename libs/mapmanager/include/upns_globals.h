#ifndef __UPNS_GLOBALS_H
#define __UPNS_GLOBALS_H

#ifdef USE_QT_STRUCTURES
#include <QVector>
#include <QString>
#include <QSharedPointer>
#include <QPair>
#else
#include <vector>
#include <string>
#include <iostream>
#ifdef USE_BOOST_STRUCTURES
#include <boost/shared_ptr.hpp>
#else
#include <memory>
#endif
#endif

#include <log4cplus/logger.h>
#ifdef UPNS_DEBUG
    #ifdef USE_QT_STRUCTURES
        #define log_error(msg) do{LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("main"), std::string() + msg); qassert_x(false, "upns", msg); break;}while(true)
    #else
        #define log_error(msg) do{LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("main"), std::string() + msg); assert(false); break;}while(true)
    #endif
#else // UPNS_DEBUG
    #define log_error(msg) LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("main"), std::string() + msg)
#endif // UPNS_DEBUG
#define log_warn(msg) LOG4CPLUS_WARN(log4cplus::Logger::getInstance("main"), std::string() + msg)
#define log_info(msg) LOG4CPLUS_INFO(log4cplus::Logger::getInstance("main"), std::string() + msg)

namespace upns
{

#ifdef USE_QT_STRUCTURES
using upnsuint64 = quint64;
using upnsuint32 = quint32;
using upnsString = QString;
using upnsReal = qreal;
using upnsIStream = std::istream;
using upnsOStream = std::ostream;
template <typename T>
using upnsVec = QVector<T>;
template<typename T>
using upnsSharedPointer = QSharedPointer<T>;
template <typename T1, typename T2>
using upnsPair = QPair<T1, T2>;
#else
using upnsuint64 = long long unsigned int;
using upnsuint32 = unsigned int;
using upnsString = std::string;
using upnsReal = float;
using upnsIStream = std::istream;
using upnsOStream = std::ostream;
template <typename T>
using upnsVec = std::vector<T>;

#ifdef USE_BOOST_STRUCTURES
template<typename T>
using upnsSharedPointer = boost::shared_ptr<T>;
template<class T, class U>
upnsSharedPointer<T> static_pointer_cast(upnsSharedPointer<U> const & r)
{
    return boost::static_pointer_cast<T>(r);
}
#else
template<typename T>
using upnsSharedPointer = std::shared_ptr<T>;
template<class T, class U>
upnsSharedPointer<T> static_pointer_cast(upnsSharedPointer<U> const & r)
{
    return std::static_pointer_cast<T>(r);
}
#endif
template <typename T1, typename T2>
using upnsPair = std::pair<T1, T2>;
#endif

using CommitId = upnsString;
using MapIdentifier = upnsuint64;
using LayerIdentifier = upnsuint64;
using EntityIdentifier = upnsuint64;

using LockHandle = upnsuint32;
using StatusCode = upnsuint32;

using CommitId = upnsString;
using ObjectId = upnsString;
using Path = upnsString;

#define InvalidCommitId "invalidCId"
#define InvalidObjectId "invalidOId"

using StatusPair = upnsPair<MapIdentifier, StatusCode>;
using MapResultsVector = upnsVec<StatusPair >;

class OperationDescription;
using OperationResult = upnsPair<const StatusCode, const OperationDescription>;
}

extern "C"
{

// keep in sync with .proto
//enum UpnsLayerType {
//  POINTCLOUD2 = 0,
//  OCTOMAP = 1,
//  OPENVDB = 2,
//  LAST_PREDEFINED = 3
//};

#define UPNS_MODULE_API_VERSION 1

}

#endif
