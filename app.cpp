#include "app.hpp"
#include "render_system.hpp"
#include <stdexcept>
#include <array>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>


namespace rt {



	App::App()
	{
		loadObjects();


	}

	App::~App(){}

	void App::run(){
		RenderSystem renderSystem{rtDevice, rtRenderer.getSwapChainRenderPass()};

		while (!rtWindow.shouldClose())
		{
			glfwPollEvents();
			if (auto commandBuffer = rtRenderer.beginFrame())
			{

				//Muokkaa jatkossa valokipoisuus ja postprosessointi tänne
				rtRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(commandBuffer,objects);
				rtRenderer.endSwapChainRenderPass(commandBuffer);
				rtRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(rtDevice.device());
	}


	void App::loadObjects()
	{
		std::vector<RtModel::Vertex> vertices{
			{{0.0f,-0.5f}, { 1.0f,0.0f,0.0f }},
			{{0.5f,0.5f}, { 0.0f,0.0f,1.0f }},
			{{-0.5f,0.5f}, { 0.0f,1.0f,0.0f }}
		};

		auto rtModel = std::make_shared<RtModel>(rtDevice, vertices);

		auto triangle = RtObject::createObject();
		triangle.model = rtModel;
		triangle.color = { 0.1f,0.1f,0.8f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.f,0.5f };
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

		objects.push_back(std::move(triangle));

	}


}