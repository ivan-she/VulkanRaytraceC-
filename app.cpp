#include "app.hpp"

#include "keybord_movement_control.hpp"
#include "rt_camera.hpp"
#include "render_system.hpp"
#include "rt_buffer.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>


namespace rt {
    struct GlobalUbo
    {
        alignas(16) glm::mat4 projectionView{ 1.f };
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,-3.f,-1.f });
    };


	App::App()
	{
        globalPool = RtDescriptorPool::Builder(rtDevice).setMaxSets(RtSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RtSwapChain::MAX_FRAMES_IN_FLIGHT).build();
		loadObjects();
	}

	App::~App(){}

    void App::run() {


        std::vector<std::unique_ptr<RtBuffer>> uboBuffers(RtSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<RtBuffer>(
                rtDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);/* | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)*/
            uboBuffers[i]->map();
        }

        auto globalSetLayout = RtDescriptorSetLayout::Builder(rtDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).build();

        std::vector<VkDescriptorSet> globalDescriptorSets(RtSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            RtDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
        }


        RenderSystem renderSystem{ rtDevice, rtRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        RtCamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewObject = RtObject::createObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!rtWindow.shouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime,"predifined value here")

            cameraController.moveInPlaneXZ(rtWindow.getGLFWwindow(), frameTime, viewObject);

            camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);

            float aspect = rtRenderer.getAspectRation();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,10.f);
			if (auto commandBuffer = rtRenderer.beginFrame())
			{
                int frameIndex = rtRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };
                //update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render
				//Muokkaa jatkossa valokipoisuus ja postprosessointi tänne
				rtRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(frameInfo,objects);
				rtRenderer.endSwapChainRenderPass(commandBuffer);
				rtRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(rtDevice.device());
	}



	void App::loadObjects()
	{
        std::shared_ptr<RtModel> rtModel = RtModel::creatModelFromFile(rtDevice, "Models/flat_vase.obj");

        auto obj = RtObject::createObject();
        obj.model = rtModel;
        obj.transform.translation = { .0f,.5f,2.5f };
        obj.transform.scale = glm::vec3{ 3.f };
        objects.push_back(std::move(obj));

	}


}