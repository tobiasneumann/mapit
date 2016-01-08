#ifndef __OPERATIONENVIRONMENTIMPL_H
#define __OPERATIONENVIRONMENTIMPL_H
#include "operationenvironment.h"
#include "../upns_interface/services.pb.h"

namespace upns
{
class CheckoutRaw;

class OperationEnvironmentImpl : public OperationEnvironment
{
public:
    OperationEnvironmentImpl(const OperationDescription& desc);
    void setCheckout(CheckoutRaw *checkout);

    // OperationEnvironment Interface
    virtual CheckoutRaw *getCheckout() const;
    virtual const OperationDescription *getDescription() const;
    virtual const OperationParameter *getParameter(const std::string &key) const;
    virtual void setOutputDescription(OperationDescription);
    virtual const OperationDescription& outputDescription() const;

private:
    CheckoutRaw *m_checkout;
    const OperationDescription m_operationDesc;
    OperationDescription m_outDesc;
};

}

#endif
