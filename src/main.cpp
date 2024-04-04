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
#include "camera.h"
#include "core.h"

int main(int argc, char **argv)
{
    using namespace std;
    using namespace btoleda;

    // Setup some default values
    // TODO: Replace with window abstraction
    int width = 1024;
    int height = 768;
    float aspect_ratio = (float)width / height;
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

    glfwSetWindowSizeCallback(window, [](GLFWwindow *, GLint width, GLint height)
                              { glViewport(0, 0, width, height); });

    // Build a camera
    glm::vec3 camera_origin{0.0f, 0.0f, -3.0f};
    glm::vec3 camera_target{0.0f, 0.0f, 0.0f};
    glm::vec3 camera_up{0.0f, 1.0f, 0.0f};
    const double fov = 90;
    camera cam{camera_origin, camera_target, camera_up, fov, aspect_ratio};

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
            if (action == GLFW_PRESS)
            {
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                }
            } });

    // Temporary arrays
    std::vector<GLfloat> vertices{
        // vertex 1
        -0.5f, -0.5f, 0.0f, // coords
        0.0f, 0.0f, 1.0f,   // normal
        1.0, 0.0, 0.0, 1.0, // color

        0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0,

        0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0};

    std::vector<GLuint> indices{
        // Triangle 1
        0, 1, 2};

    // Define mesh
    mesh triangle{vertices, indices};

    std::vector<GLfloat> basic_quad_vertices{
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        1.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    std::vector<GLuint> basic_quad_indices{
        0, 1, 2,
        2, 3, 0
    };

    mesh screen{ basic_quad_vertices, basic_quad_indices };

    glm::mat4 view = cam.view();

    glm::mat4 perspective = cam.perspective();

    // Create and compile our GLSL program from the shaders
    // shader_program program{ BTOLEDA_FILEPATH("/shaders/simple.vert.glsl"), BTOLEDA_FILEPATH("/shaders/simple.frag.glsl") };
    shader_program program{BTOLEDA_FILEPATH("/shaders/simple.vert.glsl"), BTOLEDA_FILEPATH("/shaders/simple.frag.glsl")};
    program.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);

    shader_program from_framebuffer{ BTOLEDA_FILEPATH("/shaders/from_framebuffer.vert.glsl"), BTOLEDA_FILEPATH("/shaders/from_framebuffer.frag.glsl") };
    //program.set_uniform(uniform_type::INT, "u_depth", 0);

    // Set the clear color
    auto identity = glm::mat4{1.0f};
    glm::mat4 model, modelview;

    glEnable(GL_DEPTH_TEST);

    // To be abstracted
    GLuint fb, tex, db;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glGenRenderbuffers(1, &db);
    glBindRenderbuffer(GL_RENDERBUFFER, db);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, db);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        return 1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    while (!glfwWindowShouldClose(window))
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fb);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(triangle);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int n = 5;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                model = glm::translate(identity, glm::vec3{ 2.0f * i - 1.0f * n, 0.0f, -1.0f * j });
                modelview = model * view;
                program.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
                glUseProgram(program);
                glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
                BTOLEDA_DEBUG_GL();
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(from_framebuffer);
        glBindVertexArray(screen);
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);
        BTOLEDA_DEBUG_GL();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}