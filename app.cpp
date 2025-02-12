#include "app.hpp"

namespace rt {
	void App::run(){
	while (!rtWindow.shouldClose())
	{
		glfwPollEvents();
}
}
}