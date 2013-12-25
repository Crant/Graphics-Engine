#pragma once

#include "DirectX.h"
#include <string>

class Shader
{
private:
	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;
	ID3D11InputLayout* zInputLayout;
	D3D11_INPUT_ELEMENT_DESC* zInputElementDesc;
	std::string zFilename;
	unsigned int zNumElements;

	ID3DX11Effect*			zEffect;
	ID3DX11EffectTechnique* zTechnique;

	UINT zCurrentTechIndex;

public:
	Shader();
	~Shader();

	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename,
		const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements);

	HRESULT Apply(unsigned int pass);

	void SetMatrix(char* variable, const D3DXMATRIX& mat);
	void SetMatrix(char* variable, const D3DXMATRIX* mat );
	void SetFloat(char* variable, const float& value);

	void SetFloat2(char* variable, const D3DXVECTOR2& value);
	void SetFloat3(char* variable, const D3DXVECTOR3& value);
	void SetFloat4(char* variable, const D3DXVECTOR4& value);

	void SetInt(char* variable, int value);
	void SetBool(char* variable, bool value);
	void SetRawData(char* variable, void* data, size_t size);

	void SetResource(const char* variable, ID3D11ShaderResourceView* value);

	void SetFloatAtIndex(int index, char* variable, float value);
	HRESULT SetTechnique(UINT index);
};