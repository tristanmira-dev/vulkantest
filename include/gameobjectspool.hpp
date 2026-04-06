#ifndef GAMEOBJECTSPOOL_HPP
#define GAMEOBJECTSPOOL_HPP



#include "vulkan/vulkan.hpp"
#include "vertex.hpp"

template<typename T, typename Source>
void addToCollection(std::vector<T> &collection, Source srcCollection) {
	size_t initialSize{ collection.size() };
	collection.resize(initialSize + srcCollection.size());
	std::copy(srcCollection.begin(), srcCollection.end(), collection.begin() + initialSize);
}

struct GameObjectPool {
	std::vector<Vertex> vertices;
	std::vector<GameObjectInfo> gameObjInfoCollection;
	std::vector<uint32_t> indices;
	std::vector<uint32_t> verticesInformation;

	void addGameObject(std::initializer_list<Vertex> const &verticesData, GameObjectInfo gameObjInfo, std::vector<uint32_t> index) {
		addToCollection(vertices, verticesData);

		gameObjInfoCollection.emplace_back(gameObjInfo); 

		addToCollection(indices, index);
		verticesInformation.emplace_back(static_cast<uint32_t>(index.size()));
	}
};





#endif