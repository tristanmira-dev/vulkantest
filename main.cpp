#include "HelloTriangleApplication.hpp"

#include <iostream>
#include <cstdlib>


int main() {
    try {
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cout << e.what();
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
