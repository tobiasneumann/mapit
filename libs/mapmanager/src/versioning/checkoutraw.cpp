#include "checkoutraw.h"
#include "upns.h"

namespace upns
{

StatusCode CheckoutRaw::storeTree(ObjectId previous, Tree tree)
{
}

StatusCode CheckoutRaw::createTree(ObjectId parent, Tree tree)
{
}

upnsSharedPointer<Entity> CheckoutRaw::createEntity(ObjectId parent)
{
}

void CheckoutRaw::setConflictSolved(ObjectId solved)
{

}

//upnsSharedPointer<AbstractEntityData> CheckoutRaw::getEntityData(const ObjectId &entityId, bool readOnly)
//{
//    return CheckoutImpl::getEntityDataImpl(entityId, readOnly);
//}

}
