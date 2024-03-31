#pragma once

#define BTOLEDA_DEBUG_GL(x)                                          \
    x;                                                               \
    do                                                               \
    {                                                                \
        GLenum error = glGetError();                                 \
        if (error != GL_NO_ERROR)                                    \
        {                                                            \
            printf("OpenGL Error: %s\n", glewGetErrorString(error)); \
        }                                                            \
    } while (0)

#ifndef CMAKE_FILEPATH
#define CMAKE_FILEPATH "C:/Users/Brandon/School/COMP371_Winter2024/OpenGLProject/build/"
#endif
#define BTOLEDA_FILEPATH(x) (CMAKE_FILEPATH x)