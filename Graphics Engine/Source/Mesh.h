#pragma once

#include "ObjLoader.h"
#include "iMesh.h"
#include "Vector.h"
#include "MeshStripResource.h"

class Mesh : public virtual iMesh
{
protected:
	std::string zFilePath;

	D3D_PRIMITIVE_TOPOLOGY zTopology;

	MeshStripResource* zMeshStripResource;

	D3DXVECTOR3 zPosition;
	D3DXQUATERNION zRotationQuat;
	D3DXVECTOR3 zScale;
	D3DXMATRIX zWorldMatrix;

public:
	Mesh(D3DXVECTOR3 pos, std::string filePath);
	virtual ~Mesh();
	// ID
	virtual const char* GetFilePath() const { return this->zFilePath.c_str(); }

	MeshStripResource* GetMeshStripsResourcePointer() { return this->zMeshStripResource; }

	// Transformation
	void SetPosition(D3DXVECTOR3 pos);
	void SetQuaternion(D3DXQUATERNION quat);
	void MoveBy(D3DXVECTOR3 moveby);
	void Rotate(D3DXVECTOR3 radians);
	void Rotate(D3DXQUATERNION quat);
	void RotateAxis(D3DXVECTOR3 around, float angle);
	void Scale(D3DXVECTOR3 scale);

	// Is used internally when needed, but can be used from the outside for debugging.
	void RecreateWorldMatrix();

	// iMesh Interface Functions
	virtual void RotateAxis(const Vector3& around, float angle);
	virtual void Rotate(const Vector4& quat);
	virtual void SetPosition(const Vector3& pos);
	virtual void SetQuaternion(const Vector4& quat);
	virtual void MoveBy(const Vector3& moveby);
	virtual void Rotate(const Vector3& radians);
	virtual void Scale(const Vector3& scale);
	virtual void Scale(float scale);
	virtual void SetScale(float scale);
	virtual void SetScale(Vector3 scale);
	virtual void SetScale(D3DXVECTOR3 scale);
	virtual Vector3 GetPosition() const;
	virtual Vector4 GetRotationQuaternion() const;
	virtual Vector3 GetScaling() const;
	virtual void ResetRotation();

	D3DXMATRIX& GetWorldMatrix() { return this->zWorldMatrix; }
	D3DXVECTOR3& GetScalingD3D() { return this->zScale; }

	// Load Mesh From File
	virtual bool LoadFromFile(std::string file);

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->zTopology; }

	// Get Strips
	virtual std::vector<MeshStrip*> GetStrips() = 0;
};