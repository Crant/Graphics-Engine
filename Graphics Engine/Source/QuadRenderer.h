#pragma once

#include "DirectX.h"

class QuadRenderer
{
private:
	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;
	ID3D11Buffer* zIndexBuffer;
	ID3D11Buffer* zVertexBuffer;
public:
	QuadRenderer();
	virtual ~QuadRenderer();
	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Draw();
};