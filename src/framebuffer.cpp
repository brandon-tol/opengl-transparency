#include "framebuffer.h"
#include <stdexcept>

namespace btoleda {
	framebuffer::framebuffer(int width, int height)
	{
		glGenFramebuffers(1, &m_framebuffer_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);

		// Frame render
		glGenTextures(1, &m_render_texture_id);
		glBindTexture(GL_TEXTURE_2D, m_render_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_render_texture_id, 0);

		// Depth buffer
		glGenTextures(1, &m_depth_buffer_id);
		glBindTexture(GL_TEXTURE_2D, m_depth_buffer_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_buffer_id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	framebuffer::framebuffer()
	{
		glGenFramebuffers(1, &m_framebuffer_id);
	}

	framebuffer::~framebuffer()
	{
		glDeleteTextures(1, &m_render_texture_id);
		glDeleteTextures(1, &m_depth_buffer_id);
		glDeleteFramebuffers(1, &m_framebuffer_id);
	}

	GLuint framebuffer::frame() const
	{
		return m_render_texture_id;
	}

	GLuint framebuffer::depth_buffer() const
	{
		return m_depth_buffer_id;
	}

	framebuffer::operator GLuint() const
	{
		return m_framebuffer_id;
	}

	void framebuffer::set_viewport(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
		GLuint updated_render_id, updated_depth_id;
		// Frame render
		glGenTextures(1, &updated_render_id);
		glBindTexture(GL_TEXTURE_2D, updated_render_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, updated_render_id, 0);

		// Depth buffer
		glGenTextures(1, &updated_depth_id);
		glBindTexture(GL_TEXTURE_2D, updated_depth_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, updated_depth_id, 0);

		glDeleteTextures(1, &m_depth_buffer_id);
		glDeleteTextures(1, &m_render_texture_id);

		m_render_texture_id = updated_render_id;
		m_depth_buffer_id = updated_depth_id;

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}