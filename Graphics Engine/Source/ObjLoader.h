#pragma once


#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "DirectX.h"
#include <iosfwd>


struct MaterialData
{
	std::string name;
	std::string texture;
	int illum;
	D3DXVECTOR3 kd;
	D3DXVECTOR3 ka;
	D3DXVECTOR3 tf;
	D3DXVECTOR3 ks;

	float ni;

	MaterialData()
	{
		name = "";
		illum = 0;
		kd = D3DXVECTOR3(0, 0, 0);
		ka = D3DXVECTOR3(0, 0, 0);
		tf = D3DXVECTOR3(0, 0, 0);
		texture = "";
		ks = D3DXVECTOR3(0, 0, 0);
		ni = 0.0f;
	}
};

struct FaceData
{
	int data[3][3];
	// NOTE: string in material is slow?
	std::string material;
};

class ObjData
{
public:
	std::vector<D3DXVECTOR3>* vertspos;
	std::vector<D3DXVECTOR2>* textcoords;
	std::vector<D3DXVECTOR3>* vertsnorms;
	std::vector<FaceData>* faces;
	std::vector<MaterialData>* mats;

	ObjData() 
	{ 
		vertspos = new std::vector<D3DXVECTOR3>();
		textcoords = new std::vector<D3DXVECTOR2>();
		vertsnorms = new std::vector<D3DXVECTOR3>();
		faces = new std::vector<FaceData>();
		mats = new std::vector<MaterialData>();
	}

	virtual ~ObjData()
	{
		if(this->vertspos)
			delete this->vertspos;

		if(this->textcoords)
			delete this->textcoords;

		if(this->vertsnorms)
			delete this->vertsnorms;

		if(this->faces)
			delete this->faces;

		if(this->mats)
			delete this->mats;

	}
};

class ObjLoader
{
private:
	D3DXVECTOR3 GetVertPosData(std::string line);
	D3DXVECTOR2 GetTextCoordsData(std::string line);
	D3DXVECTOR3 GetVertsNormsData(std::string line);
	FaceData GetFaceData(std::string line);

public:
	ObjLoader() {}
	virtual ~ObjLoader() {}

	ObjData* LoadObjFile(const std::string& filepath);
	void ReadFromBinaryFile(ObjData* returndata, std::ifstream& binfile);
	void CreateBinaryFile(const std::string& filename, std::string cacheFolder, ObjData* returndata);
};