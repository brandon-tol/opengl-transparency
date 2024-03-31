#include "ping_pong_framebuffer.h"
#include "shader_program.h"
#include "core.h"

namespace btoleda
{
    void ping_pong_framebuffer::render_to(GLuint i)
    {
        glBindTexture(GL_TEXTURE_2D, m_color_texture[1]);
        shader_program basic{ BTOLEDA_FILEPATH("/shaders/basic.vert.glsl"), BTOLEDA_FILEPATH("shaders/basic.frag.glsl") };
        glUseProgram(basic);
        //basic.set_uniform(uniform_type::INT, "u_texture", 0);
        glBindFramebuffer(GL_FRAMEBUFFER, i);
        GLfloat quadVertices[] = {
            // Positions   // TexCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
        };

        GLuint quadIndices[] = {
            0, 1, 2,
            2, 1, 3
        };

        GLuint quadVAO, quadVBO, quadEBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &quadEBO);
        
        glBindVertexArray(quadVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
} // btoleda