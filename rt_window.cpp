#include "rt_window.hpp"

#include <stdexcept>

namespace rt {
	RtWindow::RtWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName(name) 
	{ initWindow(); }

	RtWindow::~RtWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void RtWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBfferResizedCallback);
	}
	void RtWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a window surface");
		}
	}

	
	void RtWindow::frameBfferResizedCallback(GLFWwindow* window, int width, int height) 
	{
		auto rtWindow = reinterpret_cast<RtWindow*>(glfwGetWindowUserPointer(window));
		rtWindow->framebufferResized = true;
		rtWindow->width = width;
		rtWindow->height = height;

	}

}