cmake_minimum_required(VERSION 3.20) # For Hunter

project(project)

# add_subdirectory(src)

set(APP_NAME "DepthAI_BLOB_benchmark")

set(CMAKE_PREFIX_PATH "$ENV{OPENCV_DIR}/../GitHub/opencv/build;$ENV{DEPTHAI_DIR}")
# set(CMAKE_PREFIX_PATH "$ENV{OPENCV_DIR}/../GitHub/opencv/build;$ENV{DEPTHAI_DIR2}")
# set(CMAKE_PREFIX_PATH "$ENV{OPENCV_DIR}/build;$ENV{DEPTHAI_DIR}")

# set(layer_directory "${CMAKE_BINARY_DIR}/${layer_name}/")
# file(MAKE_DIRECTORY ${layer_directory})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${layer_directory})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${layer_directory})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${layer_directory})
set(CMAKE_CXX_STANDARD 17)

add_definitions( -DXLINK_USE_MX_ID_NAME=ON -D__PC__=ON)

find_library(DEPTHAI_CORE_LIB NAMES depthai-core NO_CACHE )
find_library(DEPTHAI_OPENCV_LIB NAMES depthai-opencv NO_CACHE )

if ( NOT DEPTHAI_CORE_LIB )
	message( FATAL_ERROR "ERROR DepthAI-Core Lib NOT FOUND !")
endif( NOT DEPTHAI_CORE_LIB)
message( STATUS "Found DepthAI-Core Lib ! " (${DEPTHAI_CORE_LIB}))

if ( NOT DEPTHAI_OPENCV_LIB )
	message( FATAL_ERROR "ERROR DepthAI-OpenCV Lib NOT FOUND !")
endif( NOT DEPTHAI_OPENCV_LIB)
message( STATUS "Found DepthAI-OpenCV Lib ! " (${DEPTHAI_OPENCV_LIB}))

find_package( OpenCV REQUIRED )

# add_executable(${APP_NAME} host_decoding/main.cpp)
# add_executable(${APP_NAME} host_decoding/test.cpp)
add_executable(${APP_NAME} host_decoding/test2.cpp)

target_include_directories(${APP_NAME} PRIVATE
	# $ENV{DEPTHAI_DIR2}/include
	# $ENV{DEPTHAI_DIR2}/include/depthai-shared/3rdparty
	# $ENV{DEPTHAI_DIR2}/lib/cmake/depthai/dependencies/include
	$ENV{DEPTHAI_DIR}/include
	$ENV{DEPTHAI_DIR}/include/depthai-shared/3rdparty
	$ENV{DEPTHAI_DIR}/lib/cmake/depthai/dependencies/include
	${OpenCV_INCLUDE_DIRS}
	)

target_link_libraries( ${APP_NAME} PRIVATE ${OpenCV_LIBS} ${DEPTHAI_OPENCV_LIB} ${DEPTHAI_CORE_LIB} )