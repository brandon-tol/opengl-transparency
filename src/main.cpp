#include <string>
#include <vector>
#include <iostream>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "mesh.h"

#define BTOLEDA_DEBUG_GL(x)                                      \
    x;                                                           \
    {                                                            \
        GLenum error = glGetError();                             \
        if (error != GL_NO_ERROR)                                \
        {                                                        \
            printf("OpenGL Error: %s\n", glewGetErrorString(error)); \
        }                                                        \
    }
#ifndef CMAKE_FILEPATH
#define CMAKE_FILEPATH "C:\\Users\\Brandon\\School\\COMP371_Winter2024\\OpenGLProject\\build\\"
#endif
#define BTOLEDA_FILEPATH(x) (CMAKE_FILEPATH x)


int main(int argc, char **argv)
{
    using namespace std;
    using namespace btoleda;

    // Setup some default values
    const int width = 1024;
    const int height = 768;
    const double fov = 90;
    const string title = "Hello World";

    // Initialize the library
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "Failed to initialize GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(window, [](GLFWwindow*, GLint width, GLint height) {
        glViewport(0, 0, width, height);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                }
            }
        });

    // Temporary arrays
    std::vector<GLfloat> vertices {
        // vertex 1
        -0.5f, -0.5f, 0.0f, // coords
        0.0f, 0.0f, 1.0f,   // normal
        1.0, 0.0, 0.0, 1.0,     // color

        0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0,

        0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0
    };

    std::vector<GLuint> indices{
        // Triangle 1
        0, 1 ,2
    };

    // Define mesh
    mesh triangle{ vertices, indices };

    // Build a camera
    glm::vec3 camera_origin{ 0.0f, 0.0f, -3.0f };
    glm::vec3 camera_target{ 0.0f, 0.0f, 0.0f };
    glm::vec3 camera_up{ 0.0f, 1.0f, 0.0f };
    glm::mat4 view = glm::lookAt(camera_origin, camera_target, camera_up);

    glm::mat4 perspective = glm::perspective<float>(glm::radians(fov), (float)width / height, 0.1f, 100.0f);

    // Create and compile our GLSL program from the shaders
    shader_program program{ BTOLEDA_FILEPATH("/shaders/simple.vert.glsl"), BTOLEDA_FILEPATH("/shaders/simple.frag.glsl") };
    glUseProgram(program);
    program.set_uniform(uniform_type::MAT4, "perspective", &perspective);


    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    auto identity = glm::mat4{ 1.0f };
    glm::mat4 model, modelview;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const int num_triangles = 5;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(triangle);

        for(int i = 0; i < num_triangles; i++)
        {
            for (int j = 0; j < num_triangles; j++)
            {
                model = glm::translate(identity, glm::vec3{ 2.0f * i - 1.0f * num_triangles, 0.0f, -1.0f * j });
                modelview = model * view;
                program.set_uniform(uniform_type::MAT4, "modelview", &modelview);
                glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}