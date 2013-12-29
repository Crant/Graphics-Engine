#include "SpotLight.h"
#include "Exceptions.h"

SpotLight::SpotLight()
{
	this->zDevice = NULL;
	this->zDeviceContext = NULL;
	this->zSphere = NULL;

	this->zColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->zPosition = D3DXVECTOR3();
	this->zDirection = D3DXVECTOR3();
	this->zIntensity = 0.0f;

	D3DXMatrixTranslation(&this->zWorldMatrix, this->zPosition.x, this->zPosition.y, this->zPosition.z);
}

SpotLight::SpotLight(D3DXVECTOR3 position, D3DXVECTOR3 direction, float intensity, bool enableShadows, float fov, float farZ, float nearZ)
{
	this->zShadowMapSize = 1024;
	this->zDevice = NULL;
	this->zDeviceContext = NULL;
	this->zRadialAttTexture = NULL;
	this->zColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	this->zPosition = position;
	this->zDirection = direction;
	this->zIntensity = intensity;
	this->zHasShadows = enableShadows;

	this->zRadius = tan(fov / 2.0f) * 2.0f * farZ;

	this->zSphere = new Sphere();

	D3DXMatrixPerspectiveFovLH(&this->zProjectionMatrix, fov, 1.0f, nearZ, this->zRadius);

	this->RecreateWorldMatrix();

}

SpotLight::~SpotLight()
{
	if (this->zSphere)
		delete this->zSphere;

	SAFE_RELEASE(zColorMapRTV);
	SAFE_RELEASE(zColorMapSRV);
	SAFE_RELEASE(zDepthMapDSV);
}

void SpotLight::BuildViewMatrix( D3DXMATRIX& view, D3DXVECTOR3 mLookW, D3DXVECTOR3 mPosW, D3DXVECTOR3 mUpW )
{

}

void SpotLight::RecreateWorldMatrix()
{
	if(this->zDirection == D3DXVECTOR3(0, 0, 0))
	{
		this->zDirection = D3DXVECTOR3(0, -1, 0);
	}
	D3DXVECTOR3 up;
	D3DXVec3Normalize(&this->zDirection, &this->zDirection);
	D3DXVec3Cross(&up, &this->zDirection, &D3DXVECTOR3(0, 1, 0));
	if(up == D3DXVECTOR3(0, 0, 0))
	{
		up = D3DXVECTOR3(1, 0, 0);
	}
	else
	{
		up = D3DXVECTOR3(0, 1 , 0);
	}

	D3DXMATRIX mTrans, mRot, mScale, semiProduct;
	D3DXMatrixScaling(&mScale, this->zRadius, this->zRadius, this->zFarZ);
	D3DXMatrixTranslation(&mTrans, this->zPosition.x, this->zPosition.y, this->zPosition.z);
	D3DXMatrixPerspectiveLH(&this->zProjectionMatrix, this->zFOV, 1.0f, this->zNearZ, this->zFarZ);
	this->BuildViewMatrix(this->zViewMatrix, this->zDirection, this->zPosition, up);
	D3DXQUATERNION Q;
	D3DXVECTOR3 S, P;

	D3DXMatrixInverse(&semiProduct, 0, &this->zViewMatrix);
	semiProduct = mScale * semiProduct;

	D3DXMatrixDecompose(&S, &Q, &P, &semiProduct);

	D3DXMatrixRotationQuaternion(&mRot, &Q);
	this->zWorldMatrix = mScale * mRot * mTrans;
}

void SpotLight::CreateShadowMap()
{
	HRESULT hr;
	ID3D11Texture2D* depthMap = NULL;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = this->zShadowMapSize;
	texDesc.Height = this->zShadowMapSize;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = this->zDevice->CreateTexture2D(&texDesc, 0, &depthMap);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;

	hr = this->zDevice->CreateDepthStencilView(depthMap, &dsvDesc, &this->zDepthMapDSV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	hr = this->zDevice->CreateShaderResourceView(depthMap, &srvDesc, &this->zDepthMapSRV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	SAFE_RELEASE(depthMap);

	ID3D11Texture2D* colorMap = 0;

	D3D11_TEXTURE2D_DESC texDescColor;
	texDescColor.Width = this->zShadowMapSize;
	texDescColor.Height = this->zShadowMapSize;
	texDescColor.MipLevels = 0;
	texDescColor.ArraySize = 1;
	texDescColor.Format = DXGI_FORMAT_R32_TYPELESS;
	texDescColor.SampleDesc.Count = 1;
	texDescColor.SampleDesc.Quality = 0;
	texDescColor.Usage = D3D11_USAGE_DEFAULT;
	texDescColor.BindFlags = D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_RENDER_TARGET;
	texDescColor.CPUAccessFlags = 0;
	texDescColor.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	this->zDevice->CreateTexture2D(&texDescColor, 0, &colorMap);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	_ASSERT( colorMap );
	colorMap->GetDesc( &texDescColor );
	int iMipLevels = texDescColor.MipLevels;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	hr = this->zDevice->CreateRenderTargetView(colorMap, &RTVDesc, &this->zColorMapRTV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;

	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.Texture2D.MipLevels = iMipLevels;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	hr = this->zDevice->CreateShaderResourceView(colorMap, &SRVDesc, &this->zColorMapSRV);

	if(FAILED(hr))
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));

	SAFE_RELEASE(colorMap);
}

void SpotLight::Init( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	this->zColorMapRTV = NULL;
	this->zColorMapSRV = NULL;
	this->zDepthMapDSV = NULL;

	this->zDevice = device;
	this->zDeviceContext = deviceContext;

	this->zSphere->Init(device, deviceContext, 1.0f, 50, 50);

	try
	{
		if(this->zHasShadows)
			this->CreateShadowMap();
	}
	catch (GraphicalException e)
	{

	}
}

void SpotLight::SetSmapSize( int size )
{
	this->zShadowMapSize = size;
}

Vector3 SpotLight::GetPosition()
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}

void SpotLight::SetColor(const Vector3& color )
{
	this->SetColor(D3DXVECTOR3(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f));
}

Vector3 SpotLight::GetColor()
{
	return Vector3(this->zColor.x, this->zColor.y, this->zColor.z);
}

void SpotLight::SetPosition( const Vector3& position )
{
	this->SetPosition(D3DXVECTOR3(position.x, position.y, position.z));
}

void SpotLight::SetPosition( const D3DXVECTOR3& position )
{
	this->zPosition = position;
}

Vector3 SpotLight::GetDirection()
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}

void SpotLight::SetDirection( const Vector3& direction )
{
	this->SetDirection(D3DXVECTOR3(direction.x, direction.y, direction.z));
}

void SpotLight::SetDirection( const D3DXVECTOR3& direction )
{
	this->zDirection = direction;
}