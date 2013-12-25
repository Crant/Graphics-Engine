#include "Shader.h"
#include "Exceptions.h"

Shader::Shader()
{
	this->zEffect = NULL;
	this->zTechnique = NULL;
	this->zInputLayout = NULL;
	this->zNumElements = 0;
	this->zFilename = "";
	this->zInputElementDesc = NULL;
	this->zCurrentTechIndex = 10;
}

Shader::~Shader()
{
	SAFE_RELEASE(this->zInputLayout);
	SAFE_RELEASE(this->zEffect);
}

HRESULT Shader::Init( ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements )
{
	this->zDevice = device;
	this->zDeviceContext = deviceContext;
	this->zNumElements = numElements;
	this->zFilename = filename;
	this->zInputElementDesc = (D3D11_INPUT_ELEMENT_DESC*)inputElementDesc;

	HRESULT hr = S_OK;

	ID3DBlob* mBlobEffect = NULL;
	ID3DBlob* mBlobErrors = NULL;
	
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	D3DX11CompileFromFile(filename, NULL, NULL, 
		"", "fx_5_0", dwShaderFlags, NULL, NULL,
		&mBlobEffect, &mBlobErrors, &hr);

	if (FAILED(hr))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Effect compilation error, file: " + zFilename));
	}

	if (FAILED(hr = D3DX11CreateEffectFromMemory(
		mBlobEffect->GetBufferPointer(), 
		mBlobEffect->GetBufferSize(), 
		dwShaderFlags, 
		this->zDevice, 
		&this->zEffect)))
	{
		throw(ShaderException(
			__FILE__, 
			__LINE__, 
			__FUNCTION__, 
			"Cannot create effect from memory, file: " + zFilename));
	}

	SAFE_RELEASE(mBlobEffect);
	SAFE_RELEASE(mBlobErrors);

	return this->SetTechnique(0);
}

HRESULT Shader::SetTechnique(UINT index)
{
	HRESULT hr = S_OK;

	if (this->zCurrentTechIndex != index)
	{
		this->zCurrentTechIndex = index;

		this->zTechnique = this->zEffect->GetTechniqueByIndex(index);

		if (this->zInputElementDesc)
		{
			D3DX11_PASS_DESC passDesc;
			this->zTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
			if (FAILED(hr = this->zDevice->CreateInputLayout(
				this->zInputElementDesc,
				this->zNumElements,
				passDesc.pIAInputSignature,
				passDesc.IAInputSignatureSize,
				&this->zInputLayout
				)))
			{
				throw(ShaderException(
					__FILE__, 
					__LINE__, 
					__FUNCTION__, 
					"Cannot create input layout"));
			}

			return hr;
		}
	}
	return hr;
}

HRESULT Shader::Apply( unsigned int pass )
{
	ID3DX11EffectPass* p = this->zTechnique->GetPassByIndex(pass);
	if (p)
	{
		p->Apply(0, this->zDeviceContext);

		if (this->zInputLayout)
		{
			this->zDeviceContext->IASetInputLayout(this->zInputLayout);
		}
		return S_OK;
	}
	return E_FAIL;
}

void Shader::SetMatrix( char* variable, const D3DXMATRIX& mat )
{
	this->zEffect->GetVariableByName(variable)->AsMatrix()->SetMatrix((float*)&mat);
}

void Shader::SetMatrix( char* variable, const D3DXMATRIX* mat )
{
	this->zEffect->GetVariableByName(variable)->AsMatrix()->SetMatrix((float*)&mat);
}

void Shader::SetFloat( char* variable, const float& value )
{
	this->zEffect->GetVariableByName(variable)->AsScalar()->SetFloat(value);
}

void Shader::SetFloat2( char* variable, const D3DXVECTOR2& value )
{
	this->zEffect->GetVariableByName(variable)->AsVector()->SetFloatVector((float*)&value);
}

void Shader::SetFloat3( char* variable, const D3DXVECTOR3& value )
{
	this->zEffect->GetVariableByName(variable)->AsVector()->SetFloatVector((float*)&value);
}

void Shader::SetFloat4( char* variable, const D3DXVECTOR4& value )
{
	this->zEffect->GetVariableByName(variable)->AsVector()->SetFloatVector((float*)&value);
}

void Shader::SetInt( char* variable, int value )
{
	this->zEffect->GetVariableByName(variable)->AsScalar()->SetInt(value);
}

void Shader::SetBool( char* variable, bool value )
{
	this->zEffect->GetVariableByName(variable)->AsScalar()->SetBool(value);
}

void Shader::SetRawData( char* variable, void* data, size_t size )
{
	this->zEffect->GetVariableByName(variable)->SetRawValue(data, 0, (UINT)size);
}

void Shader::SetResource( const char* variable, ID3D11ShaderResourceView* value )
{
	this->zEffect->GetVariableByName(variable)->AsShaderResource()->SetResource(value);
}

void Shader::SetFloatAtIndex(int index, char* variable, float value)
{
	this->zEffect->GetVariableByName(variable)->GetElement(index)->AsScalar()->SetFloat(value);
}
