/*******************************************************************************
 *
 * Copyright      2015 Daniel Bulla	<d.bulla@fh-aachen.de>
 *                2015 Tobias Neumann	<t.neumann@fh-aachen.de>
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

#include "testgrid2d.h"

#include "../../src/autotest.h"

#include <mapit/errorcodes.h>
#include <mapit/versioning/repositoryfactory.h>
#include <mapit/versioning/workspace.h>
#include <mapit/operators/versioning/workspacewritable.h>
#include <mapit/operators/operationenvironment.h>
#include <iostream>


Q_DECLARE_METATYPE(std::shared_ptr<mapit::Repository>)
Q_DECLARE_METATYPE(std::shared_ptr<mapit::Workspace>)
Q_DECLARE_METATYPE(std::function<void()>)

void TestGrid2D::init()
{
    startServer();
}

void TestGrid2D::cleanup()
{
    stopServer();
}


void TestGrid2D::initTestCase()
{
    initTestdata();
}

void TestGrid2D::cleanupTestCase()
{
    cleanupTestdata();
}

//void TestGrid2D::testGetData() {
//    QVERIFY(true);
//}

//void TestGrid2D::testSetData() {
//    QVERIFY(true);
//}

//void TestGrid2D::testGetData2() {
//    QVERIFY(true);
//}

void TestGrid2D::testGetData()
{

    QFETCH(std::shared_ptr<mapit::Repository>, repo);
    QFETCH(std::shared_ptr<mapit::Workspace>, workspace);

    std::string epath("/hello/test/gridentity");
    mapit::msgs::Grid2D grid;

    // test data
    float resolution = 100;
    float width = 2;
    float height = 3;
    // Position of origin, create quarernation & vector
    mapit::msgs::Quaternion origin_rot;
    origin_rot.set_w(1);
    origin_rot.set_x(2);
    origin_rot.set_y(3);
    origin_rot.set_z(4);

    mapit::msgs::Vector origin_trans;
    origin_trans.set_x(5);
    origin_trans.set_y(6);
    origin_trans.set_z(7);


    mapit::msgs::Pose origin;

    origin.set_allocated_rotation(&origin_rot);
    origin.set_allocated_translation(&origin_trans);
    // data as bytes but converted to string in c++, so how does the string look like?
    //bytes data = 5;

    grid.set_resolution(resolution);
    grid.set_width(width);
    grid.set_height(height);
    grid.set_allocated_origin(&origin);
    std::string data = "12345";
    grid.set_allocated_data(&data);

    std::cout << "Grid at creation, rot(w) is: " << grid.origin().rotation().w() << "\n";
//    origin.release_rotation();
//    origin.release_translation();
//    grid.release_origin();
//    grid.release_data();


    std::cout << "Grid at after release, rot(w) is: " << grid.origin().rotation().w() << "\n";


    // create operation
    OperationDescription desc;
    desc.mutable_operator_()->set_operatorname("myUntraceable");
    desc.set_params("{\"source:\":\"testInlineOperator\"}");
    QVERIFY(repo     != nullptr);
    QVERIFY(workspace != nullptr);
    mapit::OperationResult res = workspace->doUntraceableOperation(desc
                                                                   , [&grid, &epath](mapit::OperationEnvironment* env)
    {
        mapit::operators::WorkspaceWritable *coraw = env->getWorkspace();
        std::shared_ptr<Entity> e(new Entity);
        e->set_type(mapit::entitytypes::Grid2D::TYPENAME());
        mapit::StatusCode status = coraw->storeEntity(epath, e);
        if(!mapitIsOk(status))
        {
            return MAPIT_STATUS_ERROR;
        }
        std::shared_ptr<mapit::AbstractEntitydata> abstractEntitydata = coraw->getEntitydataForReadWrite( epath );
        std::shared_ptr<mapit::entitytypes::Grid2D> entityData = std::dynamic_pointer_cast<mapit::entitytypes::Grid2D>( abstractEntitydata );
        if(entityData == nullptr)
        {
            return MAPIT_STATUS_ERROR;
        }
        std::cout << "Grid input, rot(w) is: " << grid.origin().rotation().w() << "\n";

        //copy from scan, then setData
        std::shared_ptr<mapit::msgs::Grid2D> grid2d = std::make_shared<mapit::msgs::Grid2D>();
        grid2d->CopyFrom(grid);
        std::cout << "Grid before copy, rot(w) is: " << grid2d->origin().rotation().w() << "\n";
        std::shared_ptr<mapit::entitytypes::Grid2DHelper> help = std::make_shared<mapit::entitytypes::Grid2DHelper>();
        help->setGrid(*grid2d);
        entityData->setData(help);
        return MAPIT_STATUS_OK;
    });
    QVERIFY(mapitIsOk(res.first));

    float epsilon = 0.000001;

    std::shared_ptr<mapit::AbstractEntitydata> abstractentitydataByPath = workspace->getEntitydataReadOnly(epath);
    std::shared_ptr<mapit::entitytypes::Grid2D> entityData = std::dynamic_pointer_cast<mapit::entitytypes::Grid2D>( abstractentitydataByPath );
    QVERIFY( entityData != nullptr );
    std::shared_ptr<mapit::entitytypes::Grid2DHelper> helpOut = entityData->getData(0);
    mapit::msgs::Grid2D gridFromWS = helpOut->getGrid();
    QVERIFY(std::abs(gridFromWS.resolution() - resolution) < epsilon);
    QVERIFY(std::abs(gridFromWS.width() - width)< epsilon);
    QVERIFY(std::abs(gridFromWS.height() - height) < epsilon);
    mapit::msgs::Pose poseFromWS = gridFromWS.origin();
    //QVERIFY( poseFromWS != nullptr );
    mapit::msgs::Quaternion rotationFromWS = poseFromWS.rotation();
    mapit::msgs::Vector translationFromWS = poseFromWS.translation();
    //QVERIFY( rotationFromWS != nullptr );
   // QVERIFY( translationFromWS != nullptr );
    std::cout << "WS rot(w): " << rotationFromWS.x() << "\n";
    std::cout << "OV rot(w): " << origin_rot.w() << "\n";
    QVERIFY(std::abs(rotationFromWS.w() - origin_rot.w()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.x() - origin_rot.x()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.y() - origin_rot.y()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.z() - origin_rot.z()) < epsilon);
    QVERIFY(std::abs(translationFromWS.x() - origin_trans.x()) < epsilon);
    QVERIFY(std::abs(translationFromWS.y() - origin_trans.y()) < epsilon);
    QVERIFY(std::abs(translationFromWS.z() - origin_trans.z()) < epsilon);

    // release objects for memory integrity, seems like protobuf does not take care of that
    // altough documentation says it should
    origin.release_rotation();
    origin.release_translation();
    grid.release_origin();
    grid.release_data();
}

void TestGrid2D::testGetData2()
{

    QFETCH(std::shared_ptr<mapit::Repository>, repo);
    QFETCH(std::shared_ptr<mapit::Workspace>, workspace);

    std::string epath("/hello/test/gridentity");
    Grid2D grid;

    // test data
    float resolution = 100;
    float width = 2;
    float height = 3;
    // Position of origin, create quarernation & vector
    mapit::msgs::Quaternion origin_rot;
    origin_rot.set_w(1);
    origin_rot.set_x(2);
    origin_rot.set_y(3);
    origin_rot.set_z(4);

    mapit::msgs::Vector origin_trans;
    origin_trans.set_x(5);
    origin_trans.set_y(6);
    origin_trans.set_z(7);


    mapit::msgs::Pose origin;

    origin.set_allocated_rotation(&origin_rot);
    origin.set_allocated_translation(&origin_trans);
    // data as bytes but converted to string in c++, so how does the string look like?
    //bytes data = 5;

    grid.set_resolution(resolution);
    grid.set_width(width);
    grid.set_height(height);
    grid.set_allocated_origin(&origin);
    signed char ch1 = -1;
    signed char ch2 = 100;
    signed char ch3 = 15;
    std::string data;
    data += ch1;
    data += ch2;
    data += ch3;
    //std::string data = "12345";
    grid.set_allocated_data(&data);

    // create operation
    OperationDescription desc;
    desc.mutable_operator_()->set_operatorname("myUntraceable");
    desc.set_params("{\"source:\":\"testInlineOperator\"}");
    QVERIFY(repo     != nullptr);
    QVERIFY(workspace != nullptr);
    mapit::OperationResult res = workspace->doUntraceableOperation(desc
                                                                   , [&grid, &epath](mapit::OperationEnvironment* env)
    {
        mapit::operators::WorkspaceWritable *coraw = env->getWorkspace();
        std::shared_ptr<Entity> e(new Entity);
        e->set_type(mapit::entitytypes::Grid2D::TYPENAME());
        mapit::StatusCode status = coraw->storeEntity(epath, e);
        if(!mapitIsOk(status))
        {
            return MAPIT_STATUS_ERROR;
        }
        std::shared_ptr<mapit::AbstractEntitydata> abstractEntitydata = coraw->getEntitydataForReadWrite( epath );
        std::shared_ptr<mapit::entitytypes::Grid2D> entityData = std::dynamic_pointer_cast<mapit::entitytypes::Grid2D>( abstractEntitydata );
        if(entityData == nullptr)
        {
            return MAPIT_STATUS_ERROR;
        }
        std::cout << "Grid input, rot(w) is: " << grid.origin().rotation().w() << "\n";

        //copy from scan, then setData
        std::shared_ptr<mapit::msgs::Grid2D> grid2d(new mapit::msgs::Grid2D);
        grid2d->CopyFrom(grid);
        std::cout << "Grid before copy, rot(w) is: " << grid2d->origin().rotation().w() << "\n";
        std::shared_ptr<mapit::entitytypes::Grid2DHelper> help = std::make_shared<mapit::entitytypes::Grid2DHelper>();
        help->setGrid(*grid2d);
        entityData->setData(help);
        return MAPIT_STATUS_OK;
    });
    QVERIFY(mapitIsOk(res.first));

    float epsilon = 0.000001;

    std::shared_ptr<mapit::AbstractEntitydata> abstractentitydataByPath = workspace->getEntitydataReadOnly(epath);
    std::shared_ptr<mapit::entitytypes::Grid2D> entityData = std::dynamic_pointer_cast<mapit::entitytypes::Grid2D>( abstractentitydataByPath );
    QVERIFY( entityData != nullptr );
    std::shared_ptr<mapit::entitytypes::Grid2DHelper> helpOut = entityData->getData(0);
    mapit::msgs::Grid2D gridFromWS = helpOut->getGrid();
    QVERIFY(std::abs(gridFromWS.resolution() - resolution) < epsilon);
    QVERIFY(std::abs(gridFromWS.width() - width) < epsilon);
    QVERIFY(std::abs(gridFromWS.height() - height) < epsilon);
    mapit::msgs::Pose poseFromWS = gridFromWS.origin();
    //QVERIFY( poseFromWS != nullptr );
    mapit::msgs::Quaternion rotationFromWS = poseFromWS.rotation();
    mapit::msgs::Vector translationFromWS = poseFromWS.translation();
    //QVERIFY( rotationFromWS != nullptr );
   // QVERIFY( translationFromWS != nullptr );
    std::cout << "WS rot(w): " << rotationFromWS.x() << "\n";
    std::cout << "OV rot(w): " << origin_rot.w() << "\n";
    QVERIFY(std::abs(rotationFromWS.w() - origin_rot.w()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.x() - origin_rot.x()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.y() - origin_rot.y()) < epsilon);
    QVERIFY(std::abs(rotationFromWS.z() - origin_rot.z()) < epsilon);
    QVERIFY(std::abs(translationFromWS.x() - origin_trans.x()) < epsilon);
    QVERIFY(std::abs(translationFromWS.y() - origin_trans.y()) < epsilon);
    QVERIFY(std::abs(translationFromWS.z() - origin_trans.z()) < epsilon);

    std::string dataFromWS = gridFromWS.data();


    int n = dataFromWS.length();
    char char_array[n+1];
    strcpy(char_array, dataFromWS.c_str());

    std::cout << "Printing chars as integer: ";
    for (int i=0; i<n; i++)
      std::cout << static_cast<signed char>(char_array[i]);
    std::cout << "\n";

    QVERIFY(static_cast<int>(char_array[0]) == static_cast<int>(ch1));
    QVERIFY(static_cast<int>(char_array[1]) == static_cast<int>(ch2));
    QVERIFY(static_cast<int>(char_array[2]) == static_cast<int>(ch3));

    // release objects for memory integrity, seems like protobuf does not take care of that
    // altough documentation says it should
    origin.release_rotation();
    origin.release_translation();
    grid.release_origin();
    grid.release_data();
}

void TestGrid2D::testSetData()
{

    QFETCH(std::shared_ptr<mapit::Repository>, repo);
    QFETCH(std::shared_ptr<mapit::Workspace>, workspace);

    std::string epath("/hello/test/gridentity");
    Grid2D grid;

    // test data
    float resolution = 100;
    float width = 2;
    float height = 3;
    // Position of origin, create quarernation & vector
    mapit::msgs::Quaternion origin_rot;
    origin_rot.set_w(1);
    origin_rot.set_x(2);
    origin_rot.set_y(3);
    origin_rot.set_z(4);

    mapit::msgs::Vector origin_trans;
    origin_trans.set_x(5);
    origin_trans.set_y(6);
    origin_trans.set_z(7);


    mapit::msgs::Pose origin;

    origin.set_allocated_rotation(&origin_rot);
    origin.set_allocated_translation(&origin_trans);
    // data as bytes but converted to string in c++, so how does the string look like?
    //bytes data = 5;

    grid.set_resolution(resolution);
    grid.set_width(width);
    grid.set_height(height);
    grid.set_allocated_origin(&origin);
    std::string data = "12345";
    grid.set_allocated_data(&data);

    // create operation
    OperationDescription desc;
    desc.mutable_operator_()->set_operatorname("myUntraceable");
    desc.set_params("{\"source:\":\"testInlineOperator\"}");
    QVERIFY(repo     != nullptr);
    QVERIFY(workspace != nullptr);
    mapit::OperationResult res = workspace->doUntraceableOperation(desc
                                                                   , [&grid, &epath](mapit::OperationEnvironment* env)
    {
        mapit::operators::WorkspaceWritable *coraw = env->getWorkspace();
        std::shared_ptr<Entity> e(new Entity);
        e->set_type(mapit::entitytypes::Grid2D::TYPENAME());
        mapit::StatusCode status = coraw->storeEntity(epath, e);
        if(!mapitIsOk(status))
        {
            return MAPIT_STATUS_ERROR;
        }
        std::shared_ptr<mapit::AbstractEntitydata> abstractEntitydata = coraw->getEntitydataForReadWrite( epath );
        std::shared_ptr<mapit::entitytypes::Grid2D> entityData = std::dynamic_pointer_cast<mapit::entitytypes::Grid2D>( abstractEntitydata );
        if(entityData == nullptr)
        {
            return MAPIT_STATUS_ERROR;
        }

        //copy from scan, then setData
        std::shared_ptr<mapit::msgs::Grid2D> grid2d(new mapit::msgs::Grid2D);
        grid2d->CopyFrom(grid);
        std::shared_ptr<mapit::entitytypes::Grid2DHelper> help = std::make_shared<mapit::entitytypes::Grid2DHelper>();
        help->setGrid(*grid2d);
        entityData->setData(help);
        return MAPIT_STATUS_OK;
    });
    QVERIFY(mapitIsOk(res.first));

    // release objects, see testGetData
    origin.release_rotation();
    origin.release_translation();
    grid.release_origin();
    grid.release_data();
}

void TestGrid2D::testGetData_data() { createTestdata(false, false); }
void TestGrid2D::testGetData2_data() { createTestdata(false, false); }

void TestGrid2D::testSetData_data() { createTestdata(false, false); }

DECLARE_TEST(TestGrid2D)
