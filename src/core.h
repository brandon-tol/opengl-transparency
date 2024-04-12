#pragma once

#define BTOLEDA_DEBUG_GL(x)                                          \
    x;                                                               \
    do                                                               \
    {                                                                \
        GLenum error = glGetError();                                 \
        if (error != GL_NO_ERROR)                                    \
        {                                                            \
            printf("(Line %d) OpenGL Error: %s\n", __LINE__, glewGetErrorString(error)); \
        }                                                            \
    } while (0)

#ifndef CMAKE_FILEPATH
#define CMAKE_FILEPATH "C:/Users/Brandon/School/COMP371_Winter2024/OpenGLProject/build/"
#endif
#define BTOLEDA_FILEPATH(x) (CMAKE_FILEPATH x)

#define BTOLEDA_SHADER(x) shader_program x{BTOLEDA_FILEPATH("/shaders/" #x ".vert.glsl"), BTOLEDA_FILEPATH("/shaders/" #x ".frag.glsl")}

#define BTOLEDA_LOG(x) do\
    {\
        std::cout << __LINE__ << ": " << x << std::endl;\
    } while (0)