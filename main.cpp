#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "utils.hpp"
#include "extensions.hpp"

const std::vector<char const*> validationLayers {
    "VK_LAYER_KHRONOS_validation"
};


#if NDEBUG
    constexpr bool enableValidationLayers{ false };
#else
    constexpr bool enableValidationLayers{ true };
#endif

constexpr int WIDTH{ 800 };
constexpr int HEIGHT{ 600 };

class HelloTriangleApplication {
    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        GLFWwindow* window{ nullptr };
        vk::raii::Context context;
        vk::raii::Instance instance{ nullptr };
        vk::raii::DebugUtilsMessengerEXT debugMessenger{ nullptr };

        void createInstance() {
            /*Validation layers*/
            std::vector<char const*> requiredLayers;
            if (enableValidationLayers) requiredLayers.assign(validationLayers.begin(), validationLayers.end());

            auto layerProps{ context.enumerateInstanceLayerProperties() };

            auto layerIT {
                /*Basically just find the layer name in requiredLayer that couldnt be found in vulkan*/
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
            
            // Get the required instance extensions from GLFW.
            std::vector<const char*> requiredExtensions{ getRequiredInstanceExtensions() };

            /*get the master list of current extensions from VULKAN*/
            auto extensionProps{context.enumerateInstanceExtensionProperties()};

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

            ///*Not optional*/
            vk::InstanceCreateInfo createInfo{ 
                .pApplicationInfo = &appInfo, 
                .enabledLayerCount=static_cast<uint32_t>(requiredLayers.size()), 
                .ppEnabledLayerNames = requiredLayers.data(), 
                .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()), 
                .ppEnabledExtensionNames = requiredExtensions.data()
            };

            instance = vk::raii::Instance{ context, createInfo };
       
        }

        void initVulkan() {
            createInstance();
            setupDebugMessenger();
        }

        void setupDebugMessenger() {
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

        void initWindow() {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        }

        void mainLoop() {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
            }
        }

        void cleanup() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
};

int main() {





    //vk::raii::Context ctx;

    //instance = vk::raii::Instance(ctx, createInfo);

    try {
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cout << e.what();
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    //pause();

    return EXIT_SUCCESS;
}