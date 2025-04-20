project(test_msque)
CMAKE_MINIMUM_REQUIRED(VERSION 4.0) 

AUX_SOURCE_DIRECTORY(. DIR_SRC)

ADD_executable(test_msque ${DIR_SRC})