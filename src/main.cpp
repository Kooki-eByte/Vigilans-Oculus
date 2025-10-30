// Following a Vulkan tutorial guide to draw a triangle:
// https://vulkan-tutorial.com/resources/vulkan_tutorial_en.pdf

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

// TODO: Remove these globals into a better location later
VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice logicalDevice;
VkQueue graphicsQueue;

typedef struct vulk_extension_t {
  const char **extensions;
  size_t count;
} vulk_extension_t;
vulk_extension_t vulk_extension;

typedef struct queue_family_indices_t {
  u32 graphicsFamily;
} queue_family_indices_t;

// Queue Family
queue_family_indices_t *findQueueFamily(VkPhysicalDevice device) {
  queue_family_indices_t *indices = NULL;
  
  u32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);

  VkQueueFamilyProperties queue_families[queue_family_count] = {};
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

  bool queue_family_initialized = false;
  for (u32 i = 0; i < queue_family_count; i++) {
    if (indices != NULL) break;

    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      // Initialize to memory so it is no longer null and can add values to actual memory and not NULL
      if (!queue_family_initialized) {
        indices = (queue_family_indices_t *)malloc(sizeof(queue_family_indices_t));
        queue_family_initialized = true;
      }
      indices->graphicsFamily = i;
    }
  }

  return indices;
}
// End Queue Family section

// logical device
bool createLogicalDevice() {
  queue_family_indices_t *indices = findQueueFamily(physicalDevice);

  // Queue Create Info
  f32 queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_info = {};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = indices->graphicsFamily;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;
  
  // Get device properties for things like name, type, supported Vulkan version.
  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(physicalDevice, &device_props);
  // Support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR)
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(physicalDevice, &device_features);
  
  // logical queue
  VkDeviceCreateInfo logical_create_info = {};
  logical_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  logical_create_info.pQueueCreateInfos = &queue_create_info;
  logical_create_info.queueCreateInfoCount = 1;
  logical_create_info.pEnabledFeatures = &device_features;
  
  // No longer as of this link: https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/chap40.html#extendingvulkan-layers-devicelayerdeprecation
  // However to be compatible with older version of vulkan GPUs we will include it here.
  logical_create_info.enabledExtensionCount = 0;
  if (enableValidationLayers) {
    logical_create_info.enabledLayerCount = validationLayersLength;
    logical_create_info.ppEnabledLayerNames = validationLayers;
  } else {
    logical_create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &logical_create_info, 0, &logicalDevice) != VK_SUCCESS) {
    g_log_error("Failed to create logical device!");
    return false;
  }

  vkGetDeviceQueue(logicalDevice, indices->graphicsFamily, 0, &graphicsQueue);
  return true;
}

// End logical device

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
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  vulk_extension.count = glfw_extension_count;
  // TODO: Change to work with arena allocator later
  vulk_extension.extensions = (const char **)malloc(sizeof(const char *) * vulk_extension.count);
  if (!vulk_extension.extensions) {
    g_log_error("handle allocation failure!");
    return false;
  }
  memcpy(vulk_extension.extensions, glfw_extensions, sizeof(const char *) * vulk_extension.count);

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

GLFWwindow *initWindow(window_settings *ws) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  
  GLFWwindow *win = glfwCreateWindow(ws->width, ws->height, ws->title, 0, 0);

  if (win == NULL) {
    g_log_error("Window is null, after trying to create window!");
    return 0;
  }

  return win;
}

bool isDeviceSuitable(VkPhysicalDevice device) {
  // TODO: Put this in arena allocator to free later upon cleanup
  queue_family_indices_t *indices = findQueueFamily(device);
  if (indices == NULL) {
    g_log_error("No graphics family was found in given device!");
    free(indices);
    return false;
  }

  // Set this so that we only support GPUs with support for geometry shaders.
  // return device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader;
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
  if (enableValidationLayers) {
    if (!setupDebugMessenger()) {
      g_log_error("Failed to setup debug messenger!");
      return false;
    }
  }
  if (!pickPhysicalDevice()) {
    g_log_error("Failed to pick physical device!");
    return false;
  }
  if (!createLogicalDevice()) {
    g_log_error("Failed to create logical device!");
    return false;
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
  vkDestroyDevice(logicalDevice, 0);
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

  window = initWindow(&settings);

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