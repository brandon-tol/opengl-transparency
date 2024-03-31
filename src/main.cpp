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
#include "ping_pong_framebuffer.h"
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

    glm::mat4 view = cam.view();

    glm::mat4 perspective = cam.perspective();

    // Create and compile our GLSL program from the shaders
    // shader_program program{ BTOLEDA_FILEPATH("/shaders/simple.vert.glsl"), BTOLEDA_FILEPATH("/shaders/simple.frag.glsl") };
    shader_program program{BTOLEDA_FILEPATH("/shaders/depth_peeling.vert.glsl"), BTOLEDA_FILEPATH("/shaders/depth_peeling.frag.glsl")};
    glUseProgram(program);
    program.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);

    ping_pong_framebuffer fb{width, height};

    program.set_uniform(uniform_type::INT, "u_depth", 0);

    // Set the clear color
    auto identity = glm::mat4{1.0f};
    glm::mat4 model, modelview;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glUseProgram(program);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(triangle);

        const int NUM_DEPTH_PASSES = 3;
        for (int p = 0; p < NUM_DEPTH_PASSES; p++)
        {
            for(int q = 0; q < 2; q++) {
                glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer_id(q));
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glEnable(GL_DEPTH_TEST);

                int n = 5;
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                    {
                        model = glm::translate(identity, glm::vec3{ 2.0f * i - 1.0f * n, 0.0f, -1.0f * j });
                        modelview = model * view;
                        program.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
                        glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
                        BTOLEDA_DEBUG_GL();
                    }
                }

                if (p | q)
                {
                    glBindTexture(GL_TEXTURE_2D, fb.depth_texture_id((q + 1) % 2));
                }
            }
        }

        
        fb.render_to(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}