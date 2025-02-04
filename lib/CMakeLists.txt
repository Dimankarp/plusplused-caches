

file(GLOB_RECURSE CACHES_SOURCES *.cpp *.hpp)
add_library(caches ${CACHES_SOURCES})
set_target_properties(caches PROPERTIES LINKER_LANGUAGE CXX)
target_include_directorieS(caches PUBLIC ./)