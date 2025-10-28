#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>

#include <stdio.h>
#include <stdbool.h>
#include <cstdlib>
#include "defines.h"
#define GREED_IMPLEMENTATION
#include "../include/greed/greed.h"

// Following a Vulkan tutorial guide to draw a triangle:
// https://vulkan-tutorial.com/resources/vulkan_tutorial_en.pdf

// TODO: Remove these globals into a better location later
static VkInstance instance;

typedef struct window_settings {
  int width;
  int height;
  const char *title;
} window_settings;

bool createInstance() {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Vigilans Oculus";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Eureka";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  u32 glfwExtensionCount = 0;
  const char **glfwExtensions;

  u32 extCountForVulkan = 0;
  vkEnumerateInstanceExtensionProperties(0, &extCountForVulkan, 0);

  VkExtensionProperties extensions[extCountForVulkan] = {};
  vkEnumerateInstanceExtensionProperties(0, &extCountForVulkan, extensions);

  fprintf(stderr, "Available extensions:");
  for (u32 i = 0; i < extCountForVulkan; i++) {
    fprintf(stderr, "\t%s\n", extensions->extensionName);
  }

  // Useful glfw built-in function to get the required extensions for GLFW to work.
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  // Last two attributes are for validation layer
  createInfo.enabledLayerCount = 0;

  // TODO: 2nd arg will be for arena allocator later on.
  if (vkCreateInstance(&createInfo, 0, &instance) != VK_SUCCESS) {
    g_log_error("Failed to create vulkan instance!");
    return false;
  }

  return true;
}

bool initWindow(GLFWwindow *win, window_settings *ws) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  
  win = glfwCreateWindow(ws->width, ws->height, ws->title, 0, 0);

  if (win == NULL) {
    g_log_error("Window is null, after trying to create window!");
    return false;
  }

  return true;
}

bool initVulkan() {
  if (!createInstance()) {
    g_log_error("Faield to initialize Vulkan!");
    return false;
  } 
  return true;
}

void mainLoop(GLFWwindow *win) {
  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();
  }
}

void cleanup(GLFWwindow *win) {
  // TODO: Ensure you change the 2nd arg to the free arena allocator 
  vkDestroyInstance(instance, 0);

  glfwDestroyWindow(win);
  glfwTerminate();
}

bool runApp(int width, int height, const char *window_name) {
  GLFWwindow *window = {};
  
  window_settings settings = {
    .width = width,
    .height = height,
    .title = window_name
  };

  if (!initWindow(window, &settings)) {
    g_log_error("The GLFW Window failed to initialize!");
    return false;
  }

  if (!initVulkan()) {
    g_log_error("Failed to initialize Vulkan!");
    return false;
  }

  mainLoop(window);

  cleanup(window);

  return true;
}

int main(void) {
  if (!runApp(1280, 720, "Vigilans Oculus")) {
    g_log_error("App failed to run!");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}