cmake_minimum_required (VERSION 2.8)

project (nes_demo)

set (TAGET nes_demo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory (./ TOP_SRC)
aux_source_directory (src SUB_SRC)

set (SRC_LIST 
	${PROJECT_SOURCE_DIR}/src/bus.cpp
	${PROJECT_SOURCE_DIR}/src/olc6502.cpp
	${PROJECT_SOURCE_DIR}/main.cpp
	)


include_directories (${PROJECT_SOURCE_DIR}
					 inc 
					 )

add_executable(${TAGET}
			${SRC_LIST}
			)

target_link_libraries(${TAGET}
		pthread
		)
