#pragma once
#include <GL/glew.h>
#include <stdexcept>

namespace btoleda {
	class ping_pong_framebuffer {
	public:
		inline ping_pong_framebuffer(GLsizei width, GLsizei height)
		{
			glGenFramebuffers(2, m_framebuffer);
			glGenTextures(2, m_depth_texture);
			glGenTextures(2, m_color_texture);

			for(int i = 0; i < 2; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer[i]);

				glBindTexture(GL_TEXTURE_2D, m_color_texture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture[i], 0);

				glBindTexture(GL_TEXTURE_2D, m_depth_texture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture[i], 0);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
				}
			}

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		inline ~ping_pong_framebuffer()
		{
			glDeleteTextures(2, m_depth_texture);
			glDeleteTextures(2, m_color_texture);
			glDeleteFramebuffers(2, m_framebuffer);
		}

		inline GLuint framebuffer_id(int i)
		{
			return m_framebuffer[i % 2];
		}

		inline GLuint depth_texture_id(int i)
		{
			return m_depth_texture[i % 2];
		}

		inline GLuint color_texture_id(int i)
		{
			return m_color_texture[i % 2];
		}

		void render_to(GLuint i);

	private:
		GLuint m_framebuffer[2];
		GLuint m_depth_texture[2];
		GLuint m_color_texture[2];
	};
} // btoleda