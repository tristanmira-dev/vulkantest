#include "commonMath.hpp"

glm::mat4 projection(GLFWwindow*& window, float fovDegrees, float n, float f) {
	 int width, height;
	 glfwGetWindowSize(window, &width, &height);

	 float aspect{ static_cast<float>(width) / height };


	 float tan{ glm::tan(glm::radians(fovDegrees / 2.f)) };

	 glm::mat4 projectionMtx{ glm::identity<glm::mat4>() };

	 projectionMtx[0][0] = 1 / (aspect * tan);

	 projectionMtx[1][1] = -1 / tan;

	 projectionMtx[2][2] = f / (f - n);

	 projectionMtx[2][3] = (-n * f) / (f - n);

	 projectionMtx[3][2] = 1.f;

	 projectionMtx[3][3] = 0.f;

	 return glm::transpose(projectionMtx);
}