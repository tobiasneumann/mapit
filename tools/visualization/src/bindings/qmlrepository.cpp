#include "upns/ui/bindings/qmlrepository.h"
#include <upns/versioning/repositoryfactory.h>
#include <upns/errorcodes.h>
#include <upns/serialization/operatorlibrarymanager.h>

QmlRepository::QmlRepository(std::shared_ptr<upns::Repository> repo)
    :QmlRepository(repo, nullptr)
{
}

QmlRepository::QmlRepository(std::shared_ptr<upns::Repository> repo, QObject *parent)
    :QObject( parent )
{
    m_repository = repo;
    m_checkoutNames.clear();
    std::vector<std::string> coNames(m_repository->listCheckoutNames());
    for(std::vector<std::string>::const_iterator iter(coNames.cbegin()) ; iter != coNames.cend() ; iter++)
    {
        m_checkoutNames.append(QString::fromStdString(*iter));
    }
    Q_EMIT checkoutNamesChanged( m_checkoutNames );
    Q_EMIT internalRepositoryChanged( this );
}

QStringList QmlRepository::listCheckoutNames() const
{
    return checkoutNames();
}

QmlTree *QmlRepository::getTree(QString oid)
{
    if(!m_repository) return nullptr;
    std::string o = oid.toStdString();
    std::shared_ptr<mapit::msgs::Tree> obj( m_repository->getTree( o ) );
    if(!obj) return nullptr;
    return new QmlTree( obj );
}

QmlEntity *QmlRepository::getEntity(QString oid)
{
    if(!m_repository) return nullptr;
    std::string o = oid.toStdString();
    std::shared_ptr<mapit::msgs::Entity> obj( m_repository->getEntity( o ) );
    if(!obj) return nullptr;
    return new QmlEntity( obj );
}

QmlCommit *QmlRepository::getCommit(QString oid)
{
    if(!m_repository) return nullptr;
    std::string o = oid.toStdString();
    std::shared_ptr<mapit::msgs::Commit> obj( m_repository->getCommit( o ) );
    if(!obj) return nullptr;
    return new QmlCommit( obj );
}

QmlBranch *QmlRepository::getBranch(QString name)
{
    if(!m_repository) return nullptr;
    std::string o = name.toStdString();
    std::shared_ptr<mapit::msgs::Branch> obj( m_repository->getBranch( o ) );
    if(!obj) return nullptr;
    return new QmlBranch( obj );
}

QString QmlRepository::typeOfObject(QString oid)
{
    if(!m_repository) return "";
    std::string o(oid.toStdString());
    mapit::msgs::MessageType mt = m_repository->typeOfObject(o);
    switch(mt)
    {
    case mapit::msgs::MessageTree:
        return"MessageTree";
    case mapit::msgs::MessageEntity:
        return"MessageEntity";
    case mapit::msgs::MessageCommit:
        return"MessageCommit";
    case mapit::msgs::MessageCheckout:
        return"MessageCheckout";
    case mapit::msgs::MessageBranch:
        return"MessageBranch";
    case mapit::msgs::MessageEntitydata:
        return"MessageEntitydata";
    case mapit::msgs::MessageEmpty:
        return"MessageEmpty";
    default:
        return"";
    }
}

QmlEntitydata *QmlRepository::getEntitydataReadOnly(QString oid)
{
    if(!m_repository) return nullptr;
    std::string o = oid.toStdString();
    std::shared_ptr<upns::AbstractEntitydata> obj( m_repository->getEntitydataReadOnly( o ) );
    if(!obj) return nullptr;
    return new QmlEntitydata( obj, NULL );
}

QList<QVariant> QmlRepository::listOperators()
{
    QList<QVariant> result;
    std::vector<upns::OperatorInfo> moduleInfos = upns::OperatorLibraryManager::listOperators();
    for(auto iter = moduleInfos.cbegin(); iter != moduleInfos.cend() ; ++iter)
    {
        bool any = false;
        for(auto iter2=result.cbegin() ; iter2 != result.cend() ; ++iter2)
        {
            if(iter2->toJsonObject()["moduleName"].toString() == QString::fromStdString(iter->moduleName))
            {
                any = true;
                break;
            }
        }
        if(any) continue;

        QJsonObject obj;
        obj["compiler"] = QJsonValue(iter->compiler.c_str());
        obj["compilerConfig"] = QJsonValue(iter->compilerConfig.c_str());
        obj["date"] = QJsonValue(iter->date.c_str());
        obj["time"] = QJsonValue(iter->time.c_str());
        obj["moduleName"] = QJsonValue(iter->moduleName.c_str());
        obj["description"] = QJsonValue(iter->description.c_str());
        obj["author"] = QJsonValue(iter->author.c_str());
        obj["moduleVersion"] = iter->moduleVersion;
        obj["apiVersion"] = iter->apiVersion;
        obj["layerType"] = QJsonValue(iter->layerType.c_str());
        result.append(obj);
    }
    return result;
}

QmlCheckout *QmlRepository::createCheckout(QString commitIdOrBranchname, QString name)
{
    if(!m_repository) return nullptr;
    std::string o = commitIdOrBranchname.toStdString(),
                     n = name.toStdString();
    std::shared_ptr<upns::Checkout> obj( m_repository->createCheckout( o, n ) );
    if(!obj) return nullptr;
    m_checkoutNames.append( name );
    Q_EMIT checkoutNamesChanged(m_checkoutNames);
    return new QmlCheckout( obj, this, name );
}

QmlCheckout *QmlRepository::getCheckout(QString checkoutName)
{
    if(!m_repository) return nullptr;
    std::string o = checkoutName.toStdString();
    std::shared_ptr<upns::Checkout> obj( m_repository->getCheckout( o ) );
    if(!obj) return nullptr;
    return new QmlCheckout( obj, this, checkoutName );
}

bool QmlRepository::deleteCheckoutForced(QString checkoutName)
{
    if(!m_repository) return false;
    std::string o = checkoutName.toStdString();
    upns::StatusCode s = m_repository->deleteCheckoutForced( o );
    return upnsIsOk( s );
}

QString QmlRepository::commit(QmlCheckout *checkout, QString msg)
{
    if(!m_repository) return nullptr;
    std::string m = msg.toStdString();
    upns::CommitId cid = m_repository->commit( checkout->getCheckoutObj(), m );
    return QString::fromStdString(cid);
}

QList<QmlBranch *> QmlRepository::getBranches()
{
    if(!m_repository) return QList<QmlBranch *>();
    std::vector<std::shared_ptr<mapit::msgs::Branch> > b(m_repository->getBranches());
    QList<QmlBranch *> ret;
    for(std::vector<std::shared_ptr<mapit::msgs::Branch> >::const_iterator iter(b.cbegin());
        iter != b.cend();
        ++iter)
    {
        std::shared_ptr<mapit::msgs::Branch> p(*iter);
        ret.push_back(new QmlBranch( p ));
    }
    return ret;
}

QString QmlRepository::push(QmlRepository *repo)
{
    return "not yet implemented";
}

QString QmlRepository::pull(QmlRepository *repo)
{
    return "not yet implemented";
}

QString QmlRepository::parseCommitRef(QString commitRef)
{
    if(!m_repository) return nullptr;
    std::string cr = commitRef.toStdString();
    return QString::fromStdString( m_repository->parseCommitRef( cr ) );
}

QmlCheckout *QmlRepository::merge(QString mine, QString theirs, QString base)
{
    if(!m_repository) return nullptr;
    std::string m = mine.toStdString();
    std::string t = theirs.toStdString();
    std::string b = base.toStdString();
    std::shared_ptr<upns::Checkout> co(m_repository->merge( m, t, b ));
    return new QmlCheckout( co, this );
}

QMap<QString, QString> QmlRepository::ancestors(QString commitId, QString objectId, qint32 level)
{
    //TODO: nyi
    return QMap<QString, QString>();
}

bool QmlRepository::canRead()
{
    return true;
}

bool QmlRepository::canWrite()
{
    return true;
}

std::shared_ptr<upns::Repository> QmlRepository::getRepository()
{
    return m_repository;
}
