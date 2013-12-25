#include "Sphere.h"
#include "ResourceManager.h"

#define PI (3.14159265358979323846f)

Sphere::Sphere()
: zDevice(NULL), zDeviceContext(NULL), zVertexBufferResource(NULL), 
  zIndexBufferResource(NULL), mRadius(0.0f), mNumSlices(0), mNumStacks(0)
{
	
}

Sphere::~Sphere()
{
	if (this->zVertexBufferResource)
		GetResourceManager()->DeleteBufferResource(this->zVertexBufferResource);
	if (this->zIndexBufferResource)
		GetResourceManager()->DeleteBufferResource(this->zIndexBufferResource);
}

void Sphere::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius, UINT numSlices, UINT numStacks)
{
	this->zDevice = device;
	this->zDeviceContext = deviceContext;

	this->mRadius    = radius;
	this->mNumSlices = numSlices;
	this->mNumStacks = numStacks;

	this->BuildStack();

	BUFFER_INIT_DESC verticesBuffDesc;
	verticesBuffDesc.ElementSize = sizeof(VertexNormalMap);
	verticesBuffDesc.NumElements = this->zVertices.size();
	verticesBuffDesc.Type = VERTEX_BUFFER;
	verticesBuffDesc.Usage = BUFFER_DEFAULT;
	verticesBuffDesc.InitData = &this->zVertices[0];

	this->zVertexBufferResource = GetResourceManager()->CreateBufferResource("SphereVertices", verticesBuffDesc);

	BUFFER_INIT_DESC IndexBufferDesc;
	IndexBufferDesc.ElementSize = sizeof(DWORD);
	IndexBufferDesc.NumElements = this->zIndices.size();
	IndexBufferDesc.Type = INDEX_BUFFER;
	IndexBufferDesc.Usage = BUFFER_DEFAULT;
	IndexBufferDesc.InitData = &this->zIndices[0];
	
	this->zIndexBufferResource = GetResourceManager()->CreateBufferResource("SphereIndices", IndexBufferDesc);

	//Vertex *arr = new Vertex[vertices.size()];

	//for (int i=0;i<vertices.size();i++)
	//{
	//	arr[i] = vertices.at(i);
	//}

	//createBoundingSphere(arr);
	//SAFE_DELETE(arr);

}

void Sphere::BuildStack()
{
	this->zVertices.clear();
	this->zIndices.clear();

	float phiStep = PI / mNumStacks;

	// do not count the poles as rings
	UINT numRings = mNumStacks - 1;

	// Compute vertices for each stack ring.
	for(UINT i = 1; i <= numRings; ++i)
	{
		float phi = i * phiStep;

		// vertices of ring
		float thetaStep = 2.0f * PI / mNumSlices;
		for(UINT j = 0; j <= mNumSlices; ++j)
		{
			float theta = j * thetaStep;

			VertexNormalMap v;

			// spherical to cartesian
			v.position.x = mRadius * sinf(phi) * cosf(theta);
			v.position.y = mRadius * cosf(phi);
			v.position.z = mRadius * sinf(phi) * sinf(theta);

			// partial derivative of P with respect to theta
			v.tangent.x = -mRadius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = mRadius * sinf(phi) * cosf(theta);

			D3DXVec3Normalize(&v.normal, &v.position);

			v.texCoord.x = theta / (2.0f * PI);
			v.texCoord.y = phi / PI;

			this->zVertices.push_back( v );
		}
	}

	// poles: note that there will be texture coordinate distortion
	VertexNormalMap vertexTemp = VertexNormalMap(D3DXVECTOR3(0.0f, -mRadius, 0.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	vertexTemp.tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	this->zVertices.push_back( vertexTemp );
	
	vertexTemp = VertexNormalMap(D3DXVECTOR3(0.0f,  mRadius, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f,  1.0f, 0.0f));
	vertexTemp.tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	this->zVertices.push_back( vertexTemp );

	UINT northPoleIndex = (UINT)this->zVertices.size() - 1;
	UINT southPoleIndex = (UINT)this->zVertices.size() - 2;

	UINT numRingVertices = mNumSlices + 1;

	// Compute indices for inner stacks (not connected to poles).
	for(UINT i = 0; i < mNumStacks - 2; ++i)
	{
		for(UINT j = 0; j < mNumSlices; ++j)
		{
			this->zIndices.push_back(i * numRingVertices + j);
			this->zIndices.push_back(i * numRingVertices + j + 1);
			this->zIndices.push_back((i + 1) * numRingVertices + j);

			this->zIndices.push_back((i + 1) * numRingVertices + j);
			this->zIndices.push_back(i * numRingVertices + j + 1);
			this->zIndices.push_back((i + 1) * numRingVertices + j + 1);
		}
	}

	// Compute indices for top stack.  The top stack was written 
	// first to the vertex buffer.
	for(UINT i = 0; i < mNumSlices; ++i)
	{
		this->zIndices.push_back(northPoleIndex);
		this->zIndices.push_back(i + 1);
		this->zIndices.push_back(i);
	}

	// Compute indices for bottom stack.  The bottom stack was written
	// last to the vertex buffer, so we need to offset to the index
	// of first vertex in the last ring.
	UINT baseIndex = (numRings - 1) * numRingVertices;
	for(UINT i = 0; i < mNumSlices; ++i)
	{
		this->zIndices.push_back(southPoleIndex);
		this->zIndices.push_back(baseIndex + i);
		this->zIndices.push_back(baseIndex + i + 1);
	}
}