cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(nes_demo)

set (TAGET nes_demo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CXX_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")



set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} inc)

set (SRC_LIST 
	${PROJECT_SOURCE_DIR}/src/bus.cpp
	${PROJECT_SOURCE_DIR}/src/olc6502.cpp
	${PROJECT_SOURCE_DIR}/main.cpp
	)

add_executable(${TAGET}
			${SRC_LIST}
			)

string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(${TAGET}
		pthread
        ${SDL2_LIBRARIES}
		)
