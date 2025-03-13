#pragma once

#include "rt_model.hpp"

#include <memory>

namespace rt
{

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f,1.f };
		float rotation;

		glm::mat2 mat2() 
		{ 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMatx{ {c,s},{-s,c} };

			glm::mat2 scaleMatx{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotationMatx * scaleMatx; 
		}
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
		Transform2dComponent transform2d;

	private:
		RtObject(id_t objId) : id{ objId } {}

		id_t id;
	};

}