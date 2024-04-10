#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

namespace btoleda {

	struct GLFWwindowDeleter {
		void operator()(GLFWwindow* ptr) const
		{
			glfwDestroyWindow(ptr);
		}
	};

	class window {
	public:
		window(int width, int height, const std::string &title);
		int height() const;
		int width() const;
		float aspect_ratio() const;
		~window();

		operator GLFWwindow* ()
		{
			return m_window.get();
		}

	private:

		std::unique_ptr <GLFWwindow, GLFWwindowDeleter> m_window;
		int m_width, m_height;
	};
}