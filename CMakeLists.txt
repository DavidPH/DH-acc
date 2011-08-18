cmake_minimum_required(VERSION 2.6)

project(DH-acc)



if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-Wall -ansi -pedantic-errors)
endif()



add_subdirectory(src)



