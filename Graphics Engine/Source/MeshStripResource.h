#pragma once

#include "ReferenceCounted.h"
#include "CJen.h"
#include "MeshStrip.h"
#include "Vertex.h"
#include "Material.h"
#include <string>
#include <vector>
using namespace std;

class MeshStripResource : public ReferenceCounted
{
private:
	string	zFilePath;
	std::vector<MeshStrip*> zMesh;

private:
	virtual ~MeshStripResource();

public:
	MeshStripResource();
	MeshStripResource(string filePath, std::vector<MeshStrip*> mesh);
	MeshStripResource(const MeshStripResource& origObj);

	string GetName() const { return this->zFilePath; }
	std::vector<MeshStrip*> GetMeshStripsPointer() const { return this->zMesh; }
	void SetMeshStripsPointer(std::vector<MeshStrip*> pointer) { this->zMesh = pointer; }

};