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
#include "framebuffer.h"
#include "window.h"

namespace btoleda {
    // Build a camera
    const int width = 1024;
    const int height = 768;
    camera g_cam{ { 0.0f, 1.0f, -4.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 90, (float)width/height };
    float last_frame = 0;
    float delta = 0;

    int rendermode = 0;
}

int main(int argc, char **argv)
{
    using namespace std;
    using namespace btoleda;

    const string title = "Depth Peeling Demo";

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

    window win{ width, height, title };    
    glfwMakeContextCurrent(win);


    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "Failed to initialize GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(win, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });
    // Triangle mesh ------------------------------------
    std::vector<GLfloat> triangle_vertices{
        // vertex 1
        -0.5f, -0.5f, 0.0f, // coords
        0.0f, 0.0f, 1.0f,   // normal
        1.0f, 0.0f, 0.0f, 0.55f, // color

        0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 0.55f,

        0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.55f
    };

    std::vector<GLuint> triangle_indices{
        // Triangle 1
        0, 1, 2
    };

    mesh triangle{ triangle_vertices, triangle_indices };
    // ---------------------------------------------------
 
    // Screen mesh --------------------------------------
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
    // ---------------------------------------------------

    // Create and compile our GLSL program from the shaders
    shader_program simple{BTOLEDA_FILEPATH("/shaders/simple.vert.glsl"), BTOLEDA_FILEPATH("/shaders/simple.frag.glsl")};
    shader_program depth_peel{ BTOLEDA_FILEPATH("/shaders/depth_peel.vert.glsl"), BTOLEDA_FILEPATH("/shaders/depth_peel.frag.glsl") };

    glm::vec2 viewport_dimensions{ width, height };
    depth_peel.set_uniform(uniform_type::VEC2, "u_viewport_dimensions", &viewport_dimensions);

    shader_program from_framebuffer{ BTOLEDA_FILEPATH("/shaders/from_framebuffer.vert.glsl"), BTOLEDA_FILEPATH("/shaders/from_framebuffer.frag.glsl") };

    glm::mat4 model, modelview;

    // Create a framebuffer per peel for depth peeling
    const int num_passes = 5;

    framebuffer fb[num_passes];

    for (int i = 0; i < num_passes; i++)
    {
        fb[i].set_viewport(width, height);
    }

    BTOLEDA_DEBUG_GL();

    const auto render = [&model, &modelview, &triangle](shader_program& prog) {
        glBindVertexArray(triangle);
        int n = 5;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                model = glm::translate(glm::mat4(1.0f), glm::vec3{2.0f * i - 1.0f * n, 0.0f, -1.0f * j});
                modelview = model * g_cam.view();
                prog.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
                glm::mat4 perspective = g_cam.perspective();
                prog.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);
                glUseProgram(prog);
                glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
                BTOLEDA_DEBUG_GL();
            }
        }
    };

    glfwSetKeyCallback(win, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            if (key == GLFW_KEY_W)
            {
                g_cam.move(FORWARD, g_cam.speed() * delta);
            }
            else if (key == GLFW_KEY_S)
            {
				g_cam.move(BACKWARD, g_cam.speed() * delta);
			}
            else if (key == GLFW_KEY_A)
            {
				g_cam.move(LEFT, g_cam.speed() * delta);
			}
            else if (key == GLFW_KEY_D)
            {
				g_cam.move(RIGHT, g_cam.speed() * delta);
			}
            else if (key == GLFW_KEY_LEFT_CONTROL)
            {
				g_cam.move(DOWN, g_cam.speed() * delta);
			}
            else if (key == GLFW_KEY_SPACE)
            {
				g_cam.move(UP, g_cam.speed() * delta);
            }
            else if (key == GLFW_KEY_ESCAPE)
            {
				glfwSetWindowShouldClose(window, true);
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_1)
            {
                rendermode = rendermode > 0 ? 1 : -1;
            }
            else if (action == GLFW_PRESS && key == GLFW_KEY_2)
            {
				rendermode = rendermode > 0 ? 2 : -2;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_3)
            {
				rendermode = rendermode > 0 ? 3 : -3;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_4)
            {
				rendermode = rendermode > 0 ? 4 : -4;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_5)
            {
                rendermode = rendermode > 0 ? 5 : -5;
            }
            else if (action == GLFW_PRESS && key == GLFW_KEY_0)
            {
				rendermode = 0;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_MINUS)
            {
                rendermode = -rendermode;
            }
        }
        });
    

    while (!glfwWindowShouldClose(win))
    {
        float current_frame = glfwGetTime();
        delta = current_frame - last_frame;
        last_frame = current_frame;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        // First pass
        glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);

        BTOLEDA_DEBUG_GL();

        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);  // Draw to COLOR_ATTACHMENT0 (always uses the depth buffer, specifying causes an error)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render(simple);

        // Peel away the first layer to render only subsequent layers
        for (int i = 1; i < num_passes; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fb[i]);
            glBindTexture(GL_TEXTURE_2D, fb[i-1].depth_buffer());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render(depth_peel);
        }
        
        // Render the resulting layers to screen in reverse order
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(from_framebuffer);
        glDisable(GL_DEPTH_TEST);

        if (rendermode == 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(screen);
            for (int i = num_passes - 1; i >= 0; i--) 
            {
                glBindTexture(GL_TEXTURE_2D, fb[i].frame());
                glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);

            }
        }
        else if (rendermode < 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(screen);
            for (int i = -rendermode - 1; i >= 0; i--)
            {
                glBindTexture(GL_TEXTURE_2D, fb[i].frame());
                glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);

            }
        }
        else if (rendermode > 0)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(screen);
            glBindTexture(GL_TEXTURE_2D, fb[rendermode - 1].frame());
            glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);
        }
        

        BTOLEDA_DEBUG_GL();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}