#pragma once

#include "DirectX.h"
#include "iImage.h"
#include "ResourceManager.h"

class Image : public iImage
{
private:
	D3DXVECTOR2 zPosition;
	float zStrata;
	D3DXVECTOR2 zDimension;
	float zOpacity;
	TextureResource* zTextureResource;

public:
	Image(D3DXVECTOR2 pos, D3DXVECTOR2 dim);
	virtual ~Image();

	virtual Vector2 GetPosition() const;
	virtual void SetPosition(Vector2 pos);
	virtual void SetStrata(float strata);
	virtual float GetStrata() const;
	TextureResource* GetTexture() const { return this->zTextureResource; }
	void SetTexture(TextureResource* text) { this->zTextureResource = text; }
	virtual Vector2 GetDimensions() const;
	virtual void SetDimensions(Vector2 dims);
	virtual float GetOpacity() const { return this->zOpacity; }
	virtual void SetOpacity(float opacity) { this->zOpacity = opacity; }

	float GetStrata01() { return this->zStrata; }
};