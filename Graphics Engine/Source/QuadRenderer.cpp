#include "QuadRenderer.h"
#include "Vertex.h"
#include "Exceptions.h"

QuadRenderer::QuadRenderer()
	: zDevice(NULL), zDeviceContext(NULL), zIndexBuffer(NULL), zVertexBuffer(NULL)
{
}

QuadRenderer::~QuadRenderer()
{
	SAFE_RELEASE(this->zIndexBuffer);
	SAFE_RELEASE(this->zVertexBuffer);
}

void QuadRenderer::Init( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	this->zDevice = device;
	this->zDeviceContext = deviceContext;

	D3DXVECTOR3 pos[] =
	{
		D3DXVECTOR3( 1.0f, -1.0f,  0.0f), //lower left
		D3DXVECTOR3(-1.0f, -1.0f,  0.0f), //lower right
		D3DXVECTOR3(-1.0f,  1.0f,  0.0f), //upper left
		D3DXVECTOR3( 1.0f,  1.0f,  0.0f)  //Upper right
	};
	D3DXVECTOR2 uv[] =
	{
		D3DXVECTOR2(1.0f, 1.0f), //lower right
		D3DXVECTOR2(0.0f, 1.0f), //Lower left
		D3DXVECTOR2(0.0f, 0.0f), //Upper left
		D3DXVECTOR2(1.0f, 0.0f)  //Upper right
	};
	VertexTex vertices[4];
	for(int i = 0; i < 4; i++)
	{
		vertices[i].position = pos[i];
		vertices[i].texCoord = uv[i];
	}

	unsigned int indices[] = 
	{
		0, 1, 2,
		2, 3, 0
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth = sizeof(unsigned int) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitDataIB;
	vinitDataIB.pSysMem = &indices[0];
	if( FAILED( this->zDevice->CreateBuffer( &ibd, &vinitDataIB, &zIndexBuffer ) ) )
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VertexTex) * 6;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitDataVB;
	vinitDataVB.pSysMem = &vertices[0];
	if(FAILED( this->zDevice->CreateBuffer( &vbd, &vinitDataVB, &zVertexBuffer ) ) )
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
}

void QuadRenderer::Draw()
{
	UINT stride = sizeof(VertexTex);
	UINT offset = 0;
	this->zDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->zDeviceContext->IASetVertexBuffers(0, 1, &zVertexBuffer, &stride, &offset);
	this->zDeviceContext->IASetIndexBuffer(this->zIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	this->zDeviceContext->DrawIndexed(6, 0, 0);
}