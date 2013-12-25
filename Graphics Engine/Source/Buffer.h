#pragma once

#include "DirectX.h"

enum BUFFER_TYPE
{
	VERTEX_BUFFER,
	INDEX_BUFFER,
	CONSTANT_BUFFER_VS,
	CONSTANT_BUFFER_GS,
	CONSTANT_BUFFER_PS,
	BUFFER_TYPE_COUNT
};

enum BUFFER_USAGE
{
	BUFFER_DEFAULT,
	BUFFER_STREAM_OUT_TARGET,
	BUFFER_CPU_WRITE,
	BUFFER_CPU_WRITE_DISCARD,
	BUFFER_CPU_READ,
	BUFFER_USAGE_COUNT
};

struct BUFFER_INIT_DESC
{
	BUFFER_TYPE		Type;
	UINT32			NumElements;
	UINT32			ElementSize;
	BUFFER_USAGE	Usage;
	void*			InitData;

	BUFFER_INIT_DESC()
	{
		InitData = NULL;
	}
};

class Buffer
{
private:
	ID3D11Buffer* zBuffer;

	BUFFER_TYPE		zType;
	BUFFER_USAGE	zUsage;

	UINT32			zElementSize;
	UINT32			zElementCount;

	ID3D11Device*			zDevice;
	ID3D11DeviceContext*	zDeviceContext;

public:
	Buffer();
	virtual ~Buffer();

	HRESULT Init(ID3D11Device* device,ID3D11DeviceContext* immediateContext,
		BUFFER_INIT_DESC& initDesc);

	HRESULT Apply(UINT32 misc = 0);

	ID3D11Buffer* GetBufferPtr() {return this->zBuffer;}

	UINT32 GetVertexSize() {return this->zElementSize;}
	UINT32 GetElementCount(){return this->zElementCount;}
};