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

		void set_resize_callback(const std::function<void(int width, int height)> &lambda);
		void on_resize(int width, int height);

		void set_key_callback(const std::function<void(int key, int press_type)> &lambda);
		void on_keypress(int key, int press_type, int mods);

		operator GLFWwindow* ()
		{
			return m_window.get();
		}

	private:

		std::unique_ptr <GLFWwindow, GLFWwindowDeleter> m_window;
		int m_width, m_height;
		std::function<void(int width, int height)> m_on_resize;
		std::function<void(int key, int press_type)> m_on_keypress;
	};
}