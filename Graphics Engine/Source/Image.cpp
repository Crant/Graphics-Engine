#include "Image.h"

Image::Image(D3DXVECTOR2 pos, D3DXVECTOR2 dim)
{
	this->zPosition = pos;
	this->zDimension = dim;
	this->zTextureResource = NULL;
	this->zOpacity = 1.0f;
	this->zStrata = 0.5f;
}

Image::~Image()
{
	if(this->zTextureResource) 
		GetResourceManager()->DeleteTextureResource(this->zTextureResource);
}

Vector2 Image::GetPosition() const
{
	return Vector2(this->zPosition.x, this->zPosition.y);
}

void Image::SetPosition( Vector2 pos )
{
	this->zPosition = D3DXVECTOR2(pos.x, pos.y);
}

Vector2 Image::GetDimensions() const
{
	return Vector2(this->zDimension.x, this->zDimension.y);
}

void Image::SetDimensions( Vector2 dims )
{
	this->zDimension = D3DXVECTOR2(dims.x, dims.y);
}

void Image::SetStrata( float strata )
{
	this->zStrata = strata / 1000.0f;
}

float Image::GetStrata() const
{
	return this->zStrata * 1000.0f;
}
