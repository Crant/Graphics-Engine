#pragma once

#include "DirectX.h"
#include "Mesh.h"

class StaticMesh : public Mesh
{
public:
	StaticMesh(D3DXVECTOR3 pos, std::string filePath);
	virtual ~StaticMesh();

	bool LoadFromFile(std::string file);
	virtual std::vector<MeshStrip*> GetStrips();
};