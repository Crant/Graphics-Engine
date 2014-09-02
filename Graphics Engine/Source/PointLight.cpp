#include "PointLight.h"
#include "Exceptions.h"

#define PI 3.14159265358979323846f

PointLight::PointLight()
{
	this->zDevice = NULL;
	this->zDeviceContext = NULL;
	this->zSphere = NULL;

	this->zColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->zPosition = D3DXVECTOR3();
	this->zRadius = 0.0f;
	this->zIntensity = 0.0f;

	D3DXMatrixTranslation(&this->zWorldMatrix, this->zPosition.x, this->zPosition.y, this->zPosition.z);
	D3DXMatrixPerspectiveFovLH(&this->zProjectionMatrix, PI * 0.5f, 1.0f, 1.0f, this->zRadius);
}

PointLight::PointLight( D3DXVECTOR3 position, float radius, float intensity, bool enableShadows )
{
	this->zShadowMapSize = 1024;
	this->zDevice = NULL;
	this->zDeviceContext = NULL;

	this->zColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->zPosition = position;
	this->zRadius = radius;
	this->zIntensity = intensity;
	this->zHasShadows = enableShadows;

	this->zSphere = new Sphere();

	D3DXMatrixPerspectiveFovLH(&this->zProjectionMatrix, PI * 0.5f, 1.0f, 1.0f, this->zRadius);

	this->RecreateWorldMatrix();

	this->UpdateViews();
}

void PointLight::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->zPosition.x, this->zPosition.y, this->zPosition.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->zRadius, this->zRadius, this->zRadius);

	D3DXMATRIX world = scaling*translate;

	this->zWorldMatrix = world;
}

PointLight::~PointLight()
{
	if (this->zSphere)
		delete this->zSphere;

	SAFE_RELEASE(zColorCubeMapRTV);
	SAFE_RELEASE(zColorCubeMapSRV);
	SAFE_RELEASE(zDepthCubeMapDSV);
	SAFE_RELEASE(zColorCubeMapArrayDSV);

	for(int i = 0; i < VIEWS; i++)
	{
		SAFE_RELEASE(zColorCubeMapArrayRTV[i]);
	}
}

void PointLight::Init( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	for(int i = 0; i < VIEWS; i++)
	{
		this->zColorCubeMapArrayRTV[i] = NULL;
	}
	this->zColorCubeMapRTV = NULL;
	this->zColorCubeMapSRV = NULL;
	this->zDepthCubeMapDSV = NULL;
	this->zColorCubeMapArrayDSV = NULL;

	this->zDevice = device;
	this->zDeviceContext = deviceContext;

	this->zSphere->Init(device, deviceContext, this->zRadius, 50, 50);

	try
	{
		if(this->zHasShadows)
			this->CreateShadowMapCube();
	}
	catch (GraphicalException e)
	{
		
	}
	
}

void PointLight::UpdateViews()
{
	this->BuildViewMatrix(this->zViewMatrix[0], D3DXVECTOR3( 1.0f,  0.0f,  0.0f), this->zPosition, D3DXVECTOR3(0.0f, 1.0f,  0.0f));
	this->BuildViewMatrix(this->zViewMatrix[1], D3DXVECTOR3(-1.0f,  0.0f,  0.0f), this->zPosition, D3DXVECTOR3(0.0f, 1.0f,  0.0f));
	this->BuildViewMatrix(this->zViewMatrix[2], D3DXVECTOR3( 0.0f,  1.0f,  0.0f), this->zPosition, D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	this->BuildViewMatrix(this->zViewMatrix[3], D3DXVECTOR3( 0.0f, -1.0f,  0.0f), this->zPosition, D3DXVECTOR3(0.0f, 0.0f,  1.0f));
	this->BuildViewMatrix(this->zViewMatrix[4], D3DXVECTOR3( 0.0f,  0.0f,  1.0f), this->zPosition, D3DXVECTOR3(0.0f, 1.0f,  0.0f));
	this->BuildViewMatrix(this->zViewMatrix[5], D3DXVECTOR3( 0.0f,  0.0f, -1.0f), this->zPosition, D3DXVECTOR3(0.0f, 1.0f,  0.0f));
}

void PointLight::BuildViewMatrix( D3DXMATRIX& view, D3DXVECTOR3 mLookW, D3DXVECTOR3 mPosW, D3DXVECTOR3 mUpW )
{
	D3DXVECTOR3 mRightW;

	//Keep camera's axes orthogonal to each other and of unit length
	D3DXVec3Normalize(&mLookW, &mLookW);

	D3DXVec3Cross(&mRightW, &mUpW, &mLookW);
	D3DXVec3Normalize(&mRightW, &mRightW);

	D3DXVec3Cross(&mUpW, &mLookW, &mRightW);
	D3DXVec3Normalize(&mUpW, &mUpW);

	// Fill in the view matrix entries;
	float x = -D3DXVec3Dot(&mPosW, &mRightW);
	float y = -D3DXVec3Dot(&mPosW, &mUpW);
	float z = -D3DXVec3Dot(&mPosW, &mLookW);

	view(0,0) = mRightW.x;
	view(1,0) = mRightW.y;
	view(2,0) = mRightW.z;
	view(3,0) = x;

	view(0,1) = mUpW.x;
	view(1,1) = mUpW.y;
	view(2,1) = mUpW.z;
	view(3,1) = y;

	view(0,2) = mLookW.x;
	view(1,2) = mLookW.y;
	view(2,2) = mLookW.z;
	view(3,2) = z;

	view(0,3) = 0.0f;
	view(1,3) = 0.0f;
	view(2,3) = 0.0f;
	view(3,3) = 1.0f;
}

void PointLight::SetSmapSize( int size )
{
	this->zShadowMapSize = size;
}

void PointLight::SetPosition(const Vector3& position)
{
	this->SetPosition(D3DXVECTOR3(position.x, position.y, position.z));
}

void PointLight::SetPosition(const D3DXVECTOR3& position )
{
	this->zPosition = position;
	this->RecreateWorldMatrix();

	this->UpdateViews();
}

Vector3 PointLight::GetPosition()
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}

Vector3 PointLight::GetColor()
{
	return Vector3(this->zColor.x * 255.0f, this->zColor.y * 255.0f, this->zColor.z * 255.0f);
}

void PointLight::SetColor(const Vector3& color )
{
	this->SetColor(D3DXVECTOR3(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f));
}

void PointLight::SetRadius( float radius )
{
	this->zRadius = radius;
	this->RecreateWorldMatrix();
}

D3DXMATRIX PointLight::GetViewMatrix( const int index )
{
	if(index < VIEWS && index >= 0)
		return this->zViewMatrix[index];
	
	throw("out of bounds");
}

void PointLight::CreateShadowMapCube()
{
	HRESULT hr;

	ID3D11Texture2D* depthMap = NULL;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = this->zShadowMapSize;
	texDesc.Height = this->zShadowMapSize;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = this->zDevice->CreateTexture2D(&texDesc, 0, &depthMap);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.ArraySize=6;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	hr = this->zDevice->CreateDepthStencilView(depthMap, &dsvDesc, &this->zDepthCubeMapDSV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	dsvDesc.Texture2DArray.ArraySize = 1;
	hr = this->zDevice->CreateDepthStencilView(depthMap, &dsvDesc, &this->zColorCubeMapArrayDSV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	ID3D11Texture2D* colorMap = NULL;

	D3D11_TEXTURE2D_DESC texDesc2;
	texDesc2.Width = this->zShadowMapSize;
	texDesc2.Height = this->zShadowMapSize;
	texDesc2.MipLevels = 1;
	texDesc2.ArraySize = 6;
	texDesc2.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc2.SampleDesc.Count = 1;
	texDesc2.SampleDesc.Quality = 0;
	texDesc2.Usage = D3D11_USAGE_DEFAULT;
	texDesc2.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc2.CPUAccessFlags = 0;
	texDesc2.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = this->zDevice->CreateTexture2D(&texDesc2, 0, &colorMap);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	_ASSERT(colorMap);

	colorMap->GetDesc(&texDesc2);

	int iMipLevels = texDesc2.MipLevels;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTVDesc.Texture2DArray.MipSlice = 0;
	RTVDesc.Texture2DArray.ArraySize = 6;
	RTVDesc.Texture2DArray.FirstArraySlice=0;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.Texture2D.MipLevels = iMipLevels;
	SRVDesc.Texture2D.MostDetailedMip = 0;


	hr = this->zDevice->CreateRenderTargetView(colorMap, &RTVDesc, &zColorCubeMapRTV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	hr = this->zDevice->CreateShaderResourceView(colorMap, &SRVDesc, &zColorCubeMapSRV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	//Creates a RTV for each cube Face
	RTVDesc.Texture2DArray.ArraySize = 1;
	for(int i = 0; i < VIEWS; i++)
	{
		RTVDesc.Texture2DArray.FirstArraySlice = i;
		hr = this->zDevice->CreateRenderTargetView(colorMap, &RTVDesc, &zColorCubeMapArrayRTV[i]);

		if(FAILED(hr))
			throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
	}

	SAFE_RELEASE(depthMap);
	SAFE_RELEASE(colorMap);
}