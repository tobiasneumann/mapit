################################################################################
#
# Copyright 2015-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
#           2015-2017 Tobias Neumann	<t.neumann@fh-aachen.de>
#
################################################################################

#  This file is part of mapit.
#
#  Mapit is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Mapit is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with mapit.  If not, see <http://www.gnu.org/licenses/>.
#

if(NOT DISABLE_PCL)
project(filter_voxelgrid)
cmake_minimum_required(VERSION 2.8)

set(OPERATOR_VERSION 1)

find_package(mapit REQUIRED)

add_definitions(-DOPERATOR_NAME="${PROJECT_NAME}")

add_definitions(-DOPERATOR_VERSION=${OPERATOR_VERSION})

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

#if(NOT TARGET json11)
#    find_package(json11 REQUIRED)
#endif(NOT TARGET json11)

# Note: Using c++11 in combination with pcl voxelgrid (1.7) filter
# can lead problems resulting from to incompatible ABIs
# (
# solution:
# git clone https://github.com/PointCloudLibrary/pcl.git
# git checkout pcl-1.7.2
# add line 'SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")' to CMakeLists.txt
# mkdir build
# cd build
# cmake ..
# make
# sudo make install

find_package(PCL QUIET REQUIRED COMPONENTS common io filters)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

file(GLOB_RECURSE OPERATOR_VOXELGRIDFILTER_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
add_library(${PROJECT_NAME} SHARED ${OPERATOR_VOXELGRIDFILTER_SOURCES})

target_link_libraries( ${PROJECT_NAME}
    ${PCL_LIBRARIES}
    ${PCL_COMMON_LIBRARIES}
    ${PCL_IO_LIBRARIES}
    ${PCL_FILTERS_LIBRARIES}
    ${mapit_interface}
    ${mapit_core}
    layertype_pointcloud2)

qt5_use_modules(${PROJECT_NAME} Core)

SET_TARGET_PROPERTIES(
    ${PROJECT_NAME}
  PROPERTIES
    VERSION ${OPERATOR_VERSION}
    SOVERSION ${OPERATOR_VERSION}
)

if(UNIX)
    # tell CMake Prefix is "lib" otherwise it can not be queried
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif(UNIX)
get_target_property(OLD_PREFIX ${PROJECT_NAME} PREFIX)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX ${OLD_PREFIX}${MAPIT_INSTALL_OPERATORS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

endif(NOT DISABLE_PCL)
