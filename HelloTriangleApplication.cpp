#include <stdexcept>
#include <vector>


#include "HelloTriangleApplication.hpp"
#include "extensions.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#if NDEBUG
    constexpr bool enableValidationLayers{ false };
#else
    constexpr bool enableValidationLayers{ true };
#endif

constexpr int WIDTH{ 800 };
constexpr int HEIGHT{ 600 };


namespace {
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
}

void HelloTriangleApplication::run() {
    initWindow();
    initVulkan();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    //instance.submitDebugUtilsMessageEXT(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
    //    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral, vk::DebugUtilsMessengerCallbackDataEXT{ .pMessage = physicalDevice.getProperties().deviceName });
    createLogicalDevice();
    createSwapChain(); /*7th and counting, MAN OH MAN*/
    mainLoop();
    cleanup();
}

/*3rd - check glfw extension requirements, debug extensions, and create instance*/
void HelloTriangleApplication::createInstance() {
    std::vector<char const*> requiredLayers;
    if (enableValidationLayers) requiredLayers.assign(validationLayers.begin(), validationLayers.end());

    auto layerProps{ context.enumerateInstanceLayerProperties() };

    /*Check if layer is supported*/
    auto layerIT {
        std::ranges::find_if(requiredLayers, [&layerProps](const char* const &constlayer) {
            return std::ranges::none_of(layerProps, [&constlayer](vk::LayerProperties const& props) {
                return std::strcmp(constlayer, props.layerName) == 0;
            });
        })
    };

    if (layerIT != requiredLayers.end()) {
        throw std::runtime_error{ *layerIT };
    }

    constexpr vk::ApplicationInfo appInfo{
        .pApplicationName = "Tri", .applicationVersion = VK_MAKE_VERSION(1,0,0), .pEngineName = "No Engine", .engineVersion = VK_MAKE_VERSION(1,0,0), .apiVersion = vk::ApiVersion14
    };

    /*Mostly glfw extensions to compare against vulkan supported extensions*/
    std::vector<const char*> requiredExtensions{ getRequiredInstanceExtensions() };

    /*Get current extensions supported by vulkan*/
    auto extensionProps{ context.enumerateInstanceExtensionProperties() };

    auto requiredExtIT{
        std::ranges::find_if(requiredExtensions, [&extensionProps](const char * const &required) {
            return std::ranges::none_of(extensionProps, [&required](vk::ExtensionProperties const &extProps) {
                return std::strcmp(required, extProps.extensionName) == 0;
            });
        })
    };

    if (requiredExtIT != requiredExtensions.end()) {
        throw std::runtime_error{ *requiredExtIT };
    }

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    instance = vk::raii::Instance{ context, createInfo };
}

/*2nd*/
void HelloTriangleApplication::initVulkan() {
    createInstance();
}

/*5th*/
void HelloTriangleApplication::createSurface() {
    //VkWin32SurfaceCreateInfoKHR createInfo{};

    
    VkSurfaceKHR _surface;
    
    /*if you want to do it natively, glfw covers this already*/
    //createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    //createInfo.hwnd = glfwGetWin32Window(window);
    //createInfo.hinstance = GetModuleHandle(nullptr);

    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != VK_SUCCESS) throw std::runtime_error{ "Where the surfaces at!?!??!?" };

    surface = vk::raii::SurfaceKHR{ instance, _surface };
}

/*7th*/
void HelloTriangleApplication::createLogicalDevice() {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicsQueueFamilyProperty{ std::ranges::find_if(queueFamilyProperties, [](const vk::QueueFamilyProperties &props) {
        return (props.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    })};

    uint32_t graphicsIdx{static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty))};

    auto presentIndex{ physicalDevice.getSurfaceSupportKHR(graphicsIdx, *surface) ? graphicsIdx : static_cast<uint32_t>(queueFamilyProperties.size()) };

    /*if index(currentFamily that supports graphics) that was found using the find_if cannot support the current surface*/
    if (presentIndex == queueFamilyProperties.size()) {

        /*Search for families that support both present(surface) and graphics*/
        for (std::size_t i{}; i < queueFamilyProperties.size(); ++i) {
            if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(i, *surface)) {
                graphicsIdx = static_cast<uint32_t>(i);
                presentIndex = static_cast<uint32_t>(i);
                break;
            }
        }

        /*Search for a different family that supports surface*/
        if (presentIndex == queueFamilyProperties.size()) {
            for (std::size_t i{}; i < queueFamilyProperties.size(); ++i) {
                if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface)) {
                    presentIndex = static_cast<uint32_t>(i);
                    break;
                }
            }
        }
    }


    if (( graphicsIdx == static_cast<uint32_t>(queueFamilyProperties.size()) ) || ( presentIndex == static_cast<uint32_t>(queueFamilyProperties.size()) )) {
        throw std::runtime_error{ "No appropriate graphics or surface compatible queue found" };
    }

    auto features{ physicalDevice.getFeatures2() };
    vk::PhysicalDeviceVulkan13Features vulkan13Features;
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
    vulkan13Features.dynamicRendering = vk::True; /*skip boilerplate rendering setup*/
    extendedDynamicStateFeatures.extendedDynamicState = vk::True; /*Change pipeline settings without recreating*/
    vulkan13Features.pNext = &extendedDynamicStateFeatures;
    features.pNext = &vulkan13Features;

    /*Create the device*/
    float queuePrio{ 0.5f };
    /*->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>IMPORTANT*/
    vk::DeviceQueueCreateInfo queueCreateInfo{ .queueFamilyIndex = graphicsIdx /*come back to this later, cuz what if we have two different indexes for present and graphics*/, .queueCount = 1, .pQueuePriorities = &queuePrio};
    vk::DeviceCreateInfo deviceCreateInfo{ .pNext = &features, .queueCreateInfoCount = 1, .pQueueCreateInfos = &queueCreateInfo };

    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    graphicsQueue = vk::raii::Queue(device, graphicsIdx, 0);
    presentQueue = vk::raii::Queue(device, presentIndex, 0);


}

/*6th*/
void HelloTriangleApplication::pickPhysicalDevice() {

    auto devices{instance.enumeratePhysicalDevices()};
    const auto devicesIter{
    
        std::ranges::find_if(devices, [&](vk::raii::PhysicalDevice const &device) {
            auto queueFamilies{device.getQueueFamilyProperties()};
            bool isSuitable{ device.getProperties().apiVersion >= VK_API_VERSION_1_3 };

            /*In a gpu, there are different 'families' called queue families that deal with different tasks. think of it like individual factories that are in charge of producing different items*/
            const auto qfpIter{
                std::ranges::find_if(queueFamilies, [](vk::QueueFamilyProperties const &queueProps) {
                    return (queueProps.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); /*just to make sure the graphics part of the family is supported*/
                })
            };

            isSuitable = isSuitable && (qfpIter != queueFamilies.end());

            auto extensions = device.enumerateDeviceExtensionProperties();
            bool found{ true };

            /*Just seeing if the current physical device in the iteration can accomodate presenting on the monitor*/
            for ( char const * const &extension : deviceExtensions) {
                auto extensionIter{ std::ranges::find_if(extensions, [extension](vk::ExtensionProperties const &props) {
                    return std::strcmp(extension, props.extensionName) == 0;
                })};

                found = found && extensionIter != extensions.end();
            }

            isSuitable = found && isSuitable;

            return isSuitable;
        })
    
    };

    if (devices.empty() || (devicesIter == devices.end())) { throw std::runtime_error{"Mate you don't even have a proper GPU"}; }
    physicalDevice = *devicesIter;


}

/*4th*/
void HelloTriangleApplication::setupDebugMessenger() {
    if (!enableValidationLayers) { return; }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    };

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags {
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    };

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback
    };

    debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

/*1st*/
void HelloTriangleApplication::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); /*Dont create an opengl context, by default glfw was made for opengl*/
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /*Disables resizing for now....*/
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void HelloTriangleApplication::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

/*8th*/
void HelloTriangleApplication::createSwapChain() {
    auto surfaceCapabilities{ physicalDevice.getSurfaceCapabilitiesKHR(*surface) };


    //std::vector<vk::SurfaceFormatKHR> availableFormats{ physicalDevice.getSurfaceCapabilitiesKHR(*surface) };

    //std::vector<vk::PresentModeKHR> availablePresentModes{ physicalDevice.getSurfacePresentModesKHR(*surface) };
}

namespace {
    /*NON CLASS MEMBER FUNCTIONS!*/
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {

        for (vk::SurfaceFormatKHR const& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
}