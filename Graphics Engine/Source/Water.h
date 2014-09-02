#pragma once

#include "iWater.h"
#include "DirectX.h"
#include "TextureResource.h"

class Water : iWater
{
private:
	D3DXVECTOR3 zPoints[4];
	float zWaterTranslation;
	float zRefractScale;

	TextureResource* zNormalMapResource;

public:
	Water(D3DXVECTOR3 pCorners[4], float pRefractScale = 0.015f);
};