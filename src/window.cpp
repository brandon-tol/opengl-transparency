#include "window.h"
#include <stdexcept>

namespace btoleda {
	window::window(int width, int height, const std::string& title) : m_width(width), m_height(height), m_window{glfwCreateWindow(width, height, title.c_str(), NULL, NULL)}
	{
		if (!m_window.get())
		{
			throw std::runtime_error("Failed to create GLFW window");
		}
	}

	window::~window() = default;

	int window::height() const
	{
		return m_height;
	}

	int window::width() const
	{
		return m_width;
	}

	float window::aspect_ratio() const
	{
		return (float)m_width / m_height;
	}
}