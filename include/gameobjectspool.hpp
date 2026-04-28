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

struct VertexInfo {
	uint32_t totalIndices;
	std::vector<uint32_t>::iterator start;
};

struct GameObjectPool {
	std::vector<Vertex> vertices;
	std::vector<GameObjectInfo> gameObjInfoCollection;
	std::vector<uint32_t> indices;
	std::vector<VertexInfo> verticesInformation; /*Must match the index of the mesh you want*/
	std::vector<uint32_t> meshIdx;

	void addGameObject(GameObjectInfo gameObjInfo, uint32_t idx) {

		gameObjInfoCollection.emplace_back(gameObjInfo); 
		meshIdx.push_back(idx);

	}


	void addMeshObject(std::initializer_list<Vertex> const& verticesData, std::vector<uint32_t> index) {
		
		addToCollection(vertices, verticesData);

		addToCollection(indices, index);

		verticesInformation.emplace_back(VertexInfo{ static_cast<uint32_t>(index.size()), indices.end() - index.size()});
	}

	void addMeshObject(std::vector<Vertex> const& verticesData, std::vector<uint32_t> index) {

		addToCollection(vertices, verticesData);

		addToCollection(indices, index);

		verticesInformation.emplace_back(VertexInfo{ static_cast<uint32_t>(index.size()), indices.end() - index.size() });
	}
};





#endif