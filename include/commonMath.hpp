#ifndef COMMONMATH_HPP
#define COMMONMATH_HPP


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

glm::mat4 projection(GLFWwindow*& window, float fovDegrees, float n, float f);


#endif