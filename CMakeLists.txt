cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_MODULE_STD 1)
project(LEARNVULKAN)



list(APPEND CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake/modules")

find_package (Vulkan REQUIRED)
find_package (glfw3 REQUIRED)


include(${CMAKE_SOURCE_DIR}/cmake/utils/compileShader.cmake)

# # set up Vulkan C++ module (enabled when ENABLE_CPP20_MODULE=ON)
# add_library(VulkanCppModule)
# add_library(Vulkan::cppm ALIAS VulkanCppModule)

# target_compile_definitions(VulkanCppModule PUBLIC
#         VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
#         VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
# )

# target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

# target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

# set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

# target_sources(VulkanCppModule
#         PUBLIC
#         FILE_SET cxx_modules TYPE CXX_MODULES
#         BASE_DIRS "${Vulkan_INCLUDE_DIR}"
#         FILES
#         "${Vulkan_INCLUDE_DIR}/vulkan/vulkan_video.cppm"
#         "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
# )


# Your shader source files
set(SHADER_SLANG_SOURCES ${CMAKE_SOURCE_DIR}/shaders/vertAndFrag.slang)


add_subdirectory(utils)
add_subdirectory(io)


add_slang_shader_target(mainShaders SOURCES ${SHADER_SLANG_SOURCES})

add_executable(mainStuff main.cpp)
add_dependencies(mainStuff mainShaders)

target_compile_definitions(mainStuff PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS) #CPP 20 feature
target_compile_definitions(mainStuff PRIVATE VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS)
target_compile_definitions(mainStuff PRIVATE VK_USE_PLATFORM_WIN32_KHR GLFW_INCLUDE_VULKAN)
target_compile_definitions(mainStuff PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

#Headers for root
target_include_directories(mainStuff PRIVATE include)

#EXTRA SOURCES
target_sources(mainStuff PRIVATE extensions.cpp HelloTriangleApplication.cpp graphicspipeline.cpp commandpool.cpp draw.cpp vertex.cpp buffers.cpp commonMath.cpp)

target_link_libraries(mainStuff PRIVATE Vulkan::Vulkan)
target_link_libraries(mainStuff PRIVATE glfw)
target_link_libraries(mainStuff PRIVATE utils)
target_link_libraries(mainStuff PRIVATE io)