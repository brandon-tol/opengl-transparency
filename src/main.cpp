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

#include <fstream>
#include <sstream>

void parseOBJ(const std::string& filename,
    std::vector<float>& vertices,
    std::vector<float>& normals,
    std::vector<float>& data,
    std::vector<unsigned int>& indices) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') // Ignore empty lines and comments
            continue;

        std::istringstream iss(line);
        
        std::string token;
        iss >> token;
        if (token == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (token == "vn") {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
        else if (token == "f") {
            unsigned long v, n;
            char slash;
            for(int i = 0; i < 3; i++)
            {
                iss >> v >> slash >> slash >> n;
                data.push_back(vertices[(v - 1) * 3]); // Vertex position x
                data.push_back(vertices[(v - 1) * 3 + 1]); // Vertex position y
                data.push_back(vertices[(v - 1) * 3 + 2]); // Vertex position z

                data.push_back(normals[(n - 1) * 3]); // Normal x
                data.push_back(normals[(n - 1) * 3 + 1]); // Normal y
                data.push_back(normals[(n - 1) * 3 + 2]); // Normal z

                // Add color data as needed
                data.push_back(1.0f); // Red
                data.push_back(1.0f); // Green
                data.push_back(1.0f); // Blue
                data.push_back(1.0f); // Alpha (assuming opacity)

                indices.push_back(indices.size()); // Push back the index
            }
        }
    }
    std::clog << indices.size() << "vertices" << std::endl;
    file.close();
}

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

    // Build a camera
    glm::vec3 camera_origin{0.0f, 2.0f, -7.0f};
    glm::vec3 camera_target{0.0f, 0.0f, 0.0f};
    glm::vec3 camera_up{0.0f, 1.0f, 0.0f};
    const double fov = 90;
    camera cam{camera_origin, camera_target, camera_up, fov, aspect_ratio};

    // Temporary arrays
    std::vector<GLfloat> vertices{
        // vertex 1
        -0.5f, -0.5f, 0.0f, // coords
        0.0f, 0.0f, 1.0f,   // normal
        1.0f, 0.0f, 0.0f, 0.55f, // color

        0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.55f,

        0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.55f};

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

    shader_program depth_peel{ BTOLEDA_FILEPATH("/shaders/depth_peel.vert.glsl"), BTOLEDA_FILEPATH("/shaders/depth_peel.frag.glsl") };
    depth_peel.set_uniform(uniform_type::MAT4, "u_perspective", &perspective);

    glm::vec2 viewport_dimensions{ width, height };
    depth_peel.set_uniform(uniform_type::VEC2, "u_viewport_dimensions", &viewport_dimensions);


    shader_program from_framebuffer{ BTOLEDA_FILEPATH("/shaders/from_framebuffer.vert.glsl"), BTOLEDA_FILEPATH("/shaders/from_framebuffer.frag.glsl") };

    // Set the clear color
    glm::mat4 model, modelview;

    glEnable(GL_DEPTH_TEST);

    const int num_passes = 2;

    framebuffer fb[num_passes];

    for (int i = 0; i < num_passes; i++)
    {
        fb[i].set_viewport(width, height);
    }

    BTOLEDA_DEBUG_GL();

    const auto render1 = [&model, &modelview, &view, &triangle](shader_program &prog) {
        int n = 5;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                model = glm::translate(glm::mat4(1.0f), glm::vec3{2.0f * i - 1.0f * n, 0.0f, -1.0f * j});
                modelview = model * view;
                prog.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
                glUseProgram(prog);
                glDrawElements(GL_TRIANGLES, triangle.size, GL_UNSIGNED_INT, nullptr);
                BTOLEDA_DEBUG_GL();
            }
        }
    };

    std::vector<float> teapot_vertices, teapot_normals, teapot_data;
    std::vector<unsigned int> teapot_indices;

    parseOBJ("../assets/teapot1.obj", teapot_vertices, teapot_normals, teapot_data, teapot_indices);
    mesh teapot{ teapot_data, teapot_indices };

    const auto render = [&modelview, &teapot, &view](shader_program& prog)
        {
            modelview = glm::mat4{ 1.0f } * view;
            prog.set_uniform(uniform_type::MAT4, "u_modelview", &modelview);
            glUseProgram(prog);
            glDrawElements(GL_TRIANGLES, teapot.size, GL_UNSIGNED_INT, nullptr);
            BTOLEDA_DEBUG_GL();

        };

    while (!glfwWindowShouldClose(win))
    {
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

        glBindVertexArray(teapot);
        render(program);

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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(screen);
        for (int i = num_passes - 1; i >= 0; i--) 
        {
            glBindTexture(GL_TEXTURE_2D, fb[i].frame());
            glDrawElements(GL_TRIANGLES, screen.size, GL_UNSIGNED_INT, nullptr);

        }
        

        BTOLEDA_DEBUG_GL();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}