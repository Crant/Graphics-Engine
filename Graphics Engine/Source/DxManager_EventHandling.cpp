#include "DxManager.h"
#include "DxManager_Events.h"

void DxManager::HandleSkyBoxEvent( SkyBoxEvent* sbe )
{
	if (sbe->IsAdding())
	{
		if (this->zSkyBox)
			delete this->zSkyBox;

		this->zSkyBox = sbe->GetSkyBox();
	}
}

void DxManager::HandleStaticMeshEvent(StaticMeshEvent* me)
{
	if(me->IsAdding())
	{
		this->zObjects.push_back(me->GetStaticMesh());
	}
	else
	{
		StaticMesh* mesh = me->GetStaticMesh();
		for(auto it = this->zObjects.begin(); it != this->zObjects.end(); it++)
		{
			if((*it) == mesh)
			{
				this->zObjects.erase(it);
				delete mesh;
				mesh = NULL;
				return;
			}
		}
	}
}

void DxManager::HandleTerrainEvent( TerrainEvent* te )
{
	if (te->IsAdding())
	{
		te->GetTerrain()->Init(this->Dx_Device, this->Dx_DeviceContext);
		this->zTerrains.push_back(te->GetTerrain());
	}
	else
	{
		Terrain* terrain = te->GetTerrain();

		for(auto it_terrain = this->zTerrains.begin(); it_terrain != this->zTerrains.end(); it_terrain++)
		{
			Terrain* temp = (*it_terrain);
			if (temp == terrain)
			{
				this->zTerrains.erase(it_terrain);
				delete terrain;
				terrain = NULL;
				return;
			}
		}
	}
}

void DxManager::HandleWaterPlaneEvent(WaterPlaneEvent* wpe)
{
	if (wpe->IsAdding())
	{
		//wpe->GetWaterPlane()->Init(this->Dx_Device, this->Dx_DeviceContext);
		this->zWaterPlanes.push_back(wpe->GetWaterPlane());
	}
	else
	{
		WaterPlane* waterPlane = wpe->GetWaterPlane();

		for(auto it_WaterPlane = this->zWaterPlanes.begin(); it_WaterPlane != this->zWaterPlanes.end(); it_WaterPlane++)
		{
			WaterPlane* temp = (*it_WaterPlane);
			if (temp == waterPlane)
			{
				this->zWaterPlanes.erase(it_WaterPlane);
				delete waterPlane;
				waterPlane = NULL;
				return;
			}
		}
	}
}

void DxManager::HandleImageEvent( ImageEvent* iev )
{
	if(iev->IsAdding())
		this->zImages.push_back(iev->GetImage());
	else
	{
		Image* img = iev->GetImage();
		for(auto it_Images = this->zImages.begin(); it_Images != this->zImages.end(); it_Images++)
		{
			Image* temp = (*it_Images);
			if (img == temp)
			{
				this->zImages.erase(it_Images);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
}

void DxManager::HandlePointLightEvent( PointLightEvent* lev )
{
	if(lev->IsAdding())
		this->zPointLights.push_back(lev->GetLight());
	else
	{
		PointLight* img = lev->GetLight();
		for(auto it_Light = this->zPointLights.begin(); it_Light != this->zPointLights.end(); it_Light++)
		{
			PointLight* temp = (*it_Light);
			if (img == temp)
			{
				this->zPointLights.erase(it_Light);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
}

void DxManager::HandleSpotLightEvent( SpotLightEvent* lev )
{
	if(lev->IsAdding())
		this->zSpotLights.push_back(lev->GetLight());
	else
	{
		SpotLight* img = lev->GetLight();
		for(auto it_Light = this->zSpotLights.begin(); it_Light != this->zSpotLights.end(); it_Light++)
		{
			SpotLight* temp = (*it_Light);
			if (img == temp)
			{
				this->zSpotLights.erase(it_Light);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
}

void DxManager::HandleTextEvent( TextEvent* tev )
{
	if(tev->IsAdding())
		this->zTexts.push_back(tev->GetText());
	else
	{
		Text* txt = tev->GetText();
		for(auto it_Text = this->zTexts.begin(); it_Text != this->zTexts.end(); it_Text++)
		{
			Text* temp = (*it_Text);
			if (txt == temp)
			{
				this->zTexts.erase(it_Text);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
}