#include "Terrain.h"
#include "ResourceManager.h"
#include "Exceptions.h"

Terrain::Terrain(D3DXVECTOR3 position, D3DXVECTOR3 scale, unsigned int size, const std::string& heightmap)
{
	this->zIndexBuffer = NULL;
	this->zVertexBuffer = NULL;
	this->zHeightMap = NULL;

	this->zSize = size;
	this->zScale = scale;
	this->zPosition = position;
	
	this->RecreateWorldMatrix();

	this->zTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (heightmap != "")
	{
		this->zHeightMap = new HeightMap(size, size, heightmap, 0.01f, -1.0f);
	}

	this->CreateMesh();
}

void Terrain::CreateMesh()
{
	unsigned int vertRows = this->zSize - 1;
	unsigned int vertCols = this->zSize - 1;

	this->zVertices.resize(this->zSize * this->zSize);
	this->zIndices.resize(vertRows * vertCols * 2 * 3);
	
	/*float cellspacing = 2.0f;
	// coordinates to start generating vertices at
	float width = this->zSize * cellspacing;
	float depth = this->zSize * cellspacing;
	float vrWidth = vertRows * cellspacing;
	float vcDepth = vertCols * cellspacing;

	int startX = (int)(-width / cellspacing);
	int startZ = (int)( depth / cellspacing);

	// coordinates to end generating vertices at
	int endX = (int)( width / cellspacing);
	int endZ = (int)(-depth / cellspacing);

	int ii = 0;
	int jj = 0;
	int i = 0;
	float height = 0;
	for (int zCoord = startZ; zCoord > endZ; zCoord -= (int)cellspacing)
	{
		int j = 0;

		for(int xCoord = startX; xCoord < endX; xCoord += (int)cellspacing)
		{
			unsigned int index = i * this->zSize + j;

			D3DXVECTOR3 position = D3DXVECTOR3(xCoord, height, zCoord);
			D3DXVECTOR3 normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			D3DXVECTOR2 texCoord = D3DXVECTOR2( (xCoord + (0.5f * vrWidth) / vrWidth), (zCoord - (0.5f * vcDepth ) / -vcDepth) );

			this->zVertices[index] = Vertex(position, texCoord, normal);

			j++; // Next Column
		}
		i++; // Next Row
		ii++;
	}*/


	/*for (unsigned int i = 0; i < this->zSize; i++)
	{
	for (unsigned int u = 0; u < this->zSize; u++)
	{
	unsigned int index = i * this->zSize + u;

	D3DXVECTOR3 position = D3DXVECTOR3( ((float)u / vertRows) - 0.5f, height, ((float)i / vertCols) - 0.5f );
	D3DXVECTOR3 normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR2 texCoord = D3DXVECTOR2( ((float)u / vertRows), ((float)i / vertCols) );

	this->zVertices[index] = Vertex(position, texCoord, normal);
	}
	}*/

	float height = 0.0f;
	for (unsigned int i = 0; i < this->zSize; i++)
	{
		for (unsigned int j = 0; j < this->zSize; j++)
		{
			unsigned int index = i * this->zSize + j;

			if (this->zHeightMap)
				height = this->zHeightMap->GetHeightAt(i, j);

			//local pos range [-0.5, 0.5f] * scale
			float x = (float)j / (this->zSize - 1) - 0.5f;
			float z = (float)i / (this->zSize - 1) - 0.5f;
			
			float u = (float)j / ((this->zSize - 1));
			float v = (float)i / ((this->zSize - 1));
			D3DXVECTOR3 position = D3DXVECTOR3(x, height, z);
			D3DXVECTOR3 normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			D3DXVECTOR2 texCoord = D3DXVECTOR2(u, v);

			this->zVertices[index] = Vertex(position, texCoord, normal);
		}
	}

	//D3DXVECTOR3 normal, tangent, bitangent;
	//Vertex v1, v2, v3;
	//
	////Calculate Normals
	//for (unsigned int i = 0; i < this->zSize - 1; i++)
	//{
	//	for (unsigned int j = 0; j < this->zSize; j++)
	//	{
	//		DWORD index = i * this->zSize + j;
	//
	//		v1 = this->zVertices[index];
	//
	//		v2 = this->zVertices[index + 1];
	//
	//		int auxRow = this->zSize;
	//		if (i + 1 == this->zSize)
	//			auxRow = 0;
	//
	//		v3 = this->zVertices[index + auxRow];
	//
	//		GetResourceManager()->CalculateTangentBinormal(v1, v2, v3, tangent, bitangent);
	//
	//		GetResourceManager()->CalculateNormal(tangent, bitangent, normal);
	//
	//		this->zVertices[index].normal = normal;
	//	}
	//}

	this->CalcNormals();

	//Create Indices
	int k = 0;
	for (unsigned int i = 0; i < vertCols; i++)
	{
		for (unsigned int j = 0; j < vertRows; j++)
		{
			this->zIndices[k]	  =	 i      * this->zSize + j;
			this->zIndices[k + 1] = (i + 1)	* this->zSize + j;
			this->zIndices[k + 2] = (i + 1) * this->zSize + j + 1;

			this->zIndices[k + 3] =  i		* this->zSize + j;
			this->zIndices[k + 4] = (i + 1)	* this->zSize + j + 1;
			this->zIndices[k + 5] =  i		* this->zSize + j + 1;

			k += 6; // Next Quad
		}
	}
}

void Terrain::CalcNormals()
{
	D3DXVECTOR3 v1, v2, normal;

	for(unsigned int x = 1; x < this->zSize-1; ++x)
	{
		for(unsigned int z = 1; z < this->zSize-1; ++z)
		{
			unsigned int a = (x-1) * this->zSize + z;
			unsigned int b = x * this->zSize + z-1;
			unsigned int d = x * this->zSize + z+1;
			unsigned int e = (x+1) * this->zSize + z;

			v1 = this->zVertices[e].position - this->zVertices[a].position;
			v2 = this->zVertices[b].position - this->zVertices[d].position;
			D3DXVec3Cross(&normal, &v2, &v1);
			D3DXVec3Normalize(&normal, &normal);
			this->zVertices[x * this->zSize + z].normal = normal;
		}
	}

	// Top Normals
	for( unsigned int x = 1; x < this->zSize-1; ++x )
	{
		unsigned int a = x;
		unsigned int b = this->zSize + x - 1;
		unsigned int d = this->zSize + x + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].position - this->zVertices[a].position;
		D3DXVECTOR3 v2 = this->zVertices[d].position - this->zVertices[a].position;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v1, &v2);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Bottom Normals
	for( unsigned int x = 1; x < this->zSize-1; ++x )
	{
		unsigned int a = (this->zSize-1) * this->zSize + x;
		unsigned int b = (this->zSize-2) * this->zSize + x - 1;
		unsigned int d = (this->zSize-2) * this->zSize + x + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].position - this->zVertices[a].position;
		D3DXVECTOR3 v2 = this->zVertices[d].position - this->zVertices[a].position;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v2, &v1);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Left Normals
	for( unsigned int z = 1; z < this->zSize-1; ++z )
	{
		unsigned int a = this->zSize * z;
		unsigned int b = this->zSize * (z - 1) + 1;
		unsigned int d = this->zSize * (z + 1) + 1;

		D3DXVECTOR3 v1 = this->zVertices[b].position - this->zVertices[a].position;
		D3DXVECTOR3 v2 = this->zVertices[d].position - this->zVertices[a].position;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v2, &v1);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}

	// Right Normals
	for( unsigned int z = 1; z < this->zSize-1; ++z )
	{
		unsigned int a = this->zSize * z + this->zSize - 1;
		unsigned int b = this->zSize * (z - 1) + this->zSize - 2;
		unsigned int d = this->zSize * (z + 1) + this->zSize - 2;

		D3DXVECTOR3 v1 = this->zVertices[b].position - this->zVertices[a].position;
		D3DXVECTOR3 v2 = this->zVertices[d].position - this->zVertices[a].position;

		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm, &v1, &v2);
		D3DXVec3Normalize(&norm,&norm);

		this->zVertices[a].normal = norm;
	}
}

void Terrain::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	BUFFER_INIT_DESC bufferVerts;
	bufferVerts.ElementSize = sizeof(Vertex);
	bufferVerts.InitData = &this->zVertices[0];
	bufferVerts.NumElements = this->zVertices.size();
	bufferVerts.Type = VERTEX_BUFFER;
	bufferVerts.Usage = BUFFER_DEFAULT;

	this->zVertexBuffer = GetResourceManager()->CreateBufferResource("Terrain_Vertices", bufferVerts);

	BUFFER_INIT_DESC bufferInds;
	bufferInds.ElementSize = sizeof(unsigned int);
	bufferInds.InitData = &this->zIndices[0];
	bufferInds.NumElements = this->zIndices.size();
	bufferInds.Type = INDEX_BUFFER;
	bufferInds.Usage = BUFFER_DEFAULT;

	this->zIndexBuffer = GetResourceManager()->CreateBufferResource("Terrain_Indices", bufferInds);

	std::string textureName;
	if (this->zHeightMap)
	{
		textureName = this->zHeightMap->GetFileName();
		textureName = textureName.substr(0, textureName.size() - 4) + ".dds";
	}
	else
	{
		textureName = "media/Terrain/DefaultTerrain.png.dds";
	}
	
	this->zTextureResource = GetResourceManager()->CreateTextureResourceFromFile(textureName.c_str());
}

Terrain::~Terrain()
{
	if (this->zVertexBuffer)
		GetResourceManager()->DeleteBufferResource(this->zVertexBuffer);

	if (this->zIndexBuffer)
		GetResourceManager()->DeleteBufferResource(this->zIndexBuffer);

	if (this->zTextureResource)
		GetResourceManager()->DeleteTextureResource(this->zTextureResource);

	if (this->zHeightMap)
		delete this->zHeightMap;
}

void Terrain::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->zPosition.x, this->zPosition.y, this->zPosition.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->zScale.x, this->zScale.y, this->zScale.z);

	D3DXMATRIX world = scaling * translate;

	this->zWorldMatrix = world;
}

float Terrain::CalcHeightAt( float x, float z )
{
	float width = (float)this->zSize;
	float depth = (float)this->zSize;

	//Transform from terrain local space to "cell" space.
	float c = (x + 0.5f * this->zSize) / 1.0f;
	float d = (z - 0.5f * this->zSize) / -1.0f;

	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float A = this->GetHeightAt(row, col);
	float B = this->GetHeightAt(row, col + 1);
	float C = this->GetHeightAt(row + 1, col);
	float D = this->GetHeightAt(row + 1, col + 1);

	float ds = c - (float)col;
	float dt = d - (float)row;

	float hh = 0.0f;
	if (dt < 1.0f - ds)
	{
		float uy = B - A;
		float vy = C - A;
		hh = A + ds * uy + dt * vy;
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		hh = D + (1.0f - ds) * uy + (1.0f - dt) *vy;
	}
	return hh;
}

float Terrain::GetHeightAt(int row, int col)
{
	if (row < 0 || row >= this->zSize || col < 0 || col >= this->zSize)
		throw(MeshException(__FILE__, __LINE__, __FUNCTION__, "Out of Range"));

	return this->zVertices[row * this->zSize + col].position.y;
}

void Terrain::SetScale( const Vector3& scale )
{
	this->zScale = D3DXVECTOR3(scale.x, scale.y, scale.z);

	this->RecreateWorldMatrix();
}

bool Terrain::IsInside(const Vector2& pos)
{
	D3DXVECTOR3 maxWorldPos = D3DXVECTOR3(0.5f, this->zPosition.y, 0.5f);
	D3DXVECTOR3 minWorldPos= D3DXVECTOR3(-0.5f, this->zPosition.y, -0.5f);
	D3DXVec3TransformCoord(&maxWorldPos, &maxWorldPos, &this->zWorldMatrix);
	D3DXVec3TransformCoord(&minWorldPos, &minWorldPos, &this->zWorldMatrix);

	if (maxWorldPos.x > pos.x && 
		maxWorldPos.z > pos.y && 
		minWorldPos.x < pos.x && 
		minWorldPos.z < pos.y)
		return true;

	return false;
}