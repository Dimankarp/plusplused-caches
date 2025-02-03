file(GLOB_RECURSE CACHES_SOURCES *.cpp *.hpp)
add_library(caches ${CACHES_SOURCES})
target_include_directorieS(caches PUBLIC ./)