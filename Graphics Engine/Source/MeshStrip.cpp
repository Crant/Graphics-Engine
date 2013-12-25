#include "MeshStrip.h"
#include "Object3D.h"
#include "Material.h"

MeshStrip::MeshStrip()
{
	this->zRenderObject = NULL;
	this->zIndicies.clear();
	this->zMesh.clear();
	this->material = new Material(LAMBERT);
	this->zTexture = "";
}
	
MeshStrip::~MeshStrip() 
{
	if(this->zRenderObject)
	{
		delete this->zRenderObject;
		this->zRenderObject = NULL;
	}
	if(this->material)
	{
		delete this->material;
		this->material = NULL;
	}
}

MeshStrip::MeshStrip(const MeshStrip* origObj)
{
	this->zRenderObject = origObj->zRenderObject;
	this->zMesh = origObj->zMesh;
	this->zIndicies = origObj->GetIndicies();
	this->zTexture = origObj->zTexture;
	this->material = new Material(origObj->material);
}

void MeshStrip::SetMaterial(Material* mat)
{
	if(this->material)
		delete this->material;

	this->material = mat;
}