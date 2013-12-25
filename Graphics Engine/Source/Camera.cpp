#include "Camera.h"

#define PI 3.14159265358979323846f

Camera::Camera( HWND z_hWnd, GraphicsEngineParams &params )
	: zParams(params)
{
	this->z_hWnd = z_hWnd;

	this->zPosW = D3DXVECTOR3(0, 0, 0);
	this->zAngleX = 0.0f;
	this->zAngleY = 0.0f;

	this->zSpeed = 5.0f;

	float radians = this->zParams.FOV * 0.01745f;
	D3DXMatrixPerspectiveFovLH(
		&this->zProj, 
		this->zParams.FOV * 0.01745f, 
		this->zParams.WindowWidth / this->zParams.WindowHeight, 
		this->zParams.NearClip, 
		this->zParams.FarClip);

	D3DXVECTOR3 at = this->zPosW + this->zForwardW;

	D3DXMatrixLookAtLH(&this->zView, &this->zPosW, &at, &this->zUpW);

	D3DXMatrixMultiply(&this->zViewProj, &this->zView, &this->zProj);

	this->zFollowTerrain = false;
}

void Camera::Update( float dt )
{
	POINT p;

	if (GetCursorPos(&p))
	{
		if (ScreenToClient(this->z_hWnd, &p))
		{
			float diffX = (this->zParams.WindowWidth * 0.5f) - (float)p.x;
			float diffY = (this->zParams.WindowHeight * 0.5f) - (float)p.y;

			this->zAngleX += diffX * (this->zParams.MouseSensativity * 0.00001f);
			this->zAngleY += diffY * (this->zParams.MouseSensativity * 0.00001f);

			fmod(this->zAngleX, 2 * PI);
			fmod(this->zAngleY, 2 * PI);

			//Clamp Angle X between 0 & PI
			if(this->zAngleX > 2.0f * PI)
				this->zAngleX -= 2.0f * PI;
			if(this->zAngleX < 0)
				this->zAngleX += 2.0f * PI;

			//Clamp Angle Y between -PI & PI
			if(this->zAngleY > PI)
				this->zAngleY = PI;
			if(this->zAngleY < -PI)
				this->zAngleY = -PI;

			this->zForwardW.x = cos(this->zAngleX);
			this->zForwardW.y = this->zAngleY;
			this->zForwardW.z = sin(this->zAngleX);

			D3DXVec3Normalize(&this->zForwardW, &this->zForwardW);

			//Calculate new Up Vector
			Vector3 yAxis = Vector3(0, 1, 0);
			Vector3 tmpForward = Vector3();
			tmpForward.x = this->zForwardW.x;
			tmpForward.y = this->zForwardW.y;
			tmpForward.z = this->zForwardW.z;

			Vector3 rightVec = yAxis.GetCrossProduct(tmpForward);
			rightVec.Normalize();

			this->zRightW.x = rightVec.x;
			this->zRightW.y = rightVec.y;
			this->zRightW.z = rightVec.z;

			Vector3 tmpUp = Vector3();
			tmpUp = tmpForward.GetCrossProduct(rightVec);
			tmpUp.Normalize();

			this->zUpW.x = tmpUp.x;
			this->zUpW.y = tmpUp.y;
			this->zUpW.z = tmpUp.z;
		}
	}

	this->BuildViewMatrix();

	D3DXMatrixPerspectiveFovLH(&this->zProj, this->zParams.FOV * 0.01745f,
		this->zParams.WindowWidth / this->zParams.WindowHeight, this->zParams.NearClip, this->zParams.FarClip);

	D3DXMatrixMultiply(&this->zViewProj, &this->zView, &this->zProj);
}

void Camera::BuildViewMatrix()
{
	//Keep camera's axes orthogonal to each other and of unit length
	D3DXVec3Normalize(&this->zForwardW, &this->zForwardW);

	D3DXVec3Cross(&this->zUpW, &this->zForwardW, &this->zRightW);
	D3DXVec3Normalize(&this->zUpW, &this->zUpW);

	D3DXVec3Cross(&this->zRightW, &this->zUpW, &this->zForwardW);
	D3DXVec3Normalize(&this->zRightW, &this->zRightW);

	// Fill in the view matrix entries;
	float x = -D3DXVec3Dot(&this->zPosW, &this->zRightW);
	float y = -D3DXVec3Dot(&this->zPosW, &this->zUpW);
	float z = -D3DXVec3Dot(&this->zPosW, &this->zForwardW);

	this->zView(0,0) = this->zRightW.x;
	this->zView(1,0) = this->zRightW.y;
	this->zView(2,0) = this->zRightW.z;
	this->zView(3,0) = x;

	this->zView(0,1) = this->zUpW.x;
	this->zView(1,1) = this->zUpW.y;
	this->zView(2,1) = this->zUpW.z;
	this->zView(3,1) = y;

	this->zView(0,2) = this->zForwardW.x;
	this->zView(1,2) = this->zForwardW.y;
	this->zView(2,2) = this->zForwardW.z;
	this->zView(3,2) = z;

	this->zView(0,3) = 0.0f;
	this->zView(1,3) = 0.0f;
	this->zView(2,3) = 0.0f;
	this->zView(3,3) = 1.0f;
}

const D3DXMATRIX& Camera::GetViewMatrix() const
{
	return this->zView;
}

const D3DXMATRIX& Camera::GetProjMatrix() const
{
	return this->zProj;
}

const D3DXMATRIX& Camera::GetViewProjMatrix() const
{
	return this->zViewProj;
}

void Camera::MoveForward( float diff )
{
	this->zPosW += this->zForwardW * diff * this->zSpeed;
}

void Camera::MoveBackward( float diff )
{
	this->zPosW -= this->zForwardW * diff * this->zSpeed;
}

void Camera::MoveLeft( float diff )
{
	this->zPosW -= this->zRightW * diff * this->zSpeed;
}

void Camera::MoveRight( float diff )
{
	this->zPosW += this->zRightW * diff * this->zSpeed;
}

void Camera::MoveUp( float diff )
{
	this->zPosW += this->zUpW * diff * this->zSpeed;
}

void Camera::MoveDown( float diff )
{
	this->zPosW -= this->zUpW * diff * this->zSpeed;
}

const Vector3& Camera::GetRightVector() const
{
	return Vector3(this->zRightW.x, this->zRightW.y, this->zRightW.z);
}

const Vector3& Camera::GetUpVector() const
{
	return Vector3(this->zUpW.x, this->zUpW.y, this->zUpW.z);
}

const Vector3& Camera::GetForwardVector() const
{
	return Vector3(this->zForwardW.x, this->zForwardW.y, this->zForwardW.z);
}

const Vector3& Camera::GetPosition() const
{
	return Vector3(this->zPosW.x, this->zPosW.y, this->zPosW.z);
}

void Camera::SetPosition( Vector3 pos )
{
	this->zPosW = D3DXVECTOR3(pos.x, pos.y, pos.z);
}

void Camera::SetForward( Vector3 forward )
{
	this->zForwardW = D3DXVECTOR3(forward.x, forward.y, forward.z);
}

void Camera::SetUpVector( Vector3 up )
{
	this->zUpW = D3DXVECTOR3(up.x, up.y, up.z);
}

void Camera::SetRightVector( Vector3 right )
{
	this->zRightW = D3DXVECTOR3(right.x, right.y, right.z);
}

void Camera::LookAt( Vector3 target )
{
	this->zForwardW = D3DXVECTOR3(target.x, target.y, target.z) - this->zPosW;
	D3DXVec3Normalize(&this->zForwardW, &this->zForwardW);

	this->zAngleX = -atan2(this->zForwardW.x * 0 - 1 * this->zForwardW.z, this->zForwardW.x * 1 + this->zForwardW.z * 0);
	this->zAngleY = asin(this->zForwardW.y);
}

const Vector3& Camera::GetForward() const
{
	return Vector3(this->zForwardW.x, this->zForwardW.y, this->zForwardW.z);
}