//#pragma once
//
//#include "DirectX.h"
//#include <vector>
//
//typedef std::vector<D3DXVECTOR3> VertexList;
//typedef std::vector<DWORD> IndexList;
//
//class Sky
//{
//public:
//	Sky();
//	~Sky();
//
//	void init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* cubemap, float radius);
//
//	void draw(D3DXVECTOR3 mEyePos, D3DXMATRIX mView, D3DXMATRIX mProj, bool bDeferred);
//private:
//	Sky(const Sky& rhs);
//	Sky& operator=(const Sky& rhs);
//	void BuildGeoSphere(UINT numSubdivisions, float radius, VertexList& vertices, IndexList& indices);
//	void Subdivide(VertexList& vertices, IndexList& indices);
//private:
//	ID3D11Device* md3dDevice;
//	ID3D11DeviceContext* md3dDeviceContext;
//	ID3D11Buffer* mVB;
//	ID3D11Buffer* mIB;
//
//	ID3D11ShaderResourceView* mCubeMap;
//
//	UINT mNumIndices;
//
//	ID3DX11Effect* SkyFX;
//	ID3DX11EffectTechnique* mTech;
//	ID3DX11EffectMatrixVariable* mfxWVPVar;
//	ID3DX11EffectMatrixVariable* mfxWVVar;
//	ID3DX11EffectShaderResourceVariable* mfxCubeMapVar;
//};