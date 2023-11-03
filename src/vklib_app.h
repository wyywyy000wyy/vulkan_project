#pragma once
#include "vklib.h"
#include <GLFW/glfw3.h>

class vklib_app
{
public:
	vklib_app(uint32_t width, uint32_t height);

	void run();
protected:
	virtual void init();
	virtual void loop();
	virtual void cleanup();
	void initWindow();
	void _run();

	GLFWwindow* window;
	uint32_t WIDTH;
	uint32_t HEIGHT;
	vklib* vklib;
};
