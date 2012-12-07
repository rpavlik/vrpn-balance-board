cmake_minimum_required(VERSION 2.6)

# Set package properties
project(vrpn-balance-board)

###
# Options
###



###
# Perform build configuration of dependencies
###

find_package(Qt4 COMPONENTS QtCore QtGui QtSvg qtmain REQUIRED)

set(VRPN_BUILD_CLIENT_LIBRARY TRUE CACHE INTERNAL "" FORCE)
add_subdirectory(vrpn)

set(VRPN_LIBRARIES ${VRPN_CLIENT_LIBRARY})
set(VRPN_FOUND TRUE)
add_definitions(${VRPN_DEFINITIONS} -DVRPN_CLIENT_ONLY)

add_subdirectory(qvrpn)

set(QCP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/QCP)
set(QCP_SOURCES ${QCP_DIR}/qcustomplot.cpp ${QCP_DIR}/qcustomplot.h)

###
# Build the project
###

# The app is in the "src" subdirectory
add_subdirectory(src)
