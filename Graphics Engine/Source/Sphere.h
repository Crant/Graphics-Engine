#ifndef SPHERE_H
#define SPHERE_H

#include "DirectX.h"
#include "Vertex.h"
#include "BufferResource.h"
#include <vector>

class Sphere
{
private:
	float mRadius;
	UINT  mNumSlices;
	UINT  mNumStacks;

	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;

	BufferResource* zVertexBufferResource;
	BufferResource* zIndexBufferResource;

	std::vector<VertexTangent> zVertices;
	std::vector<DWORD> zIndices;

public:
	Sphere();
	~Sphere();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius, UINT numSlices, UINT numStacks);

	BufferResource* GetVertexBuffer() {return this->zVertexBufferResource;}
	BufferResource* GetIndexBuffer() {return this->zIndexBufferResource;}

private:
	

	void BuildStack();
	//void createBoundingSphere(Vertex *list);
};

#endif // SPHERE_H