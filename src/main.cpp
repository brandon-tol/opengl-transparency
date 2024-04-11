#include <string>
#include <vector>
#include <iostream>
#include <map>

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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace btoleda {
    // Build a camera
    const int g_width = 1024;
    const int g_height = 768;
    camera g_cam{ { 0.0f, 1.0f, -4.0f }, { 0.0f, 1.0f, 0.0f }, 90, (float)width/height };
    float g_last_frame = 0;

    int g_rendermode = 0;

    std::map<int, bool> g_keydown;

}

static void loadObjFile(const std::string& inputfile, std::vector<GLfloat> object_vertices, std::vector<GLuint> object_indices) 
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) 
    {
        if (!reader.Error().empty())
        {
            std::cerr << "Tinyobjloader error: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "Tinyobjloader warning: " << reader.Warning();
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

    // Iterate through shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        const tinyobj::mesh_t& mesh = shapes[s].mesh;

        // Iterate through vertices
        for (size_t v = 0; v < mesh.indices.size(); v++)
        {
            tinyobj::index_t idx = mesh.indices[v];

            // Default color
            // You can set these to any default color you want
            float default_color[] = { 1.0f, 1.0f, 1.0f, 0.55f };

            // Append vertex position, normal, texture coordinates, and color to vertex array
            object_vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
            object_vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
            object_vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
            object_vertices.push_back(0.0f); // Default normal (since OBJ file doesn't have normal)
            object_vertices.push_back(0.0f); // Default normal (since OBJ file doesn't have normal)
            object_vertices.push_back(1.0f); // Default normal (since OBJ file doesn't have normal)
            object_vertices.push_back(default_color[0]);
            object_vertices.push_back(default_color[1]);
            object_vertices.push_back(default_color[2]);
            object_vertices.push_back(default_color[3]);

        }

        // Construct indices array
        for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
            object_indices.push_back(mesh.indices[3 * f + 0].vertex_index);
            object_indices.push_back(mesh.indices[3 * f + 1].vertex_index);
            object_indices.push_back(mesh.indices[3 * f + 2].vertex_index);
        }
    }
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

    mesh triangle{ triangle_vertices, triangle_indices, false };
    // ---------------------------------------------------
 
    // Screen mesh --------------------------------------
    std::vector<GLfloat> basic_quad_vertices{
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f,

        1.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f,

        -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f,
    };

    std::vector<GLuint> basic_quad_indices{
        0, 1, 2,
        2, 3, 0
    };

    mesh screen{ basic_quad_vertices, basic_quad_indices, true };
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

    const auto render_triangles = [&model, &modelview, &triangle](shader_program& prog) {
        glBindVertexArray(triangle);
        int n = 5;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                model = glm::translate(glm::mat4(1.0f), glm::vec3{2.0f * i - 1.0f * n, 0.0f, -1.0f * j});
                modelview = g_cam.view() * model;
                prog.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
                glm::mat4 perspective = g_cam.perspective();
                prog.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);
                glUseProgram(prog);
                glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
                BTOLEDA_DEBUG_GL();
            }
        }
    };

    std::vector<GLfloat> teapot_vertices;
    std::vector<GLuint> teapot_indices;
    loadObjFile("../assets/teapot1.obj", teapot_vertices, teapot_indices);
    mesh teapot{ teapot_vertices, teapot_indices, false };

    const auto render_teapot = [&teapot](shader_program& prog)
        {
			glBindVertexArray(teapot);
			glm::mat4 modelview = g_cam.view();
			prog.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
			glm::mat4 perspective = g_cam.perspective();
			prog.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);
			glUseProgram(prog);
			glDrawElements(GL_TRIANGLES, teapot.size, GL_UNSIGNED_INT, nullptr);
		};

    glfwSetKeyCallback(win, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_W)
            {
                g_keydown[GLFW_KEY_W] = action != GLFW_RELEASE;
            }
            else if (key == GLFW_KEY_S)
            {
				g_keydown[GLFW_KEY_S] = action != GLFW_RELEASE;
			}
            else if (key == GLFW_KEY_A)
            {
			    g_keydown[GLFW_KEY_A] = action != GLFW_RELEASE;
			}
            else if (key == GLFW_KEY_D)
            {
				g_keydown[GLFW_KEY_D] = action != GLFW_RELEASE;
			}
            else if (key == GLFW_KEY_LEFT_CONTROL)
            {
				g_keydown[GLFW_KEY_LEFT_CONTROL] = action != GLFW_RELEASE;
			}
            else if (key == GLFW_KEY_SPACE)
            {
				g_keydown[GLFW_KEY_SPACE] = action != GLFW_RELEASE;
            }
            else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
            {
				glfwSetWindowShouldClose(window, true);
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_1)
            {
                g_rendermode = g_rendermode > 0 ? 1 : -1;
            }
            else if (action == GLFW_PRESS && key == GLFW_KEY_2)
            {
				g_rendermode = g_rendermode > 0 ? 2 : -2;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_3)
            {
				g_rendermode = g_rendermode > 0 ? 3 : -3;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_4)
            {
				g_rendermode = g_rendermode > 0 ? 4 : -4;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_5)
            {
                g_rendermode = g_rendermode > 0 ? 5 : -5;
            }
            else if (action == GLFW_PRESS && key == GLFW_KEY_0)
            {
				g_rendermode = 0;
			}
            else if (action == GLFW_PRESS && key == GLFW_KEY_MINUS)
            {
                g_rendermode = -g_rendermode;
            }
        });

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(win, [](GLFWwindow* window, double xpos, double ypos) {
        static bool firstMouse = true;
        static float lastX = 0;
        static float lastY = 0;
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        g_cam.rotate(xoffset, yoffset);
        });

    glfwSetScrollCallback(win, [](GLFWwindow* window, double xoffset, double yoffset) {
        g_cam.zoom(yoffset);
        });
    

    while (!glfwWindowShouldClose(win))
    {
        float delta = glfwGetTime() - g_last_frame;
        g_last_frame = glfwGetTime();
        auto render = render_teapot;
        if (g_keydown[GLFW_KEY_W]) {
            g_cam.move(FORWARD, g_cam.speed() * delta);
        }
        if (g_keydown[GLFW_KEY_S]) {
            g_cam.move(BACKWARD, g_cam.speed() * delta);
        }
        if (g_keydown[GLFW_KEY_A]) {
            g_cam.move(LEFT, g_cam.speed() * delta);
        }
        if (g_keydown[GLFW_KEY_D]) {
            g_cam.move(RIGHT, g_cam.speed() * delta);
        }
        if (g_keydown[GLFW_KEY_LEFT_CONTROL]) {
            g_cam.move(DOWN, g_cam.speed() * delta);
        }
        if (g_keydown[GLFW_KEY_SPACE]) {
            g_cam.move(UP, g_cam.speed() * delta);
        }
       
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

        if (g_rendermode == 0)
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
        else if (g_rendermode < 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(screen);
            for (int i = -g_rendermode - 1; i >= 0; i--)
            {
                glBindTexture(GL_TEXTURE_2D, fb[i].frame());
                glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);

            }
        }
        else if (g_rendermode > 0)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(screen);
            glBindTexture(GL_TEXTURE_2D, fb[g_rendermode - 1].frame());
            glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);
        }
        

        BTOLEDA_DEBUG_GL();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}