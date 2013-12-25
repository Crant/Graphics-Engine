#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iMesh
	{	
	protected:
		virtual ~iMesh() {};
		iMesh() {};

	public:
		// Special Color
		//virtual void SetSpecialColor(COLOR specialColor) = 0;
		//virtual COLOR GetSpecialColor() const = 0;

		virtual void SetPosition(const Vector3& pos) = 0;
		virtual void SetQuaternion(const Vector4& quat) = 0;
		virtual void MoveBy(const Vector3& moveby) = 0;
		virtual void Rotate(const Vector3& radians) = 0;
		virtual void Rotate(const Vector4& quat) = 0;
		virtual void RotateAxis(const Vector3& around, float angle) = 0;
		virtual void Scale(const Vector3& scale) = 0;
		virtual void Scale(float scale) = 0;
		virtual void SetScale(float scale) = 0;
		virtual void SetScale(Vector3 scale) = 0;
		virtual Vector3 GetPosition() const = 0;
		virtual Vector4 GetRotationQuaternion() const = 0;
		virtual Vector3 GetScaling() const = 0;
		virtual void ResetRotation() = 0;
		//virtual iBillboard* GetBillboard() = 0;
		//virtual bool GetDontRenderFlag() = 0;
		//virtual void DontRender(bool flag) = 0;
		//virtual bool IsUsingInvisibility() const = 0;
		//virtual void UseInvisibilityEffect(bool flag) = 0;

		//virtual void RotateVectorByMeshesRotation(Vector3& vec) = 0;

		virtual const char* GetFilePath() const { return ""; }
	};
}