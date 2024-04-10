#include "window.h"
#include <stdexcept>

namespace btoleda {
	window::window(int width, int height, const std::string& title) : m_width(width), m_height(height), m_window{glfwCreateWindow(width, height, title.c_str(), NULL, NULL)}
	{
		if (!m_window.get())
		{
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwSetWindowSizeCallback(m_window.get(), [](GLFWwindow* window_, int width, int height) {
				using btoleda::window;
				window* self = static_cast<window*>(glfwGetWindowUserPointer(window_));
				if (self)
				{
					self->on_resize(width, height);
				}
			}
		);

		glfwSetKeyCallback(m_window.get(), [](GLFWwindow * window_, int key, int scancode, int action, int mods) {
			using btoleda::window;
			window* self = static_cast<window*>(glfwGetWindowUserPointer(window_));
			if (self)
			{
				self->on_keypress(key, action, mods);
			}
		}
		);
	}

	window::~window() = default;

	void window::set_resize_callback(const std::function<void(int width, int height)> &lambda)
	{
		m_on_resize = lambda;
	}

	void window::set_key_callback(const std::function<void(int key, int press_type)>& lambda)
	{
		m_on_keypress = lambda;
	}

	void window::on_resize(int width, int height)
	{
		m_width = width;
		m_height = height;
		glViewport(0, 0, width, height);
		m_on_resize(width, height);
	}

	void window::on_keypress(int key, int press_type, int mods)
	{
		m_on_keypress(key, press_type);
	}

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