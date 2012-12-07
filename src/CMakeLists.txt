#include_directories(${QCP_DIR})
set(SOURCES
	mainwindow.cpp)

set(MOCS
	mainwindow.h)

set(UIS
	main.ui)

set(RESOURCES
	balanceboard.qrc)

include(${QT_USE_FILE})

# Let QT pre-process the files and add the generated files to the source list
qt4_wrap_ui(GENERATED_SOURCES ${UIS})
qt4_add_resources(GENERATED_SOURCES ${RESOURCES})
qt4_wrap_cpp(GENERATED_SOURCES ${MOCS})

source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${VRPN_INCLUDE_DIRS} ${QVRPN_INCLUDE_DIRS})
add_definitions(${VRPN_DEFINITIONS})
add_executable(vrpn-balance-board
	WIN32
	MACOSX_BUNDLE
	#${QCP_SOURCES}
	${SOURCES}
	${MOCS}
	${GENERATED_SOURCES})

target_link_libraries(vrpn-balance-board ${VRPN_LIBRARIES} ${QVRPN_LIBRARIES} ${QT_LIBRARIES})

if(APPLE)
	set_target_properties(vrpn-balance-board
		PROPERTIES
		OUTPUT_NAME "VRPN Balance Board Client")
endif()
