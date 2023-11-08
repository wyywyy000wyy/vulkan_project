#include "vklib_app.h"
#include "vklib.h"
#include <GLFW/glfw3.h>
#include <iostream>

vklibLogger* logger = nullptr;

class vklibLoggerImpl : public vklibLogger
{
public:
    void Log(const char* message)
    {
        std::cout<<message<<std::endl;
    }
    void LogWarning(const char* message)
    {
        std::cerr << message << std::endl;
    }
    void LogError(const char* message)
    {
        std::cerr << message << std::endl;
    }
};

class vklib_glwf3 :public vklib
{
public:
    vklib_glwf3(GLFWwindow* window)
    {
        this->window = window;
	}
    GLFWwindow* window;
protected:
    void createSurface()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            VK_LOG_ERROR("failed to create window surface!");
			throw std::runtime_error("failed to create window surface!");
		}
	}
};


vklib_app::vklib_app(uint32_t width, uint32_t height)
{
	WIDTH = width;
	HEIGHT = height;

    if(logger == nullptr)
	logger = new vklibLoggerImpl();
}

void vklib_app::_run()
{
    try {
        while (!glfwWindowShouldClose(window)) {
            loop();
            glfwPollEvents();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


void vklib_app::run()
{
	init();
    _run();
    cleanup();
}

void vklib_app::init()
{
    initWindow();
    vklib = new vklib_glwf3(window);

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    std::vector<const char*> deviceExtensions;
    vklib->initVulkan(extensions, deviceExtensions);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<vklib_app*>(glfwGetWindowUserPointer(window));
    app->vklib->windSizeChanged();
}

void vklib_app::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void vklib_app::loop()
{
	vklib->draw();
}

void vklib_app::cleanup()
{
    vklib->cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
}
