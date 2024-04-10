#pragma once

#include <glm/glm.hpp>

namespace btoleda
{
	enum Direction
	{
		NONE = 0,
		LEFT,
		RIGHT,
		DOWN,
		UP,
		FORWARD,
		BACKWARD
	};

	class camera
	{
	public:
		camera(const glm::vec3&, const glm::vec3&, const glm::vec3&, const float fov, const float aspect_ratio);
		~camera();

		glm::mat4 view() const;
		glm::mat4 perspective() const;
		void move(Direction d, float distance);
		inline float speed() const
		{
			return m_speed;
		}

		inline void set_speed(float speed)
		{
			m_speed = speed;
		}

	private:
		glm::vec3 m_origin;
		glm::vec3 m_lookat;
		glm::vec3 m_up;
		float m_fov;
		float m_aspect_ratio;
		float m_speed;
	};
}