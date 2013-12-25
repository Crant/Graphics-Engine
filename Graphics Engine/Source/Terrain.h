#pragma once

#include "iTerrain.h"
#include "Vertex.h"
#include "TextureResource.h"
#include "BufferResource.h"
#include "HeightMap.h"

class Terrain : public iTerrain
{
private:
	//DirectX Data
	D3DXMATRIX zWorldMatrix;
	D3DXVECTOR3 zPosition;
	D3DXVECTOR3 zScale;

	std::string zHeightMapFilename;
	std::vector<Vertex> zVertices;
	std::vector<unsigned int> zIndices;
	unsigned int zSize;

	BufferResource* zVertexBuffer;
	BufferResource* zIndexBuffer;

	TextureResource* zTextureResource;
	D3D_PRIMITIVE_TOPOLOGY zTopology;

	HeightMap* zHeightMap;

private:
	void CreateMesh();
	void CalcNormals();

public:
	Terrain(D3DXVECTOR3 position, D3DXVECTOR3 scale, unsigned int size, const std::string& heightmap);
	virtual ~Terrain();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	D3DXMATRIX& GetWorldMatrix() {return this->zWorldMatrix;}
	D3DXVECTOR3& GetScaleD3DX() {return this->zScale;}
	BufferResource* GetVertexBufferPtr() {return this->zVertexBuffer;}
	BufferResource* GetIndexBufferPtr() {return this->zIndexBuffer;}
	TextureResource* GetTextureResourcePtr() {return this->zTextureResource;}

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }

	void SetScale(const D3DXVECTOR3& scale) { this->zScale = scale; }

	//Is used internally when needed, but can be used from the outside for debugging.
	void RecreateWorldMatrix();

	//** iTerrain interface functions ** - for descriptions, see iTerrain.h.
	//GET-functions.
	//Object data
	virtual Vector3 GetScale() const { return Vector3(this->zScale.x, this->zScale.y, this->zScale.z); }
	virtual Vector3 GetPosition() const { return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z); }
		
	//Vertex data
	virtual float CalcHeightAt(float x, float z);
	float GetHeightAt(int row, int col);
	virtual bool IsInside(const Vector2& pos);

	//SET-functions
	//Object data
	virtual void SetScale(const Vector3& scale);
};