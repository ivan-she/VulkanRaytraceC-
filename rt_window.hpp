#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace rt
{
	class RtWindow
	{
	public:
		RtWindow(int w, int h, std::string name);
		~RtWindow();

		RtWindow(const RtWindow&) = delete;
		RtWindow& operator=(const RtWindow&) = delete;


		bool shouldClose() { return glfwWindowShouldClose(window); }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		
		void initWindow();
		const int width;
		const int height;

		std::string windowName;

		GLFWwindow* window;
	};
}