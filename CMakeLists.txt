# Version de cmake demandée.
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

FIND_PACKAGE (Threads)
 
# Chemin des répertoires contenant les fichiers entêtes.
INCLUDE_DIRECTORIES( include )

# Chemin du répertoire contenant les binaires.
SET ( EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE} )

# Option du compilateur pour supporter C 2011.
SET( CMAKE_CXX_FLAGS "-pedantic -Wall -std=c++11 -Wextra -O3")
SET( CMAKE_LD_FLAGS "-lpthread")

ADD_EXECUTABLE(cppack src/core/statsgen.cpp src/cli/main.cpp)
target_link_libraries (cppack ${CMAKE_THREAD_LIBS_INIT})

# Création des exécutables.

# Faire parler le make.
SET( CMAKE_VERBOSE_MAKEFILE off )