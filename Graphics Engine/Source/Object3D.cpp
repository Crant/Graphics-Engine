#include "Object3D.h"
#include "ResourceManager.h"

Object3D::Object3D( BufferResource* verts, BufferResource* inds, TextureResource* textureResource, TextureResource* normalMap, D3D_PRIMITIVE_TOPOLOGY topology )
{
	this->zVertices = verts;
	this->zIndices = inds;
	this->zTextureResource = textureResource;
	this->zTopology = topology;
	this->zNormalMap = normalMap;
}

Object3D::~Object3D()
{
	if(this->zTextureResource) 
		GetResourceManager()->DeleteTextureResource(this->zTextureResource); 
	if(this->zNormalMap) 
		GetResourceManager()->DeleteTextureResource(this->zNormalMap);
	if(this->zVertices) 
		GetResourceManager()->DeleteBufferResource(this->zVertices); 
	if(this->zIndices) 
		GetResourceManager()->DeleteBufferResource(this->zIndices);
}