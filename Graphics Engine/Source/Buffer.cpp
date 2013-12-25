#include "Buffer.h"

Buffer::Buffer()
{
	this->zBuffer = NULL;
}

Buffer::~Buffer()
{
	SAFE_RELEASE(this->zBuffer);
}

HRESULT Buffer::Init( ID3D11Device* device,ID3D11DeviceContext* deviceContext, BUFFER_INIT_DESC& initDesc )
{
	this->zDevice = device;
	this->zDeviceContext = deviceContext;

	D3D11_BUFFER_DESC bufferDesc;
	this->zType = initDesc.Type;

	switch (this->zType)
	{
	case VERTEX_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			if(initDesc.Usage == BUFFER_STREAM_OUT_TARGET)
				bufferDesc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
		}
		break;
	case INDEX_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		}
		break;
	case CONSTANT_BUFFER_VS:
	case CONSTANT_BUFFER_GS:
	case CONSTANT_BUFFER_PS:
		{
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		}
		break;
	default:
		return E_FAIL;
		break;
	};

	this->zUsage = initDesc.Usage;

	this->zElementSize = initDesc.ElementSize;
	this->zElementCount = initDesc.NumElements;

	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	if(this->zUsage == BUFFER_CPU_READ)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	}
	else if(this->zUsage == BUFFER_CPU_WRITE)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	else if(this->zUsage == BUFFER_CPU_WRITE_DISCARD)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = initDesc.NumElements * initDesc.ElementSize;

	//set at least 16 bytes
	if(bufferDesc.ByteWidth < 16)
		bufferDesc.ByteWidth = 16;

	HRESULT hr = S_OK;
	if(initDesc.InitData)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initDesc.InitData;
		hr = this->zDevice->CreateBuffer(&bufferDesc, &data, &this->zBuffer);
	}
	else
	{
		hr = this->zDevice->CreateBuffer(&bufferDesc, NULL, &this->zBuffer);
	}

	if(FAILED(hr))
	{
		//MessageBox(NULL, "Unable to create buffer.", "Slenda Error", MB_ICONERROR | MB_OK);
	}

	return hr;
}

HRESULT Buffer::Apply( UINT32 misc /*= 0*/ )
{
	HRESULT hr = S_OK;

	switch(this->zType)
	{
	case VERTEX_BUFFER:
		{
			UINT32 vertexSize = this->zElementSize;
			UINT32 offset = 0;
			this->zDeviceContext->IASetVertexBuffers(misc, 1, &this->zBuffer, &vertexSize, &offset );
		}
		break;
	case INDEX_BUFFER:
		{
			this->zDeviceContext->IASetIndexBuffer(this->zBuffer, DXGI_FORMAT_R32_UINT, 0);
		}
		break;
	case CONSTANT_BUFFER_VS:
		{
			this->zDeviceContext->VSSetConstantBuffers(misc, 1, &this->zBuffer);
		}
		break;
	case CONSTANT_BUFFER_GS:
		{
			this->zDeviceContext->GSSetConstantBuffers(misc, 1, &this->zBuffer);
		}
		break;
	case CONSTANT_BUFFER_PS:
		{
			this->zDeviceContext->PSSetConstantBuffers(misc, 1, &this->zBuffer);
		}
		break;
	default:
		hr = E_FAIL;
		break;
	};

	return hr;
}