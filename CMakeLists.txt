cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DDWARFQUEST_OPEN_GL)

project(DwarfQuest)

add_executable(DwarfQuest src/Main.cpp)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
target_link_libraries(DwarfQuest
    PRIVATE
        ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES}
)

target_include_directories(DwarfQuest PRIVATE libs/glm)
target_include_directories(DwarfQuest PRIVATE ${SDL2_INCLUDE_DIRS})

include(src/CMakeLists.txt)
include(src/Graphics/OpenGL/CMakeLists.txt)
include(src/Utils/CMakeLists.txt)
