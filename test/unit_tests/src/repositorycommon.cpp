#include "repositorycommon.h"
#include "repositoryserver.h"
#include "versioning/repositoryfactory.h"
#include "versioning/repositorynetworkingfactory.h"
#include "versioning/checkout.h"
#include <QThread>
#include "serverthread.h"

Q_DECLARE_METATYPE(upns::upnsSharedPointer<upns::Repository>)
Q_DECLARE_METATYPE(upns::upnsSharedPointer<upns::Checkout>)
Q_DECLARE_METATYPE(std::function<void()>)

void RepositoryCommon::createTestdata()
{
    QTest::addColumn< upns::upnsSharedPointer<upns::Repository> >("repo");
    QTest::addColumn< upns::upnsSharedPointer<upns::Checkout> >("checkout");
    QTest::addColumn< std::function<void()> >("startServer");
    QTest::addColumn< std::function<void()> >("stopServer");

    QTest::newRow("local filesystem") << m_repo[0] << m_checkout[0] << std::function<void()>([](){}) << std::function<void()>([](){});
    QTest::newRow("local database")   << m_repo[1] << m_checkout[1] << std::function<void()>([](){}) << std::function<void()>([](){});
    QTest::newRow("remote")           << m_repo[2] << m_checkout[2] << std::function<void()>([this]()
        {
            m_serverThread->start();
        })
        << std::function<void()>([this]()
        {
            m_serverThread->stop();
        });
}

void RepositoryCommon::initTestdata()
{
    {
        //// Setup Repository in Filesystem
        const char* fileSystemName = ".mapit";
        QDir dir(fileSystemName);
        if(dir.exists())
        {
            bool result = dir.removeRecursively();
            QVERIFY( result );
        }
        YAML::Node conf;
        YAML::Node mapsource;
        mapsource["name"] = "FileSystem";
        mapsource["filename"] = fileSystemName;//databaseName;
        conf["mapsource"] = mapsource;

        m_repo[0] = upns::upnsSharedPointer<upns::Repository>(upns::RepositoryFactory::openLocalRepository(conf));
        m_checkout[0] = upns::upnsSharedPointer<upns::Checkout>(m_repo[0]->createCheckout("master", "testcheckout"));
    }
    {
        //// Setup Repository as Database
        const char* databaseName = "test.db";
        QDir dir(databaseName);
        if(dir.exists())
        {
            bool result = dir.removeRecursively();
            QVERIFY( result );
        }
        YAML::Node conf;
        YAML::Node mapsource;
        mapsource["name"] = "leveldb";
        mapsource["filename"] = databaseName;
        conf["mapsource"] = mapsource;

        m_repo[1] = upns::upnsSharedPointer<upns::Repository>(upns::RepositoryFactory::openLocalRepository(conf));
        m_checkout[1] = upns::upnsSharedPointer<upns::Checkout>(m_repo[1]->createCheckout("master", "testcheckout"));
    }
    {
        //// Setup Repository as Network connection
        const char* databaseName = "test.db";
        QDir dir(databaseName);
        if(dir.exists())
        {
            bool result = dir.removeRecursively();
            QVERIFY( result );
        }
        YAML::Node conf;
        YAML::Node mapsource;
        mapsource["name"] = "leveldb";
        mapsource["filename"] = databaseName;
        conf["mapsource"] = mapsource;

        // get is okay here, m_srv and m_repo[1] have same lifecycle. Don't copy/paste this.
        upns::upnsSharedPointer<upns::RepositoryServer> srv = upns::upnsSharedPointer<upns::RepositoryServer>(upns::RepositoryNetworkingFactory::openRepositoryAsServer(5555, m_repo[1].get()));
        m_repo[2] = upns::upnsSharedPointer<upns::Repository>(upns::RepositoryNetworkingFactory::connectToRemoteRepository("tcp://localhost:5555", NULL));
        m_serverThread = QSharedPointer<ServerThread>(new ServerThread(srv));
        m_serverThread->start();
        m_checkout[2] = upns::upnsSharedPointer<upns::Checkout>(m_repo[2]->createCheckout("master", "testcheckout"));
        m_serverThread->stop();
    }
}

void RepositoryCommon::startServer()
{
    QFETCH(std::function<void()>, startServer);
    startServer();
}

void RepositoryCommon::stopServer()
{
    QFETCH(std::function<void()>, stopServer);
    stopServer();
}
