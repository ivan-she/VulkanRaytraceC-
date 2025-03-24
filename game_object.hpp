#pragma once

#include "rt_model.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

namespace rt
{

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f,1.f ,1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};


	class RtObject
	{
	public:
		using id_t = unsigned int;

		static RtObject createObject()
		{
			static id_t currentId = 0;
			return RtObject(currentId++);
		}

		RtObject(const RtObject&) = delete;
		RtObject& operator=(const RtObject&) = delete;
		RtObject(RtObject&&) = default;
		RtObject& operator=(RtObject&&) = default;

		const id_t getId()
		{
			return id;
		}

		std::shared_ptr<RtModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		RtObject(id_t objId) : id{ objId } {}

		id_t id;
	};

}