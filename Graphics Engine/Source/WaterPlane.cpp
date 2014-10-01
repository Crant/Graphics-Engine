#include "WaterPlane.h"

WaterPlane::WaterPlane()
{

}

WaterPlane::WaterPlane( D3DXVECTOR3 pCorners[4] )
{
	for (int i = 0; i < 4; i++)
	{
		this->zPoints[i] = pCorners[i];
	}
}