#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace btoleda {
	camera::camera(const glm::vec3& origin, const glm::vec3& lookat, const glm::vec3& up, const float fov, const float aspect_ratio) : m_origin{origin}, m_lookat{lookat}, m_up{up}, m_fov{glm::radians(fov)}, m_aspect_ratio(aspect_ratio), m_speed{1.5}
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

} // btoleda