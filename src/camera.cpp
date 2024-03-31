#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace btoleda {
	camera::camera(const glm::vec3& origin, const glm::vec3& target, const glm::vec3& up, const float fov, const float aspect_ratio) : m_origin{origin}, m_target{target}, m_up{up}, m_fov{glm::radians(fov)}, m_aspect_ratio(aspect_ratio)
	{

	}

	camera::~camera() = default;
	
	glm::mat4 camera::view() const
	{
		return glm::lookAt(m_origin, m_target, m_up);
	}

	glm::mat4 camera::perspective() const
	{
		// TODO: fix hardcoded far plane
		return glm::perspective<float>(m_fov, m_aspect_ratio, 0.001, 1000.0);
	}

} // btoleda