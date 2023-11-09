#include "vklibMesh.h"
#include <glm/glm.hpp>

vklibMesh* vklibMesh::_defaultTriangle = nullptr;
vklibMesh* vklibMesh::_defaultCube = nullptr;

vklibMesh::vklibMesh(vklib* vklib)
{
	this->_vklib = vklib;
}

vklibMesh::~vklibMesh()
{
}

vklibMesh::vklibMesh(vklib* vklib, const VERTEX_TYPE* vertexData, uint32_t dataSize, uint32_t dataStride,
	const INDEX_TYPE* indexData, uint32_t indexSize, uint32_t indexCount)
{
		this->_vklib = vklib;
		vertCount = indexCount;
		triangleCount = indexCount / 3;

		//create vertex buffer
		{
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			VkDeviceSize bufferSize = sizeof(vertexData[0]) * dataSize;
			vklib->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(vklib->device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertexData, (size_t)bufferSize);
			vkUnmapMemory(vklib->device, stagingBufferMemory);

			vklib->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

			vklib->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

			vkDestroyBuffer(vklib->device, stagingBuffer, nullptr);
			vkFreeMemory(vklib->device, stagingBufferMemory, nullptr);
		}

		//create index buffer
		{
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			VkDeviceSize bufferSize = sizeof(indexData[0]) * indexSize;
			vklib->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(vklib->device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, indexData, (size_t)bufferSize);
			vkUnmapMemory(vklib->device, stagingBufferMemory);

			vklib->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

			vklib->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

			vkDestroyBuffer(vklib->device, stagingBuffer, nullptr);
			vkFreeMemory(vklib->device, stagingBufferMemory, nullptr);
		}
}

VkBuffer vklibMesh::getVertexBuffer()
{
	return vertexBuffer;
}

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

vklibMesh* vklibMesh::DefaultTriangle(vklib* vklib)
{
	if (_defaultTriangle == nullptr)
	{
		_defaultTriangle = new vklibMesh(vklib);

		std::vector<Vertex> vertices = {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		_defaultTriangle->vertCount = vertices.size();

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		vklib->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _defaultTriangle->vertexBuffer, _defaultTriangle->vertexBufferMemory);

		void* data;
		vkMapMemory(vklib->device, _defaultTriangle->vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);

		vkUnmapMemory(vklib->device, _defaultTriangle->vertexBufferMemory);
	}
	return _defaultTriangle;
}

vklibMesh* vklibMesh::DefaultCube(vklib* vklib)
{
	if (_defaultCube == nullptr)
	{
		std::vector<VERTEX_TYPE> vertices = {
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		const std::vector<INDEX_TYPE> indices = {
			0, 1, 2, 2, 3, 0
		};

		_defaultCube = new vklibMesh(vklib, vertices.data(), vertices.size(), sizeof(vertices[0]), indices.data(), indices.size(), indices.size());
	}
	return _defaultCube;
}


VkVertexInputBindingDescription vklibMesh::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> vklibMesh::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

	// Position
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	//attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	// Color
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	//attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);
	return attributeDescriptions;
}
