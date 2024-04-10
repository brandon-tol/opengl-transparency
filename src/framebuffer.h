#pragma once
#include "GL/glew.h"

namespace btoleda {
	class framebuffer {
	public:
		framebuffer();
		framebuffer(int width, int height);
		~framebuffer();

		GLuint frame() const;
		GLuint depth_buffer() const;
		operator GLuint() const;
		void set_viewport(int width, int height);

	private:
		GLuint m_framebuffer_id;
		GLuint m_depth_buffer_id;
		GLuint m_render_texture_id;
	};
}