#pragma once

#include "DirectX.h"
struct VertexTex
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texCoord;

	VertexTex()
	{
		position = D3DXVECTOR3(0, 0, 0);
		texCoord = D3DXVECTOR2(0, 0);
	}
	VertexTex(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord)
	{
		position = _pos;
		texCoord = _texCoord;
	}
};

struct VertexTex3
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 texCoord;

	VertexTex3()
	{
		position = D3DXVECTOR3(0, 0, 0);
		texCoord = D3DXVECTOR3(0, 0, 0);
	}
	VertexTex3(D3DXVECTOR3 _pos, D3DXVECTOR3 _texCoord)
	{
		position = _pos;
		texCoord = _texCoord;
	}
};

struct Vertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 color;

	//Pos, TexCoord, Norm, Color
	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm, D3DXVECTOR3 _col)
	{
		position = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = _col;
	}
	//Pos, TexCoord, Norm
	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm)
	{
		position = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = D3DXVECTOR3(0, 0, 0);
	}
	//Pos, Norm, Color
	Vertex(D3DXVECTOR3 _pos, D3DXVECTOR3 _norm, D3DXVECTOR3 _color)
	{
		position = _pos;
		texCoord = D3DXVECTOR2(0, 0);
		normal = _norm;
		color = _color;
	}
	Vertex()
	{
		position = D3DXVECTOR3(0, 0, 0);
		texCoord = D3DXVECTOR2(0, 0);
		normal = D3DXVECTOR3(0, 0, 0);
		color = D3DXVECTOR3(0, 0, 0);
	}
	Vertex(const Vertex* origObj)
	{
		position = origObj->position;
		texCoord = origObj->texCoord;
		normal = origObj->normal;
		color = origObj->color;
	}
};

struct VertexNormalMap : public Vertex
{
	D3DXVECTOR3 tangent;

	VertexNormalMap(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm, D3DXVECTOR3 _tang) : Vertex(_pos, _texCoord, _norm)
	{
		tangent = _tang;
	}
	VertexNormalMap(D3DXVECTOR3 _pos, D3DXVECTOR2 _texCoord, D3DXVECTOR3 _norm) : Vertex(_pos, _texCoord, _norm)
	{
		tangent = D3DXVECTOR3(0, 0, 0);
	}
	VertexNormalMap() : Vertex()
	{
		tangent = D3DXVECTOR3(0, 0, 0);
	}
	VertexNormalMap(const VertexNormalMap* origObj)
	{
		position = origObj->position;
		texCoord = origObj->texCoord;
		normal = origObj->normal;
		tangent = origObj->tangent;
	}
};