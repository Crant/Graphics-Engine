#pragma once

#include "DirectX.h"
#include "Terrain.h"
#include "iCamera.h"
#include "GraphicsEngineParameters.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 ) // warning C4512: 'Camera' : assignment operator could not be generated

class Camera : public iCamera
{
private:
	D3DXMATRIX zView;
	D3DXMATRIX zProj;
	D3DXMATRIX zViewProj;

	D3DXVECTOR3 zPosW;
	D3DXVECTOR3 zRightW;
	D3DXVECTOR3 zUpW;
	D3DXVECTOR3 zForwardW;

	bool zFollowTerrain;

	float zSpeed;
	float zAngleX;
	float zAngleY;

	bool zUpdateCamera;

	HWND z_hWnd;
	GraphicsEngineParams &zParams;

	inline D3DXVECTOR3& NormalizeVector(D3DXVECTOR3& vec)
	{
		float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
		vec.x /= length;
		vec.y /= length;
		vec.z /= length;
		return vec;
	}
	void BuildViewMatrix();
public:
	Camera(HWND z_hWnd, GraphicsEngineParams &params);
	virtual void Update(float dt);

	const D3DXMATRIX& GetViewMatrix() const;
	const D3DXMATRIX& GetProjMatrix() const;
	const D3DXMATRIX& GetViewProjMatrix() const;

	const D3DXVECTOR3& GetPositionD3D() {return this->zPosW;}

	virtual void MoveForward(float diff);
	virtual void MoveBackward(float diff);
	virtual void MoveLeft(float diff);
	virtual void MoveRight(float diff);
	virtual void MoveUp(float diff);
	virtual void MoveDown(float diff);

	virtual const Vector3& GetRightVector() const;
	virtual const Vector3& GetUpVector() const;
	virtual const Vector3& GetForwardVector() const;
	virtual const Vector3& GetPosition() const;

	virtual void SetPosition(Vector3 pos);
	virtual void SetForward(Vector3 forward);
	virtual void SetUpVector(Vector3 up);
	virtual void SetRightVector(Vector3 right);

	virtual void LookAt(Vector3 target);

	virtual const Vector3& GetForward() const;

	virtual void SetSpeed( float speed ) {this->zSpeed = speed;}

	virtual float GetSpeed() const {return this->zSpeed;}

	virtual void SetSensitivity( float sens ) {this->zParams.MouseSensativity = sens;}

	virtual float GetSensitivity() const {return this->zParams.MouseSensativity;}

	virtual void SetFollowTerrain(bool follow) {this->zFollowTerrain = follow;}

	bool GetFollowTerrain() {return this->zFollowTerrain;}

	void SetUpdate(bool update) {this->zUpdateCamera = update;}

	bool IsUpdating() {return this->zUpdateCamera;}
};

#pragma warning ( pop )