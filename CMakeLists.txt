cmake_minimum_required (VERSION 3.10)

if(POLICY CMP0054)
        cmake_policy(SET CMP0054 NEW)
endif()

project(
    Accuracy_Test
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${PROJECT_SOURCE_DIR}/src/PhoxiControl)
include_directories(${PROJECT_SOURCE_DIR}/src/DataProcess)
include_directories(${PROJECT_SOURCE_DIR}/src/Widget)
include_directories(${PROJECT_SOURCE_DIR}/src/SocketClient)
include_directories(${PROJECT_SOURCE_DIR}/src/SocketServer)

# PhoxiControl
if (NOT PHO_API_CMAKE_CONFIG_PATH)
    set(PHO_API_CMAKE_CONFIG_PATH "$ENV{PHOXI_CONTROL_PATH}")
endif()

find_package(PhoXi REQUIRED CONFIG PATHS "${PHO_API_CMAKE_CONFIG_PATH}")

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

# Eigen3
find_package(Eigen3 REQUIRED)
include_directories(${Eigen3_INCLUDE_DIRS})
link_directories(${Eigen3_LIBRARY_DIRS})

if (UNIX AND NOT APPLE)
    add_compile_options(-std=c++1y)
    add_compile_options(-pthread)
endif(UNIX AND NOT APPLE)

file(GLOB PhoxiControl_srcs "src/PhoxiControl/*")
file(GLOB DataProcess_srcs "src/DataProcess/*")
file(GLOB Widget_srcs "src/Widget/*")
file(GLOB SocketClient_srcs "src/SocketClient/*")
file(GLOB SocketServer_srcs "src/SocketServer/*")

set(CMAKE_PREFIX_PATH $ENV{Qt5})
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
    ${PhoxiControl_srcs}
    ${DataProcess_srcs}
    ${Widget_srcs}
    ${SocketClient_srcs}
    ${SocketServer_srcs}
    )

target_include_directories (${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${PHOXI_INCLUDE_DIRS} ${Eigen3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    ${PHOXI_LIBRARY}
    $<$<PLATFORM_ID:Linux>:rt>
    ${OpenCV_LIBS}
    ${Eigen3_LIBS}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
)

