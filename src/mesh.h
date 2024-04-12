#pragma once

#include <vector>
#include <GL/glew.h>

namespace btoleda {

	/**
	* btoleda::mesh assumes the vertex data is specified as
	* X, Y, Z, coords from 0.0f to 1.0f
 	* X, Y, Z, normal coords from 0.0f to 1.0f
	* R, G, B, A colors from 0.0f to 1.0f
	*/
	class mesh
	{
	public:
		mesh(const std::vector<GLfloat>&, const std::vector<GLuint>&, bool textured);
		~mesh();
		operator GLuint() const;

	private:
		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_ebo;
		bool m_textured;

	public:
		const size_t size;
	};
}

