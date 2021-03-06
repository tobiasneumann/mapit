/*******************************************************************************
 *
 * Copyright      2017 Daniel Bulla	<d.bulla@fh-aachen.de>
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

#ifndef ENTITYDATALOADER_H
#define ENTITYDATALOADER_H

#include <QThread>
#include <QtCore/QJsonObject>
#include <QVariant>
#include <memory>

namespace mapit {
    class Workspace;
}
class EntitydataLoader : public QThread
{
    Q_OBJECT
    void run();
public:
    //EntitydataLoader(QObject *parent, QString repository, QString workspaceName, QString path );
    // This constructor requires mapit::CHeckout to be thread-safe
    EntitydataLoader(QObject *parent, std::shared_ptr<mapit::Workspace> workspace, QString path );
    ~EntitydataLoader();

Q_SIGNALS:
    void entityInfoLoaded(QJsonObject result);

private:
    QString m_repository;
    QString m_workspacename;
    QString m_path;
    std::shared_ptr<mapit::Workspace> m_workspace;
};

#endif
