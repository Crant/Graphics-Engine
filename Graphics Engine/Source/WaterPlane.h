#pragma once

#include "iWaterPlane.h"
#include "TextureResource.h"

class WaterPlane : public iWaterPlane
{
private:
	/*!
		pPlanes is the water planes corners
		[0] - x min, [1] - x max, [2] - z min, [3] - z max
	*/
	D3DXVECTOR3 zPoints[4];
	float zWaterTranslation;
	float zRefractScale;

	TextureResource* zNormalMapResource;

	bool zRefractions;
	bool zReflections;

public:
	WaterPlane();
	WaterPlane(D3DXVECTOR3 pCorners[4]);

	bool HasRefraction() {return this->zRefractions;}
	bool HasRelfections() {return this->zReflections;}
};