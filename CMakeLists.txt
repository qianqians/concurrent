PROJECT(lock_free)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

set(lock_free_include_path ${PROJECT_SOURCE_DIR} CACHE PATH "lock_free path")
INCLUDE_DIRECTORIES(${lock_free_include_path})

ADD_SUBDIRECTORY(test)