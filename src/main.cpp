#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>

#include <stdio.h>
#include <stdbool.h>
#include <cstdlib>
#include <cstring>
#include "defines.h"
#define GREED_IMPLEMENTATION
#include "../include/greed/greed.h"

// Following a Vulkan tutorial guide to draw a triangle:
// https://vulkan-tutorial.com/resources/vulkan_tutorial_en.pdf

// TODO: Remove these globals into a better location later
VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

typedef struct vulk_extension_t {
  const char **extensions;
  size_t count;
} vulk_extension_t;
vulk_extension_t vulk_extension;

// Proxy functions for debug messenger
VkResult createDebugUtilsMessengerEXT(VkInstance *pInstance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*pInstance, "vkCreateDebugUtilsMessengerEXT");
  if (func != 0) {
    return func(*pInstance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

static void destroyDebugUtilsMessengerEXT(VkInstance *pInstance, VkDebugUtilsMessengerEXT *pDebugMessenger, const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*pInstance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != 0) {
    func(*pInstance, *pDebugMessenger, pAllocator);
  }
}
// End of proxy functions

// Vulkan Debug Callback function
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  VkDebugUtilsMessageTypeFlagsEXT message_type,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void *pUserData
) {
  fprintf(stderr, "Validation layer: \n");
  switch (message_severity)
  {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      g_log_debug(pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      // TODO: Change to info (will need to make more logs in Greed lib)  
      g_log_success(pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      g_log_warning(pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      g_log_error(pCallbackData->pMessage);
      break;
    default:
      fprintf(stderr, "\t%s\n", pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo->pfnUserCallback = debugCallback;
  // createInfo->pUserData = stuff // parameter contains a pointer that allows you to pass your own data to it.
}

// TODO: Make sure when calling this function that we check if validation layer is enabled
bool setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  populateDebugMessengerCreateInfo(&createInfo);

  // TODO: Arena allocator setup - 3rd arg
  if (createDebugUtilsMessengerEXT(&instance, &createInfo, 0, &debugMessenger) != VK_SUCCESS) {
    g_log_error("Failed to set up debug messenger!");

    return false;
  }

  return true;
}


// end debug messenger code

// Validation layer
const char *validationLayers[] = {
  "VK_LAYER_KHRONOS_validation"
};

const u32 validationLayersLength = (u32)(sizeof(validationLayers) / sizeof(validationLayers[0]));

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

bool checkValidationLayerSupport(void) {
  u32 layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, 0);

  VkLayerProperties availableLayers[layerCount] = {};
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  for (u32 validationLayerIndex = 0; validationLayerIndex < validationLayersLength; validationLayerIndex++) {
    bool layerFound = false;

    for (u32 availLayerIndex = 0; availLayerIndex < layerCount; availLayerIndex++) {
      const VkLayerProperties layerProp = availableLayers[availLayerIndex];
      
      if (strcmp(validationLayers[validationLayerIndex], layerProp.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}
// End Validation layer

bool getRequiredExtensions(void) {
  u32 glfw_extension_count = 0;
  const char **glfw_extensions;

  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  vulk_extension.count = sizeof(const char *) * glfw_extension_count;
  // TODO: Change to work with arena allocator later
  vulk_extension.extensions = (const char **)malloc(vulk_extension.count);
  if (!vulk_extension.extensions) {
    g_log_error("handle allocation failure!");
    return false;
  }
  memcpy(vulk_extension.extensions, glfw_extensions, vulk_extension.count);

  if (enableValidationLayers) {
    const char *debug_util = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    size_t new_count = vulk_extension.count + 1;
    vulk_extension.extensions = (const char **)realloc(vulk_extension.extensions, sizeof(const char *) * new_count);

    vulk_extension.extensions[new_count - 1] = debug_util;
    vulk_extension.count = new_count;
  }

  return true;
}

typedef struct window_settings {
  int width;
  int height;
  const char *title;
} window_settings;

bool createInstance(void) {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    g_log_error("Validation layers requested, but not available!");
    return false;
  }

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



  u32 extCountForVulkan = 0;
  vkEnumerateInstanceExtensionProperties(0, &extCountForVulkan, 0);

  VkExtensionProperties extensions[extCountForVulkan] = {};
  vkEnumerateInstanceExtensionProperties(0, &extCountForVulkan, extensions);

  fprintf(stderr, "Available extensions:");
  for (u32 i = 0; i < extCountForVulkan; i++) {
    fprintf(stderr, "\t%s\n", extensions->extensionName);
  }

  // Useful glfw built-in function to get the required extensions for GLFW to work.
  if (!getRequiredExtensions()) {
    g_log_error("Failed to get the required extensions!");
    return false;
  }

  createInfo.enabledExtensionCount = (u32)vulk_extension.count;
  createInfo.ppEnabledExtensionNames = vulk_extension.extensions;

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = validationLayersLength;
    createInfo.ppEnabledLayerNames = (const char* const*)validationLayers;
  
    populateDebugMessengerCreateInfo(&debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
  
    createInfo.pNext = 0;
  }

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

bool isDeviceSuitable(VkPhysicalDevice device) {
  // Get device properties for things like name, type, supported Vulkan version.
  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(device, &deviceProps);
  // Support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR)
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  return true;
}

// Physical Device handling
bool pickPhysicalDevice() {
  u32 device_count = 0;
  vkEnumeratePhysicalDevices(instance, &device_count, 0);

  if (device_count == 0) {
    g_log_error("failed to find GPUs with Vulkan support!");
    return false;
  }
  
  VkPhysicalDevice devices[device_count] = {};
  vkEnumeratePhysicalDevices(instance, &device_count, devices);

  for (u32 i = 0; i < device_count; i++) {
    if (isDeviceSuitable(devices[i])) {
      physicalDevice = devices[i];
      break;
    } else {
      g_log_warning("device found is not suitable..");
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    g_log_error("Failed to find a suitable GPU!");
    return false;
  }

  return true;
}

bool initVulkan(void) {
  if (!createInstance()) {
    g_log_error("Failed to initialize Vulkan!");
    return false;
  }
  if (!setupDebugMessenger()) {
    g_log_error("Failed to setup debug messenger!");
  }
  if (!pickPhysicalDevice()) {
    g_log_error("Failed to pick physical device!");
  }

  return true;
}

void mainLoop(GLFWwindow *win) {
  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();
  }
}

void cleanup(GLFWwindow *win, const char **ext) {
  if (enableValidationLayers) {
    // TODO: Ensure you change the 3rd arg to the arena allocator 
    destroyDebugUtilsMessengerEXT(&instance, &debugMessenger, 0);
  }
  free(ext);
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

  cleanup(window, vulk_extension.extensions);

  return true;
}

int main(void) {
  if (!runApp(1280, 720, "Vigilans Oculus")) {
    g_log_error("App failed to run!");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}