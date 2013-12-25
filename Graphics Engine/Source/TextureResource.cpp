#include "TextureResource.h"

TextureResource::TextureResource()
{
	this->zFileName = "";
	this->zSRV = NULL;
}

TextureResource::TextureResource( ID3D11ShaderResourceView* SRV )
{
	this->zFileName = "";
	this->zSRV = SRV;
}

TextureResource::TextureResource( std::string filePath, ID3D11ShaderResourceView* SRV )
{
	this->zFileName = filePath;
	this->zSRV = SRV;
}