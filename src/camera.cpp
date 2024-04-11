#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace btoleda {
	camera::camera(const glm::vec3& origin, const glm::vec3& lookat, const glm::vec3& up, const float fov, const float aspect_ratio) : m_origin{origin}, m_lookat{lookat}, m_up{up}, m_fov{glm::radians(fov)}, m_aspect_ratio(aspect_ratio), m_speed{1.5}, m_yaw{0.0f}, m_pitch{0.0f}
	{

	}

	camera::~camera() = default;
	
	glm::mat4 camera::view() const
	{
		return glm::lookAt(m_origin, m_origin + m_lookat, m_up);
	}

	glm::mat4 camera::perspective() const
	{
		return glm::perspective<float>(m_fov, m_aspect_ratio, 0.001, 1000.0);
	}

	void camera::move(Direction d, float distance)
	{
		if (d == Direction::FORWARD)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, m_lookat * distance) * glm::vec4{ m_origin, 1.0f };
		}
		else if (d == Direction::BACKWARD)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, -m_lookat * distance) * glm::vec4{ m_origin, 1.0f };
		}
		else if (d == Direction::LEFT)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, -glm::cross(m_lookat, m_up) * distance) * glm::vec4{ m_origin, 1.0f };
		}
		else if (d == Direction::RIGHT)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, glm::cross(m_lookat, m_up) * distance) * glm::vec4{ m_origin, 1.0f };
		}
		else if (d == Direction::UP)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, m_up * distance) * glm::vec4{ m_origin, 1.0f };
		}
		else if (d == Direction::DOWN)
		{
			m_origin = glm::translate(glm::mat4{ 1.0f }, -m_up * distance) * glm::vec4{ m_origin, 1.0f };
		}
	}

	void camera::rotate(double x_offset, double y_offset, double sensitivity)
	{
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		m_yaw += x_offset;
		m_pitch += y_offset;

		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		direction.y = sin(glm::radians(m_pitch));
		direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_lookat = glm::normalize(direction);
	}

} // btoleda