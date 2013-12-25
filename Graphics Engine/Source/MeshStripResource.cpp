#include "MeshStripResource.h"

MeshStripResource::MeshStripResource() : zFilePath(""), zMesh(NULL)
{

}
MeshStripResource::MeshStripResource(std::string filePath, std::vector<MeshStrip*> mesh)
{
	this->zFilePath = filePath;
	this->zMesh = mesh;
}
MeshStripResource::MeshStripResource(const MeshStripResource& origObj)
{
	this->zFilePath = origObj.zFilePath;

	auto it = this->zMesh.begin();
	while (it != this->zMesh.end())
	{
		MeshStrip* temp = (*it);
		delete temp;
		temp = NULL;
	}
	this->zMesh.clear();
	for(unsigned int i = 0; i < origObj.zMesh.size(); i++)
	{
		this->zMesh.push_back(origObj.zMesh[i]); 
	}
}

MeshStripResource::~MeshStripResource()
{
	auto it = this->zMesh.begin();
	while (it != this->zMesh.end())
	{
		MeshStrip* temp = (*it);
		it = this->zMesh.erase(it);
		delete temp;
		temp = NULL;
	}
	this->zMesh.clear();
}