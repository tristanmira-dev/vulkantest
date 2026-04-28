#ifndef HELLOTRIANGLEAPPLICATION_HPP
#define HELLOTRIANGLEAPPLICATION_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <initializer_list>
#include <gameobjectspool.hpp>

#include "vertex.hpp"


struct Cam {
    glm::vec3 pos{ 0.f, 0.f, 0.f };
};


class HelloTriangleApplication {
public:
    void run();

private:

    const std::vector<char const*> validationLayers{
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char*> deviceExtensions = {
        vk::KHRSwapchainExtensionName /*extension of the gpu for presenting to the monitor*/
    };

    glm::mat4x4 proj;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    static constexpr int MAX_FRAMES_IN_FLIGHT{ 2 };

    bool frameBufferResized{ false };

    Cam cam;
    

    GLFWwindow* window{ nullptr };
    
    vk::raii::Context context;
    vk::raii::Instance instance{ nullptr };
    vk::raii::DebugUtilsMessengerEXT debugMessenger{ nullptr };

    /*Timing*/
        std::chrono::steady_clock::time_point startTime;
        float deltaTime{};

    /*Graphics card*/
        vk::raii::Device device{ nullptr }; /*Logical Device*/
        vk::raii::Queue graphicsQueue{ nullptr };
        vk::raii::Queue presentQueue{ nullptr };
        vk::raii::PhysicalDevice physicalDevice{ nullptr };
        vk::raii::SurfaceKHR surface{ nullptr };
        uint32_t queueIdx = ~0;

    
    /*Swap chain*/
        vk::raii::SwapchainKHR swapChain{ nullptr };
        std::vector<vk::Image> swapChainImages;
        vk::SurfaceFormatKHR swapChainSurfaceFormat;
        vk::Extent2D swapChainExtent;
    
    /*Image Views*/
        std::vector<vk::raii::ImageView> swapChainImageViews;

    /*Pipeline*/
        vk::raii::PipelineLayout pipelineLayout{ nullptr };
        vk::raii::Pipeline pipeline{ nullptr };
        vk::raii::DescriptorSetLayout descriptorSetLayout{ nullptr };
    /*Second Pipeline*/
        vk::raii::PipelineLayout pipelineLayout2{ nullptr };
        vk::raii::Pipeline pipeline2{ nullptr };

    /*Command buffer object*/
        vk::raii::CommandPool commandPool{ nullptr };
        std::vector<vk::raii::CommandBuffer> commandBuffers;

    /*Frames in flight*/
        uint32_t frameIdx{};

    /*Synchronization objects*/
    /*Semaphores for now are mainly for swapchain operations, and Fences are mainly for drawing frames (do not want to override the command buffers before the previous drawFrame calls while the gpu is still processing it)*/
        std::vector<vk::raii::Semaphore> presentCompleteSemaphore;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphore;
        std::vector<vk::raii::Fence> inFlightFences;

    GameObjectPool gameObjects;


    void readVertices(std::initializer_list<Vertex> verticeValues);

    void createInstance();
    
    void initVulkan();
    
    void pickPhysicalDevice();
    
    void createSurface();

    void createLogicalDevice();

    void setupDebugMessenger();

    void recreateSwapChain();

    void cleanupSwapChain();

    void initWindow();
    
    void mainLoop();
    
    void cleanup();

    void createSwapChain();

    void createImageView();

    void drawFrame();

    void createGraphicsPipeline();

    vk::raii::ShaderModule createShaderModule(std::vector<char> const& shader) const;

    void secondPipeline();

    void createSyncObjects();

    float angleX{};
    float angleY{};

    void createDescriptorSetLayout();

    /*Command Buffer Stuff*/
        void createCommandPool();
        void createCommandBuffer();
        void recordCommandBuffer(uint32_t imageIdx);
        void transition_image_layout(vk::Image const &image, vk::ImageLayout oldLayout, vk::ImageLayout imageLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspectFlag);

    /*BUFFERS*/
        /*Vertex Buffers*/
        vk::raii::Buffer vertexBuffer{ nullptr };
        vk::raii::DeviceMemory vertexBufferMemory{ nullptr };
        /*index Buffers*/
        vk::raii::Buffer indexBuffer{ nullptr };
        vk::raii::DeviceMemory indexBufferMemory{ nullptr };
        /*Uniform Buffers*/
        std::vector<vk::raii::Buffer> uniformBuffer;
        std::vector<vk::raii::DeviceMemory> uniformBufferMemory;
        std::vector<void*> mappedData;

        vk::raii::DescriptorPool descriptorPool{ nullptr };

        std::vector<vk::raii::DescriptorSet> descriptorSets;
        std::vector<vk::raii::Buffer> uniformBuffer2;
        std::vector<vk::raii::DeviceMemory> uniformBufferMemory2;
        std::vector<void*> mappedData2;

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory);
        void copyBuffer(vk::raii::Buffer& stagingBuffer, vk::raii::Buffer& vertexBuffer, vk::DeviceSize size);
        void createVertexBuffer();
        void createDescriptorPool();
        void createDescriptorSets();
        void createIndexBuffer();
        void createUniformBuffers();
        void updateUniformBuffer(uint32_t currentImg);

    /*Textures*/
        void createTextureImage();

        void createTextureImageView();

        void createTextureSampler();

        vk::raii::Image textureImage{ nullptr };
        vk::raii::Sampler textureSampler{ nullptr };
        vk::raii::ImageView textureImageView{ nullptr };
        vk::raii::DeviceMemory textureImageMemory{ nullptr };

        vk::raii::CommandBuffer beginSingleTimeCommand();
        void transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

        /*invokes submitting to the graphics queue and waitIdle*/
        void endSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer);

        void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image, vk::raii::DeviceMemory& deviceMemory);
        void copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);

    /*Depth*/
        vk::raii::Image depthImage{ nullptr };
        vk::raii::ImageView depthImageView{ nullptr };
        vk::raii::DeviceMemory depthImageDeviceMemory{ nullptr };
        vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
        vk::Format findDepthFormat();
        bool hasStencilComponent(vk::Format format);
        void createDepthResource();

        vk::raii::ImageView createImageViewHelper(vk::raii::Image& image, vk::Format const& format, vk::ImageAspectFlags aspectFlags);

    /*Load Model*/
        void loadModel();
};


#endif