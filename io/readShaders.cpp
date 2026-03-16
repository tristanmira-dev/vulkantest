#include "readShaders.hpp"
#include <fstream>
#include <vector>

std::vector<char> readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate /*Useful for getting the file size*/ | std::ios::binary);

	if (!file) {
		throw std::runtime_error{"Failed to load Shader"};
	}

	std::vector<char> buffer(file.tellg());

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close(); /*file will close automatically but whatevs*/
	return buffer;

}
