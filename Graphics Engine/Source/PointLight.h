#pragma once

#include "iPointLight.h"
#include "DirectX.h"
#include "Sphere.h"

#define VIEWS 6

class PointLight : public iPointLight
{
private:
	D3DXMATRIX zWorldMatrix;
	D3DXMATRIX zViewMatrix[6];
	D3DXMATRIX zProjectionMatrix;

	D3DXVECTOR3 zColor;
	D3DXVECTOR3 zPosition;

	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;

	unsigned int zShadowMapSize;
	float zRadius;
	float zIntensity;

	Sphere* zSphere;

	//Shadow Map
	bool zHasShadows;

	ID3D11RenderTargetView* zColorCubeMapRTV;
	ID3D11ShaderResourceView* zColorCubeMapSRV;
	ID3D11DepthStencilView* zDepthCubeMapDSV;
	ID3D11RenderTargetView* zColorCubeMapArrayRTV[VIEWS];
	ID3D11DepthStencilView* zColorCubeMapArrayDSV;

private:
	void BuildViewMatrix(D3DXMATRIX& view, D3DXVECTOR3 mLookW, D3DXVECTOR3 mPosW, D3DXVECTOR3 mUpW);
	void RecreateWorldMatrix();
	void UpdateViews();

	void CreateShadowMap();
	void CreateShadowMapCube();

public:
	PointLight();
	PointLight(D3DXVECTOR3 position, float radius, float intensity, bool enableShadows);
	virtual ~PointLight();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void SetSmapSize(int size);
	unsigned int GetShadowMapSize() {return this->zShadowMapSize;}

	D3DXMATRIX GetWorldMatrix() {return this->zWorldMatrix;}
	D3DXMATRIX GetViewMatrix(const int index);
	D3DXMATRIX GetProjMatrix() {return this->zProjectionMatrix;}

	D3DXVECTOR3 GetPositionD3D() {return this->zPosition;}

	virtual Vector3 GetPosition();
	virtual Vector3 GetColor();

	virtual void SetColor(const Vector3& color);
	void SetColor(const D3DXVECTOR3& color) {this->zColor = color;}

	virtual void SetPosition(const Vector3& position);
	void SetPosition(const D3DXVECTOR3& position);

	virtual float GetRadius() {return this->zRadius;}
	virtual float GetIntensity() {return this->zIntensity;}

	virtual void SetRadius(float radius);
	virtual void SetIntensity(float intensity) {this->zIntensity = intensity;}

	BufferResource* GetVertexBuffer() {return this->zSphere->GetVertexBuffer();}
	BufferResource* GetIndexBuffer() {return this->zSphere->GetIndexBuffer();}

	virtual bool IsShadowsEnabled() {return this->zHasShadows;};
	/*! Enable or Disable shadows.*/
	virtual void SetShadows(const bool& value) {this->zHasShadows = value;}

	ID3D11RenderTargetView* GetRTV() {return this->zColorCubeMapRTV;}
	ID3D11ShaderResourceView* GetSRV() {return this->zColorCubeMapSRV;}
	ID3D11DepthStencilView* GetDSV() {return this->zDepthCubeMapDSV;}
	ID3D11RenderTargetView* GetRTVArray(int index) {return this->zColorCubeMapArrayRTV[index];}
	ID3D11DepthStencilView* GetDSVArray() {return this->zColorCubeMapArrayDSV;}
};