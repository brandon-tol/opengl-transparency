cmake_minimum_required(VERSION 3.8)

project(transparent-rendering)

add_definitions(
	-DGLEW_STATIC
)

IF(WIN32)

# set stuff for windows
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# for windows
include_directories(C:/glfw-3.3.9.bin.WIN64/include/)
include_directories(C:/glew-2.1.0/include/)
include_directories(C:/glew-2.1.0/include/GL/)
include_directories(C:/GLM_0.9.9.8/g-truc-glm-bf71a83/)

link_directories(C:/glew-2.1.0/lib/Release/x64/)
link_directories(C:/glfw-3.3.9.bin.WIN64/lib-vc2019/)

set(ALL_LIBS
	${OPENGL_gl_LIBRARY}
	glew32s 
	glfw3
)

ELSE()

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH
    /encs # For ENCS lab computers
    /opt/local # Macports
)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED
    HINTS /encs/pkg/glfw-3.3.4/root # ENCS installation of glfw
)

set(ALL_LIBS
    GLEW::glew
    OpenGL::GL
    glfw
)

ENDIF()

set(SRC
    src/main.cpp
    src/shader_program.cpp
    src/mesh.cpp
    src/camera.cpp
    src/framebuffer.cpp
    src/window.cpp
)

set(HEADERS
    src/shader_program.h
    src/mesh.h
    src/camera.h
    src/core.h
    src/framebuffer.h
    src/window.h
)

# file(GLOB EXTERNALS external/*.cpp external/*.h)
include_directories(external)


# NOTE: ENCS glm installation is missing links to *.inl files so we need this line
include_directories(/encs/pkg/glm-0.9.9.8/root/include)

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_FILEPATH=\"${PROJECT_SOURCE_DIR}/\")
