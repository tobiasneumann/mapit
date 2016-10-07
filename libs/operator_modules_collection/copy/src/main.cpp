#include "module.h"
#include "modules/versioning/checkoutraw.h"
#include "modules/operationenvironment.h"
#include "error.h"
#include "modules/versioning/checkoutraw.h"
#include <QJsonDocument>
#include <QJsonObject>

upns::StatusCode operate(upns::OperationEnvironment* env)
{
    QJsonDocument paramsDoc = QJsonDocument::fromJson( QByteArray(env->getParameters().c_str(), env->getParameters().length()) );
    QJsonObject params(paramsDoc.object());

    std::string source = params["source"].toString().toStdString();
    std::string target = params["target"].toString().toStdString();

    if(source.empty())
    {
        log_error("could not copy, source is not set");
    }
    if(target.empty())
    {
        log_error("could not copy, target is not set");
    }

    upns::upnsSharedPointer< upns::Entity > srcEnt( env->getCheckout()->getEntity(source) );
    upns::StatusCode s;
    if(srcEnt)
    {
        // Note: Only the entity is copied, it's new data is empty (because transient path is used to identify entities data)
        s = env->getCheckout()->storeEntity(target, srcEnt);

        if(!upnsIsOk(s))
        {
            log_error("Could not copy \"" + source + "\" to \"" + target + "\"");
        }
        else
        {
            upns::upnsSharedPointer<upns::AbstractEntityData> aedSource = env->getCheckout()->getEntitydataReadOnly(source);
            upns::upnsSharedPointer<upns::AbstractEntityData> aedTarget = env->getCheckout()->getEntityDataForReadWrite(target);
            upns::upnsIStream *is = aedSource->startReadBytes();
            upns::upnsOStream *os = aedTarget->startWriteBytes();

            os->seekp(std::ios::beg);
            char buffer[1024];
            while(is->read(buffer, 1024)) {
                std::streamsize size=is->gcount();
                os->write(buffer, size);
            }
            std::streamsize size=is->gcount();
            if(size > 0)
            {
                os->write(buffer, size);
            }
            aedTarget->endWrite(os);
            aedSource->endRead(is);
        }
    }
    else
    {
        upns::upnsSharedPointer< upns::Tree > srcTree( env->getCheckout()->getTree(source) );
        if(srcTree)
        {
            s = env->getCheckout()->storeTree(target, srcTree);
        }
        else
        {
            s = UPNS_STATUS_ERR_DB_NOT_FOUND;
            log_error("Path not found " + source);
        }
    }
    if(!upnsIsOk(s))
    {
        log_error("Could not copy \"" + source + "\" to \"" + target + "\"");
    }
    else
    {
        log_info("copied \"" + source + "\" to \"" + target + "\"");
    }

    upns::OperationDescription out;
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

UPNS_MODULE(OPERATOR_NAME, "copy tree or entity", "fhac", OPERATOR_VERSION, upns::LayerType::POINTCLOUD2, &operate)
