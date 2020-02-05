# Version de cmake demandée.
cmake_minimum_required(VERSION 2.8)

find_package(Threads REQUIRED)
 
# Chemin des répertoires contenant les fichiers entêtes.
include_directories(include)

# Chemin du répertoire contenant les binaires.
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Option du compilateur pour supporter C 2011.
set(CMAKE_CXX_FLAGS "-pedantic -Wall -std=c++11 -Wextra -O3")
set(CMAKE_LD_FLAGS "-lpthread")

add_executable(cppack
				src/core/Statsgen.cpp
				src/core/Policy.cpp
				src/core/ThreadData.cpp
				src/core/Utils.cpp
				src/cli/Main.cpp)
target_link_libraries(cppack ${CMAKE_THREAD_LIBS_INIT})

# Création des exécutables.

# Faire parler le make.
set(CMAKE_VERBOSE_MAKEFILE off)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core)
if(Qt5Core_FOUND)
	find_package(Qt5Gui REQUIRED)
	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5Charts REQUIRED)

	add_executable(cppack-gui
		include/MainWindow.h
		include/Statsgen.h
		include/Utils.h
	    src/gui/MainWindow.cpp
	    src/gui/MainGui.cpp
	    src/core/Statsgen.cpp
	    src/core/Policy.cpp
	    src/core/ThreadData.cpp
	    src/core/Utils.cpp
	    ressources/logos.qrc
	)

	target_link_libraries(cppack-gui Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Charts ${CMAKE_THREAD_LIBS_INIT})
else()
	message(STATUS "Qt5 not found. Not building GUI.")
endif()

enable_testing()
add_test(cppack-test bin/cppack-test)
add_executable(cppack-test
				src/core/Statsgen.cpp
				src/core/Policy.cpp
				src/core/ThreadData.cpp
				src/core/Utils.cpp
				src/test/cppack-tests.cpp)
target_link_libraries(cppack-test ${CMAKE_THREAD_LIBS_INIT})