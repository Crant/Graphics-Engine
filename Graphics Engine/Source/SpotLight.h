#pragma once

#include "iSpotLight.h"
#include "DirectX.h"
#include "Sphere.h"
#include "GraphicsEngineParameters.h"
#include "TextureResource.h"

#define VIEWS 6

class SpotLight : public iSpotLight
{
private:
	D3DXMATRIX zWorldMatrix;
	D3DXMATRIX zViewMatrix;
	D3DXMATRIX zProjectionMatrix;

	D3DXVECTOR3 zColor;
	D3DXVECTOR3 zPosition;
	D3DXVECTOR3 zDirection;

	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;

	unsigned int zShadowMapSize;
	float zIntensity;
	float zRadius;
	float zFOV;
	float zFarZ;
	float zNearZ;

	Sphere* zSphere;

	//Shadow Map
	bool zHasShadows;

	ID3D11RenderTargetView* zColorMapRTV;
	ID3D11ShaderResourceView* zColorMapSRV;
	ID3D11DepthStencilView* zDepthMapDSV;
	ID3D11ShaderResourceView* zDepthMapSRV;
	TextureResource* zRadialAttTexture;

private:
	void BuildViewMatrix(D3DXMATRIX& view, D3DXVECTOR3 mLookW, D3DXVECTOR3 mPosW, D3DXVECTOR3 mUpW);
	void RecreateWorldMatrix();

	void CreateShadowMap();

public:
	SpotLight();
	SpotLight(D3DXVECTOR3 position, D3DXVECTOR3 direction, float intensity, bool enableShadows, float fov, float farZ, float nearZ);
	virtual ~SpotLight();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void SetSmapSize(int size);
	unsigned int GetShadowMapSize() {return this->zShadowMapSize;}

	D3DXMATRIX GetWorldMatrix() {return this->zWorldMatrix;}
	D3DXMATRIX GetViewMatrix() {return this->zViewMatrix;}
	D3DXMATRIX GetProjMatrix() {return this->zProjectionMatrix;}

	D3DXVECTOR3 GetPositionD3D() {return this->zPosition;}

	virtual Vector3 GetPosition();
	virtual Vector3 GetColor();
	virtual Vector3 GetDirection();

	virtual void SetColor(const Vector3& color);
	void SetColor(const D3DXVECTOR3& color) {this->zColor = color;}

	virtual void SetPosition(const Vector3& position);
	void SetPosition(const D3DXVECTOR3& position);

	virtual void SetDirection(const Vector3& direction);
	void SetDirection(const D3DXVECTOR3& direction);
	
	virtual float GetIntensity() {return this->zIntensity;}

	virtual void SetIntensity(float intensity) {this->zIntensity = intensity;}

	BufferResource* GetVertexBuffer() {return this->zSphere->GetVertexBuffer();}
	BufferResource* GetIndexBuffer() {return this->zSphere->GetIndexBuffer();}

	virtual bool IsShadowsEnabled() {return this->zHasShadows;};
	/*! Enable or Disable shadows.*/
	virtual void SetShadows(const bool& value) {this->zHasShadows = value;}

	ID3D11RenderTargetView* GetRTV() {return this->zColorMapRTV;}
	ID3D11ShaderResourceView* GetSRV() {return this->zColorMapSRV;}
	ID3D11DepthStencilView* GetDSV() {return this->zDepthMapDSV;}
};