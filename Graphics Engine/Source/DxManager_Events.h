#pragma once

#include "Process.h"

/* Process events for adding to rendering */

class RendererEvent : public CJen::ProcessEvent
{
protected:
	bool Adding;
	bool deleteSelf;

public:
	RendererEvent(bool adding = true) 
	{ 
		this->Adding = adding; 
		this->deleteSelf = true;
	}
	virtual ~RendererEvent() 
	{ 
	}
	bool IsAdding() { return this->Adding; }
};

class StaticMeshEvent : public RendererEvent
{
private:
	StaticMesh* mesh;

public:
	StaticMeshEvent(bool adding, StaticMesh* mesh) : RendererEvent(adding)
	{
		this->mesh = mesh; 
	}
	virtual ~StaticMeshEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->mesh)
				delete this->mesh;
		}
	}

	StaticMesh* GetStaticMesh() { this->deleteSelf = false; return this->mesh; }
};

class TerrainEvent : public RendererEvent
{
private:
	Terrain* terrain;

public:
	TerrainEvent(bool adding, Terrain* terrain) : RendererEvent(adding)
	{
		this->terrain = terrain;
	}
	virtual ~TerrainEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->terrain) delete this->terrain; this->terrain = NULL;
		}
	}

	Terrain* GetTerrain() { this->deleteSelf = false; return this->terrain; }
};

class SkyBoxEvent : public RendererEvent
{
private:
	SkyBox* sb;

public:
	SkyBoxEvent(bool adding, SkyBox* sb) : RendererEvent(adding)
	{
		this->sb = sb; 
	}
	virtual ~SkyBoxEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->sb)
				delete this->sb;
		}
	}

	SkyBox* GetSkyBox() { this->deleteSelf = false; return this->sb; }
};

class ImageEvent : public RendererEvent
{
private:
	Image* img;

public:
	ImageEvent(bool adding, Image* img) : RendererEvent(adding)
	{
		this->img = img;
	}
	virtual ~ImageEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->img)
				delete this->img;
		}
	}
	Image* GetImage() { this->deleteSelf = false; return this->img; }
};

class TextEvent : public RendererEvent
{
private:
	Text* txt;

public:
	TextEvent(bool adding, Text* txt) : RendererEvent(adding)
	{
		this->txt = txt;
	}
	virtual ~TextEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->txt)
				delete this->txt;
		}
	}
	Text* GetText() { this->deleteSelf = false; return this->txt; }
};

class LightEvent : public RendererEvent
{
private:
	PointLight* pLight;

public:
	LightEvent(bool adding, PointLight* light) : RendererEvent(adding)
	{
		this->pLight = light;
	}
	virtual ~LightEvent()
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->pLight)
				delete this->pLight;
		}
	}
	PointLight* GetLight() { this->deleteSelf = false; return this->pLight; }
};