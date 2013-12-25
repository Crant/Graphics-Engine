#pragma once

#include "DirectX.h"
#include "ReferenceCounted.h"
#include <string>

class TextureResource : public ReferenceCounted
{
private:
	std::string zFileName;
	ID3D11ShaderResourceView* zSRV;

public:
	TextureResource();
	TextureResource(ID3D11ShaderResourceView* SRV);
	TextureResource(std::string filePath, ID3D11ShaderResourceView* SRV);

	const std::string& GetName() const {return this->zFileName;}
	ID3D11ShaderResourceView* GetSRVPointer() const {return this->zSRV;}
	void SetSRVPointer(ID3D11ShaderResourceView* pointer) { this->zSRV = pointer; }
};