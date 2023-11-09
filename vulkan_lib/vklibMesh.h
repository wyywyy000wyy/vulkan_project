#pragma once
#include "vklib_require.h"
#include "vklib.h"
#include <array>



class vklibMesh
{
public:
	vklibMesh(vklib* vklib);
	~vklibMesh();

	vklibMesh(vklib* vklib, const VERTEX_TYPE* vertexData, uint32_t dataSize, uint32_t dataStride,
		const INDEX_TYPE* indexData, uint32_t indexSize, uint32_t indexCount);

	//void createVertexBuffer(const void* vertexData, uint32_t dataSize, uint32_t dataStride);
	//void destroyVertexBuffer();

	static vklibMesh* DefaultTriangle(vklib* vklib);
	static vklibMesh* DefaultCube(vklib* vklib);

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

	VkBuffer getVertexBuffer();
	VkBuffer getIndexBuffer() { return indexBuffer;}

	int getVertexCount(){ return vertCount;}
	int getTriangleCount() { return triangleCount;}
	int getIndexCount() { return triangleCount * 3; }
protected:
	int vertCount;
	int triangleCount;

	vklib* _vklib;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	static vklibMesh* _defaultTriangle;
	static vklibMesh* _defaultCube;
};