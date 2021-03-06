################################################################################
#
# Copyright 2016-2017 Daniel Bulla	<d.bulla@fh-aachen.de>
#                2018 Tobias Neumann	<t.neumann@fh-aachen.de>
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

project(networking_node)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 14)

find_package(mapit REQUIRED)

#zeromq
find_package(ZMQ REQUIRED)
include_directories(${ZMQ_INCLUDE_DIR})

#cppzeromq
find_package(CPPZMQ REQUIRED)
include_directories(${CPPZMQ_INCLUDE_DIR})

file(GLOB_RECURSE MAPIT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/include/*.h )

add_library(${PROJECT_NAME} SHARED ${MAPIT_SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
        PRIVATE
           src
)

target_link_libraries(${PROJECT_NAME} ${mapit_core} ${mapit_interface_cpp} ${ZMQ_LIBRARIES} ${Boost_LIBRARIES})

if(MAPIT_ENABLE_LOG4CPLUS)
  target_link_libraries(${PROJECT_NAME} ${LOG4CPLUS_LIBRARIES})
endif(MAPIT_ENABLE_LOG4CPLUS)

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} dl)
endif(NOT WIN32)

if(NOT WIN32)
  target_link_libraries(${PROJECT_NAME} uuid ${libzip})
endif(NOT WIN32)

#install(TARGETS ${PROJECT_NAME} DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE})
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/${CMAKE_LIBRARY_ARCHITECTURE} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME mapit_${PROJECT_NAME})

include(GenerateExportHeader)

generate_export_header(${PROJECT_NAME})
#set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${MAPIT_VERSION})
#set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${MAPIT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY
  INTERFACE_mapit_networking_node_MAJOR_VERSION ${MAPIT_MAJOR_VERSION})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING MAPIT_MAJOR_VERSION
)

install(TARGETS ${PROJECT_NAME} EXPORT mapit_networking_node_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(DIRECTORY include/mapit DESTINATION include )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/mapit_networking_nodeConfigVersion.cmake"
  VERSION 1
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT mapit_networking_node_targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/mapit_networking_nodeTargets.cmake"
  NAMESPACE mapit::
)
configure_file(cmake/mapit_networking_nodeConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/mapit_networking_nodeConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/mapit)
install(EXPORT mapit_networking_node_targets
  FILE
    mapit_networking_nodeTargets.cmake
  NAMESPACE
    mapit::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/mapit_networking_nodeConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/mapit_networking_nodeConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
