#pragma once

#include "DirectX.h"
#include "Vertex.h"
#include "TextureResource.h"
#include "BufferResource.h"

class Object3D
{
private:
	BufferResource* zVertices;
	BufferResource* zIndices;
	TextureResource* zTextureResource;
	TextureResource* zNormalMap;
	D3D_PRIMITIVE_TOPOLOGY zTopology;
	
public:

	Object3D(BufferResource* verts, BufferResource* inds, TextureResource* textureResource, 
		TextureResource* normalMap, D3D_PRIMITIVE_TOPOLOGY topology);
	virtual ~Object3D();

	BufferResource* GetVertexBuffer() { return this->zVertices; }
	BufferResource* GetIndexBuffer() { return this->zIndices; }

	D3D_PRIMITIVE_TOPOLOGY GetTopology() { return this->zTopology; }
	TextureResource* GetTextureResource() { return this->zTextureResource; }
	TextureResource* GetNormalMapResource() { return this->zNormalMap; }
};