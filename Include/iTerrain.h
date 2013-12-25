#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iTerrain
	{	
		protected:
			iTerrain() {}
			virtual ~iTerrain() {}

		public:
			//GET-FUNCTIONS.
			//Object data
			/*	Get scale of terrain in x, y and z.	*/
			virtual Vector3 GetScale() const = 0;
			/*  Get Position Of Terrain. */
			virtual Vector3 GetPosition() const = 0;

			//Vertex data
			/* Returns the Y-position in local space on the terrain at x,z in local space. */
			virtual float CalcHeightAt(float x, float z) = 0; 

			//SET-FUNCTIONS.
			//Object data 
			/* Sets the scale for the terrain. */
			virtual void SetScale(const Vector3& scale) = 0;
			/* Set diffuse color. */
			//virtual void SetDiffuseColor(const Vector3& color) = 0;

			//Vertex data
			/* Data = array of floats containing y-values for each vertex. */
			//virtual void SetHeightMap(float const* const data) = 0;
			/* Data = array of floats containing normals (x,y,z) for each vertex. */
			//virtual void SetNormals(float const* const data) = 0;
	};
}
