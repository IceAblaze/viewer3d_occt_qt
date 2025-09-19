cmake_minimum_required(VERSION 3.16)
project(Viewer3D_OCCT_Qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenCASCADE CONFIG REQUIRED)

set(SRCS
    src/main.cpp
    src/MainWindow.cpp
    src/OcctViewer.cpp
)
set(HDRS
    src/MainWindow.h
    src/OcctViewer.h
)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCASCADE_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::OpenGLWidgets ${OpenCASCADE_LIBRARIES})

if (WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
