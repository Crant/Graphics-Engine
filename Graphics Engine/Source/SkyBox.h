#pragma once

#include "StaticMesh.h"

using namespace std;

class SkyBox : public StaticMesh
{
private: //Private variables
	MeshStrip* strip;
public:
	SkyBox(D3DXVECTOR3 pos, int latitude, int longitude);
	virtual ~SkyBox();
	D3DXMATRIX GetSkyboxWorldMatrix(D3DXVECTOR3 camPos, float FarPlane);

	MeshStrip* GetStrip();
};