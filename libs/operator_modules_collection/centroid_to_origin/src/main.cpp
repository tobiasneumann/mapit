#include "module.h"
#include "libs/layertypes_collection/pointcloud2/include/pointcloudlayer.h"
#include "modules/versioning/checkoutraw.h"
#include "modules/operationenvironment.h"
#include <iostream>
#include <pcl/common/centroid.h>
#include <pcl/common/transforms.h>
#include <pcl/common/common.h>
#include <memory>
#include "upns_errorcodes.h"
#include "modules/versioning/checkoutraw.h"
#include <QJsonDocument>
#include <QJsonObject>

upns::StatusCode operate_ctr(upns::OperationEnvironment* env)
{
    QJsonDocument paramsDoc = QJsonDocument::fromJson( QByteArray(env->getParameters().c_str(), env->getParameters().length()) );
    QJsonObject params(paramsDoc.object());

    std::string target = params["target"].toString().toStdString();

    upnsSharedPointer<AbstractEntitydata> abstractEntitydata = env->getCheckout()->getEntitydataForReadWrite( target );
    upnsSharedPointer<PointcloudEntitydata> entityData = upns::static_pointer_cast<PointcloudEntitydata>( abstractEntitydata );
    upnsPointcloud2Ptr pc2 = entityData->getData();
    pcl::PointCloud<pcl::PointXYZ> pc; // TODO: make generic
    pcl::fromPCLPointCloud2(*pc2, pc);
    Eigen::Vector4f centroid;
//    pcl::compute3DCentroid(pc, centroid);
//    log_warn("DBG, centroid ctr: " + std::to_string(centroid[0]) + ", " + std::to_string(centroid[1]) + ", " + std::to_string(centroid[2]));

    pcl::PointXYZ min;
    pcl::PointXYZ max;
    pcl::getMinMax3D(pc, min, max);
    Eigen::Vector4f ctr((min.x+max.x)/2.0f, (min.y+max.y)/2.0f, (min.z+max.z)/2.0f, 1.0f);
    //pcl::PointXYZ ctr((min.x+max.x)/2.0f, (min.y+max.y)/2.0f, (min.z+max.z)/2.0f);
    pcl::PointCloud<pcl::PointXYZ> pcCtr; // TODO: make generic
    pcl::demeanPointCloud(pc, ctr, pcCtr);

//    //TODO: Bounding box! dann diese zentrieren. Ausreißer löschen
////    Eigen::Vector3f ctr = centroid.head<3>() * -1.0f;
////    Eigen::Quaternionf q;
//    //pcl::transformPointCloud(pc, pcCtr, ctr, q);
////    centroid *= -1.0f;
//    pcl::demeanPointCloud(pc, ctr, pcCtr);
    pcl::compute3DCentroid(pcCtr, centroid);
    log_warn("DBG, works" + " ctr: " + std::to_string(centroid[0]) + ", " + std::to_string(centroid[1]) + ", " + std::to_string(centroid[2]));

    pcl::demeanPointCloud(pcCtr, centroid, pc);
    pcl::compute3DCentroid(pc, centroid);
    log_warn("DBG, works2" + " ctr: " + std::to_string(centroid[0]) + ", " + std::to_string(centroid[1]) + ", " + std::to_string(centroid[2]));

//    pcl::demeanPointCloud(pc, centroid, pcCtr);
//    pcl::compute3DCentroid(pcCtr, centroid);
//    log_warn("DBG, works3" + " ctr: " + std::to_string(centroid[0]) + ", " + std::to_string(centroid[1]) + ", " + std::to_string(centroid[2]));

    upnsPointcloud2Ptr pc2Out(new pcl::PCLPointCloud2());
    pcl::toPCLPointCloud2( pc, *pc2Out);

    entityData->setData(pc2Out);
    OperationDescription out;
    out.set_operatorname(OPERATOR_NAME);
    out.set_operatorversion(OPERATOR_VERSION);
//    OperationParameter *outTarget = out.add_params();
//    outTarget->set_key("target");
////    outTarget->set_mapval( map->id() );
////    outTarget->set_layerval( layer->id() );
////    outTarget->set_entityval( entity->id() );
//    OperationParameter *outMapname = out.add_params();
//    outMapname->set_key("leafsize");
//    outMapname->set_realval( leafSize );
    env->setOutputDescription( out.SerializeAsString() );
    return UPNS_STATUS_OK;
}

UPNS_MODULE(OPERATOR_NAME, "use point of mass as origin of the pointcloud", "fhac", OPERATOR_VERSION, upns::LayerType::POINTCLOUD, &operate_ctr)
