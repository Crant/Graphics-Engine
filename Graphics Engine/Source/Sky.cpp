//#include "Sky.h"
//
//D3DX10INLINE T Min(const T& a, const T& b)
//{
//	return a < b ? a : b;
//}
//template<typename T>
//D3DX10INLINE T Max(const T& a, const T& b)
//{
//	return a > b ? a : b;
//}
//
////***************************************************************************************
//// Name: Subdivide
//// Desc: Function subdivides every input triangle into four triangles of equal area.
////***************************************************************************************
//void Sky::Subdivide(VertexList& vertices, IndexList& indices)
//{
//	VertexList vin = vertices;
//	IndexList  iin = indices;
//
//	vertices.resize(0);
//	indices.resize(0);
//
//	//       v1
//	//       *
//	//      / \
//	//     /   \
//	//  m0*-----*m1
//	//   / \   / \
//	//  /   \ /   \
//	// *-----*-----*
//	// v0    m2     v2
//
//	UINT numTris = (UINT)iin.size()/3;
//	for(UINT i = 0; i < numTris; ++i)
//	{
//		D3DXVECTOR3 v0 = vin[ iin[i*3+0] ];
//		D3DXVECTOR3 v1 = vin[ iin[i*3+1] ];
//		D3DXVECTOR3 v2 = vin[ iin[i*3+2] ];
//
//		D3DXVECTOR3 m0 = 0.5f*(v0 + v1);
//		D3DXVECTOR3 m1 = 0.5f*(v1 + v2);
//		D3DXVECTOR3 m2 = 0.5f*(v0 + v2);
//
//		vertices.push_back(v0); // 0
//		vertices.push_back(v1); // 1
//		vertices.push_back(v2); // 2
//		vertices.push_back(m0); // 3
//		vertices.push_back(m1); // 4
//		vertices.push_back(m2); // 5
//
//		indices.push_back(i*6+0);
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+5);
//
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+4);
//		indices.push_back(i*6+5);
//
//		indices.push_back(i*6+5);
//		indices.push_back(i*6+4);
//		indices.push_back(i*6+2);
//
//		indices.push_back(i*6+3);
//		indices.push_back(i*6+1);
//		indices.push_back(i*6+4);
//	}
//}
////***************************************************************************************
//// Name: BuildGeoSphere
//// Desc: Function approximates a sphere by tesselating an icosahedron.
////***************************************************************************************
//void Sky::BuildGeoSphere(UINT numSubdivisions, float radius, VertexList& vertices, IndexList& indices)
//{
//	// Put a cap on the number of subdivisions.
//	numSubdivisions = Min(numSubdivisions, UINT(5));
//
//	// Approximate a sphere by tesselating an icosahedron.
//
//	const float X = 0.525731f; 
//	const float Z = 0.850651f;
//
//	D3DXVECTOR3 pos[12] = 
//	{
//		D3DXVECTOR3(-X, 0.0f, Z),  D3DXVECTOR3(X, 0.0f, Z),  
//		D3DXVECTOR3(-X, 0.0f, -Z), D3DXVECTOR3(X, 0.0f, -Z),    
//		D3DXVECTOR3(0.0f, Z, X),   D3DXVECTOR3(0.0f, Z, -X), 
//		D3DXVECTOR3(0.0f, -Z, X),  D3DXVECTOR3(0.0f, -Z, -X),    
//		D3DXVECTOR3(Z, X, 0.0f),   D3DXVECTOR3(-Z, X, 0.0f), 
//		D3DXVECTOR3(Z, -X, 0.0f),  D3DXVECTOR3(-Z, -X, 0.0f)
//	};
//
//	DWORD k[60] = 
//	{
//		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,    
//		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,    
//		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0, 
//		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7 
//	};
//
//	vertices.resize(12);
//	indices.resize(60);
//
//	for(int i = 0; i < 12; ++i)
//		vertices[i] = pos[i];
//
//	for(int i = 0; i < 60; ++i)
//		indices[i] = k[i];
//
//	for(UINT i = 0; i < numSubdivisions; ++i)
//		Subdivide(vertices, indices);
//
//	// Project vertices onto sphere and scale.
//	for(size_t i = 0; i < vertices.size(); ++i)
//	{
//		D3DXVec3Normalize(&vertices[i], &vertices[i]);
//		vertices[i] *= radius;
//	}
//}
//struct SkyVertex
//{
//	D3DXVECTOR3 pos;
//};
// 
//Sky::Sky()
//: md3dDevice(0), mVB(0), mIB(0), mCubeMap(0)
//{
//	SkyFX = NULL;
//	mNumIndices = 0;
//}
//
//Sky::~Sky()
//{
//	SAFE_RELEASE(mVB);
//	SAFE_RELEASE(mIB);
//}
//
//void Sky::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* cubemap, float radius)
//{
//	md3dDevice = device;
//	md3dDeviceContext = deviceContext;
//	mCubeMap   = cubemap;
//
//	
//
//	mTech         = SkyFX->GetTechniqueByName("SkyTechDeferred");
//	mfxWVPVar     = SkyFX->GetVariableByName("gWVP")->AsMatrix();
//	mfxWVVar      = SkyFX->GetVariableByName("gWV")->AsMatrix();
//	mfxCubeMapVar = SkyFX->GetVariableByName("gCubeMap")->AsShaderResource();
//
//
//	std::vector<D3DXVECTOR3> vertices;
//	std::vector<DWORD> indices;
//
//	BuildGeoSphere(2, radius, vertices, indices);
//
//	std::vector<SkyVertex> skyVerts(vertices.size());
//	for(size_t i = 0; i < vertices.size(); ++i)
//	{
//		// Scale on y-axis to turn into an ellipsoid to make a flatter Sky surface
//		skyVerts[i].pos = 0.5f*vertices[i];
//	}
//
//	D3D11_BUFFER_DESC vbd;
//    vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(SkyVertex) * (UINT)skyVerts.size();
//    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vbd.CPUAccessFlags = 0;
//    vbd.MiscFlags = 0;
//    D3D11_SUBRESOURCE_DATA vinitData;
//    vinitData.pSysMem = &skyVerts[0];
//    md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB);
//
//	mNumIndices = (UINT)indices.size();
//
//	D3D11_BUFFER_DESC ibd;
//    ibd.Usage = D3D11_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
//    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    ibd.CPUAccessFlags = 0;
//    ibd.MiscFlags = 0;
//    D3D11_SUBRESOURCE_DATA iinitData;
//    iinitData.pSysMem = &indices[0];
//    md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB);
//}
//
//void Sky::draw(D3DXVECTOR3 mEyePos, D3DXMATRIX mView, D3DXMATRIX mProj, bool bDeferred)
//{
//	D3DXVECTOR3 eyePos = mEyePos;
//
//	// center Sky about eye in world space
//	D3DXMATRIX W;
//	D3DXMatrixTranslation(&W, eyePos.x, eyePos.y, eyePos.z);
//
//	D3DXMATRIX V = mView;
//	D3DXMATRIX P = mProj;
//
//	D3DXMATRIX WVP = W*V*P;
//	D3DXMATRIX WV = W*V;
//
//	mfxWVPVar->SetMatrix((float*)WVP);
//	mfxWVVar->SetMatrix((float*)WV);
//	mfxCubeMapVar->SetResource(mCubeMap);
//
//	UINT stride = sizeof(SkyVertex);
//    UINT offset = 0;
//    md3dDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
//	md3dDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
//	md3dDeviceContext->IASetInputLayout(InputLayout::PosTex2);
//	md3dDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	
//	if(bDeferred)
//	{
//		mTech = fx::SkyFX->GetTechniqueByName("SkyTechDeferred");
//	}
//	else
//	{
//		mTech = fx::SkyFX->GetTechniqueByName("SkyTechForward");
//	}
//	D3D10_TECHNIQUE_DESC techDesc;
//    mTech->GetDesc( &techDesc );
//
//    for(UINT p = 0; p < techDesc.Passes; ++p)
//    {
//        ID3DX11EffectPass* pass = mTech->GetPassByIndex(p);
//
//		pass->Apply(0, md3dDeviceContext);
//		md3dDeviceContext->DrawIndexed(mNumIndices, 0, 0);
//	}
//}