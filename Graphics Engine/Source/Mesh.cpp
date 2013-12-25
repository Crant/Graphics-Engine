#include "Mesh.h"
#include "ResourceManager.h"

Mesh::Mesh( D3DXVECTOR3 pos, std::string filePath )
{
	this->zFilePath = filePath;

	this->zMeshStripResource = NULL;

	this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	this->zPosition = pos;
	this->zRotationQuat = D3DXQUATERNION(0, 0, 0, 1);
	this->zScale = D3DXVECTOR3(1, 1, 1);
}

Mesh::~Mesh()
{
	if (this->zMeshStripResource)
		GetResourceManager()->DeleteMeshStripsResource(this->zMeshStripResource);
}

void Mesh::SetPosition( D3DXVECTOR3 pos )
{
	this->zPosition = pos;
	this->RecreateWorldMatrix();
}

void Mesh::SetPosition( const Vector3& pos )
{
	this->SetPosition( D3DXVECTOR3(pos.x,pos.y,pos.z) );
}

void Mesh::SetQuaternion( D3DXQUATERNION quat )
{
	this->zRotationQuat = quat;
	this->RecreateWorldMatrix();
}

void Mesh::SetQuaternion( const Vector4& quat )
{
	this->SetQuaternion( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

void Mesh::MoveBy( D3DXVECTOR3 moveby )
{
	this->zPosition += moveby; 
	this->RecreateWorldMatrix();
}

void Mesh::MoveBy( const Vector3& moveby )
{
	this->MoveBy( D3DXVECTOR3(moveby.x,moveby.y,moveby.z) );
}

void Mesh::Rotate( D3DXVECTOR3 radians )
{
	D3DXQUATERNION quaternion;
	D3DXQuaternionRotationYawPitchRoll(&quaternion, radians.y, radians.x, radians.z);
	D3DXQuaternionMultiply(&this->zRotationQuat, &this->zRotationQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Mesh::Rotate( D3DXQUATERNION quat )
{
	D3DXQuaternionMultiply(&this->zRotationQuat, &this->zRotationQuat, &quat);
	this->RecreateWorldMatrix();
}

void Mesh::Rotate( const Vector4& quat )
{
	this->Rotate( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

void Mesh::Rotate( const Vector3& radians )
{
	this->Rotate( D3DXVECTOR3(radians.x,radians.y,radians.z) );
}

void Mesh::RotateAxis( D3DXVECTOR3 around, float angle )
{
	D3DXQUATERNION quaternion = D3DXQUATERNION(0, 0, 0, 1);
	D3DXQuaternionRotationAxis(&quaternion, &around, angle);

	D3DXQuaternionMultiply(&this->zRotationQuat, &this->zRotationQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Mesh::RotateAxis( const Vector3& around, float angle )
{
	this->RotateAxis( D3DXVECTOR3(around.x,around.y,around.z), angle );
}

void Mesh::Scale( D3DXVECTOR3 scale )
{
	this->zScale.x *= scale.x;
	this->zScale.y *= scale.y;
	this->zScale.z *= scale.z;
	this->RecreateWorldMatrix();
}

void Mesh::Scale( const Vector3& scale )
{
	this->Scale( D3DXVECTOR3(scale.x,scale.y,scale.z) );
}

void Mesh::Scale( float scale )
{
	this->zScale.x *= scale;
	this->zScale.y *= scale;
	this->zScale.z *= scale;
	this->RecreateWorldMatrix();
}

void Mesh::SetScale( float scale )
{
	this->zScale.x = scale;
	this->zScale.y = scale;
	this->zScale.z = scale;
	this->RecreateWorldMatrix();
}

void Mesh::SetScale( Vector3 scale )
{
	this->SetScale(D3DXVECTOR3(scale.x, scale.y, scale.z));
}

void Mesh::SetScale( D3DXVECTOR3 scale )
{
	this->zScale.x = scale.x;
	this->zScale.y = scale.y;
	this->zScale.z = scale.z;
	this->RecreateWorldMatrix();
}

Vector3 Mesh::GetPosition() const
{
	return Vector3(this->zPosition.x, this->zPosition.y, this->zPosition.z);
}

Vector4 Mesh::GetRotationQuaternion() const
{
	return Vector4(this->zRotationQuat.x, this->zRotationQuat.y, this->zRotationQuat.z, this->zRotationQuat.w);
}

Vector3 Mesh::GetScaling() const
{
	return Vector3(this->zScale.x, this->zScale.y, this->zScale.z);
}

void Mesh::ResetRotation()
{
	this->zRotationQuat = D3DXQUATERNION(0, 0, 0, 1);
}

bool Mesh::LoadFromFile( std::string file )
{
	this->zFilePath = file;

	this->zMeshStripResource = GetResourceManager()->CreateMeshStripsResourceFromFile(zFilePath.c_str());
	if (this->zMeshStripResource != NULL)
	{
		this->zTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		return true;
	}
	else
	{
		return false;
	}

	return true;
}

void Mesh::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->zPosition.x, this->zPosition.y, this->zPosition.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->zScale.x, this->zScale.y, this->zScale.z);

	// Quaternions
	D3DXMATRIX QuatMat;
	D3DXMatrixRotationQuaternion(&QuatMat, &this->zRotationQuat); 

	D3DXMATRIX world = scaling*QuatMat*translate;

	this->zWorldMatrix = world;
}