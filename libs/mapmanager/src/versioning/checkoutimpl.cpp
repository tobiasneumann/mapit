#include "checkoutimpl.h"
#include <dlfcn.h>
#include <string>
#include <algorithm>
#include <log4cplus/logger.h>
#include "module.h"
#include "operationenvironmentimpl.h"
#include "serialization/entitystreammanager.h"

namespace upns
{
typedef ModuleInfo* (*GetModuleInfo)();

CheckoutImpl::CheckoutImpl(AbstractMapSerializer *serializer, upnsSharedPointer<CheckoutObj> checkoutCommit)
    :m_serializer(serializer),
     m_branch( NULL )
{
//    if(m_serializer->isCheckout(checkoutCommit))
//    {
//        m_checkoutId = commitOrCheckoutId;
//    }
//    else if(m_serializer->isCommit(commitOrCheckoutId))
//    {
//        upnsSharedPointer<Commit> co(new Commit());
//        co->add_parentcommitids(commitOrCheckoutId);
//        m_checkoutId = m_serializer->createCheckoutCommit( co );
//    }
}

//CheckoutImpl::CheckoutImpl(AbstractMapSerializer *serializer, const upnsSharedPointer<Branch> &branch)
//    :m_serializer(serializer),
//      m_branch( branch )
//{
////    if(m_serializer->isCheckout(branch->commitid()))
////    {
////        m_checkoutId = branch->commitid();
////    }
////    else
////    {
////        upnsSharedPointer<Commit> co(new Commit());
////        if(m_serializer->isCommit(branch->commitid()))
////        {
////            co->add_parentcommitids(branch->commitid());
////        }
////        else
////        {
////            log_info("Initial Commit created, branch: " + branch->name());
////        }
////        m_checkoutId = m_serializer->createCheckoutCommit( co );
////    }
//}

CheckoutImpl::~CheckoutImpl()
{
}

bool CheckoutImpl::isInConflictMode()
{

}

upnsVec<upnsSharedPointer<Conflict> > CheckoutImpl::getPendingConflicts()
{

}

upnsSharedPointer<Tree> CheckoutImpl::getRoot()
{

}

upnsSharedPointer<Tree> CheckoutImpl::getTree(const Path &path)
{

}

upnsSharedPointer<Entity> CheckoutImpl::getEntity(const Path &path)
{

}

upnsSharedPointer<Tree> CheckoutImpl::getTreeConflict(const ObjectId &objectId)
{

}

upnsSharedPointer<Entity> CheckoutImpl::getEntityConflict(const ObjectId &objectId)
{

}

OperationResult CheckoutImpl::doOperation(const OperationDescription &desc)
{
    //TODO: This code my belong to a class which handles operation-modules. A "listOperations" might be needed outside of "checkout".
    OperationEnvironmentImpl env(desc);
    env.setCheckout( this );
#ifndef NDEBUG
    upnsString debug = DEBUG_POSTFIX;
#else
    upnsString debug = "";
#endif

#ifdef _WIN32
    upnsString prefix = "";
    upnsString postfix = ".dll";
#else
    upnsString prefix = "lib";
    upnsString postfix = ".so";
#endif
    std::stringstream filename;
    filename << "./operator_modules/" << desc.operatorname() << "/" << prefix << desc.operatorname() << debug << postfix;
    if(desc.operatorversion())
    {
        filename << "." << desc.operatorversion();
    }
    void* handle = dlopen(filename.str().c_str(), RTLD_NOW);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        return OperationResult(UPNS_STATUS_ERR_MODULE_OPERATOR_NOT_FOUND, OperationDescription());
    }
    GetModuleInfo getModInfo = (GetModuleInfo)dlsym(handle, "getModuleInfo");
    ModuleInfo* info = getModInfo();
    StatusCode result = info->operate( &env );
    if(!upnsIsOk(result))
    {
        std::stringstream strm;
        strm << "operator '" << desc.operatorname() << "' reported an error. (code:" << result << ")";
        log_error(strm.str());
    }
    return OperationResult(result, env.outputDescription());
}

upnsSharedPointer<AbstractEntityData> CheckoutImpl::getEntityDataReadOnly(const Path &path)
{
    ObjectId oid;
    return EntityStreamManager::getEntityDataImpl(m_serializer, oid, true, false);
}

upnsSharedPointer<AbstractEntityData> CheckoutImpl::getEntityDataReadOnlyConflict(const ObjectId &entityId)
{
    return EntityStreamManager::getEntityDataImpl(m_serializer, entityId, false, true);
}

upnsSharedPointer<AbstractEntityData> CheckoutImpl::getEntityDataForReadWrite(const Path &path)
{
    ObjectId oid;
    return EntityStreamManager::getEntityDataImpl(m_serializer, oid, true, true);
}

StatusCode CheckoutImpl::storeTree(const Path &path, upnsSharedPointer<Tree> tree)
{
    //return m_serializer->storeTree();
}

StatusCode CheckoutImpl::storeEntity(const Path &path, upnsSharedPointer<Entity> tree)
{

}

void CheckoutImpl::setConflictSolved(const Path &path, const ObjectId &oid)
{

}

}
