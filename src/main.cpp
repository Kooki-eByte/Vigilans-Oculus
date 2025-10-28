#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <stdio.h>
#include "defines.h"

// Following a Vulkan tutorial guide to draw a triangle:
// https://vulkan-tutorial.com/resources/vulkan_tutorial_en.pdf

// Steps to draw a triangle with the Verbose API of vulkan.

// Create a VkInstance
// Select a supported graphics card (VkPhysicalDevice)
// Create a VkDevice and VkQueue for drawing and presentation
// Create a window, window surface and swap chain
// Wrap the swap chain images into VkImageView
// Create a render pass that specifies the render targets and usage
// Create framebuffers for the render pass
// Set up the graphics pipeline
// Allocate and record a command buffer with the draw commands for every possible swap chain image
// Draw frames by acquiring images, submitting the right draw command buffer and returning the images back to the swap chain

// Step 1: Instance and Physical device selection

// Step 2: Logical device and queue families 

// Step 3: Window surface and swap chain

// Step 4: Image views and framebuffers

// step 5: Render passes

// Step 6: Graphics pipeline

// Step 7: Command pools and command buffers

// Step 8: Main loop
int main(void) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Vigilans Oculus", 0, 0);

  u32 extenstionCount = 0;
  vkEnumerateInstanceExtensionProperties(0, &extenstionCount, 0);

  printf("%d extensions supported\n", extenstionCount);

  glm::mat4 matrix = {};
  glm::vec4 vec = {};
  auto test = matrix * vec;

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  for(int i = 0; i < 4; i++) {
    printf("%f\n", test[i]);
  }

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}