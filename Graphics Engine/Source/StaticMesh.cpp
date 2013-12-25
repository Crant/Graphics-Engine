#include "StaticMesh.h"

StaticMesh::StaticMesh(D3DXVECTOR3 pos, std::string filePath) : Mesh(pos, filePath)
{

}

StaticMesh::~StaticMesh()
{

}

bool StaticMesh::LoadFromFile(std::string file)
{
	return Mesh::LoadFromFile(file);
}

std::vector<MeshStrip*> StaticMesh::GetStrips()
{ 
	static std::vector<MeshStrip*> temp;
	if(this->zMeshStripResource != NULL)
	{
		return this->zMeshStripResource->GetMeshStripsPointer(); 
	}
	else
	{
		return temp;
	}
}