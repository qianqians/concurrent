PROJECT(concurrent)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

set(concurrent_include_path ${PROJECT_SOURCE_DIR} CACHE PATH "concurrent path")
INCLUDE_DIRECTORIES(${concurrent_include_path})

ADD_SUBDIRECTORY(test)