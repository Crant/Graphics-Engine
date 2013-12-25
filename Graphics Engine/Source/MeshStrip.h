#pragma once

//#include "BoundingSphere.h"
#include <string>
#include <vector>

//Avoid circular include
class Object3D;
struct VertexNormalMap;
class Material;

class MeshStrip
{
private:
	Object3D* zRenderObject;
	std::vector<VertexNormalMap> zMesh;
	std::vector<int> zIndicies;
	//int* indicies;
	std::string zTexture;
	Material* material;
	//BoundingSphere bb;

public:
	MeshStrip();
	virtual ~MeshStrip();
	MeshStrip(const MeshStrip* origObj);

	std::vector<VertexNormalMap> GetVerts() const { return this->zMesh; }
	void SetVerts(std::vector<VertexNormalMap> verts) { this->zMesh = verts; }
	int GetNrOfVerts() const { return this->zMesh.size(); }
	std::vector<int> GetIndicies() const { return this->zIndicies; }
	void SetIndicies(std::vector<int> inds) { this->zIndicies = inds; }
	int GetNrOfIndicies() const { return this->zIndicies.size(); }
	const std::string& GetTexturePath() const { return this->zTexture; }
	void SetTexturePath(std::string path) {this->zTexture = path;}

	Object3D* GetRenderObject() const { return this->zRenderObject; }
	void SetRenderObject(Object3D* ro) { this->zRenderObject = ro; }

	void SetMaterial(Material* mat);
	Material* GetMaterial() const { return this->material; }

// 	BoundingSphere& GetBoundingSphere() { return this->bb; }
// 	void SetBoundingSphere(BoundingSphere bb) { this->bb = bb; }
};