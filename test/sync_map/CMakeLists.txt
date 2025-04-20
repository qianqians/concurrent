project(test_sync_map)
CMAKE_MINIMUM_REQUIRED(VERSION 4.0) 

AUX_SOURCE_DIRECTORY(. DIR_SRC)

ADD_executable(test_sync_map ${DIR_SRC})