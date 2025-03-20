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

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),static_cast<uint32_t>(height) }; }

		bool wasWindowResized() {return framebufferResized;}
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow *getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void frameBfferResizedCallback(GLFWwindow* window, int width, int height);
		void initWindow();
		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;

		GLFWwindow* window;
	};
}