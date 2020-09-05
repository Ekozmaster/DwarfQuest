cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
    message(" [INFO] VCPKG CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
endif()

add_definitions(-DDWARFQUEST_OPEN_GL)

OPTION(DWARFQUEST_TESTING "Runs unit tests" OFF)
if (DWARFQUEST_TESTING)
    add_definitions(-DDWARFQUEST_TESTING)
endif (DWARFQUEST_TESTING)

project(DwarfQuest)

add_executable(DwarfQuest src/Main.cpp)

set (assets_source "${CMAKE_SOURCE_DIR}/Assets")
set (assets_destination "${CMAKE_CURRENT_BINARY_DIR}/Assets")
add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink ${assets_source} ${assets_destination} 
 DEPENDS ${assets_destination}
 COMMENT "symbolic link resources folder from ${assets_source} => ${assets_destination}"
)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
target_link_libraries(DwarfQuest
    PRIVATE
        ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS}
)

target_include_directories(DwarfQuest PRIVATE libs/glm)
target_include_directories(DwarfQuest PRIVATE libs/glad/include)
target_include_directories(DwarfQuest PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(DwarfQuest PRIVATE ${CMAKE_CURRENT_LIST_DIR})

include(src/CMakeLists.txt)
include(src/Graphics/OpenGL/CMakeLists.txt)
include(src/ResourceManagement/CMakeLists.txt)
include(src/Utils/CMakeLists.txt)
include(src/EntitiesBehaviourModel/CMakeLists.txt)
include(src/EntitiesBehaviourModel/Behaviour/CMakeLists.txt)
include(src/EntitiesBehaviourModel/Behaviour/GameComponents/CMakeLists.txt)
include(src/SceneManagement/CMakeLists.txt)
include(src/DataStructures/CMakeLists.txt)

include(libs/glad/include/glad/CMakeLists.txt)
include(libs/glad/src/CMakeLists.txt)
