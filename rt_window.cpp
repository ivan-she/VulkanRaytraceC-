#include "rt_window.hpp"

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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}