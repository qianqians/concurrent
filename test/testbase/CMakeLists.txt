PROJECT(testbase)
CMAKE_MINIMUM_REQUIRED(VERSION 4.0) 

AUX_SOURCE_DIRECTORY(. DIR_SRC)

SET(header_files
    base_test.h
)

ADD_LIBRARY(testbase ${DIR_SRC} ${header_files})