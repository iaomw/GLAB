cmake_minimum_required (VERSION 3.2)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(P_NAME GLAB)
project(${P_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/myproj)

# file(GLOB sources CONFIGURE_DEPENDS *.h *.c *.cpp)
AUX_SOURCE_DIRECTORY(myproj sources) 
add_executable(${P_NAME} ${sources})

# Main Executable
# add_definitions("-Wall -O0 -g")
# add_executable(${P_NAME} main.c)
# target_include_directories(${P_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

# SDL2
# find_package(SDL2 REQUIRED)
# target_link_libraries(sdl_app ${SDL2_LIBRARY})
# target_include_directories(sdl_app PUBLIC ${SDL2_INCLUDE_DIR})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${P_NAME}
    PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${P_NAME} PRIVATE ${OPENGL_gl_LIBRARY})
target_include_directories(${P_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
target_link_libraries(${P_NAME} PRIVATE GLEW::GLEW)

# GL Extension Wrangler (GLEW)
# find_package(GLEW REQUIRED)
# target_link_libraries(sdl_app PRIVATE ${GLEW_LIBRARIES})
# target_include_directories(sdl_app PUBLIC ${GLEW_INCLUDE_DIRS})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${P_NAME} PRIVATE glm::glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${P_NAME} PRIVATE imgui::imgui)
# include_directories(
#     ${IMGUI_INCLUDE_DIR}
#     ${OPENGL_INCLUDE_DIR}
#     ${SDL2_INCLUDE_DIR})

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${P_NAME} PRIVATE magic_enum::magic_enum)

find_package(Stb REQUIRED)
target_include_directories(${P_NAME} PRIVATE ${Stb_INCLUDE_DIR})
