#pragma once

#include <string>
#include "DirectX.h"
#include "iText.h"
#include "ResourceManager.h"

struct Font
{
	TextureResource* textureResource;
	int charTexCoords[256];
	int charWidth[256];

	Font()
	{
		for(int i = 0; i < 256; i++)
		{
			this->charTexCoords[i] = 0;
			this->charWidth[i] = 0;
		}
	}
};

class Text : public iText
{
private:
	std::string zText;
	D3DXVECTOR2 zPosition;
	float zSize;
	float zStrata;
	Font* zFont;
	D3DXVECTOR3 zColor;

	void ToUpper();

public:
	Text(std::string text, D3DXVECTOR2 position, float size);
	virtual ~Text();

	virtual void SetColor(Vector3 color);
	virtual D3DXVECTOR3 GetColor() const { return this->zColor; }
	virtual void SetText(const char* text);
	virtual const char* GetText() const;
	virtual void AppendText(const char* app);
	virtual void DeleteFromEnd(unsigned int CharsToDel);
	virtual void SetPosition(Vector2 pos);
	virtual Vector2 GetPosition() const;
	virtual void SetSize(float size);
	virtual float GetSize() const;
	void SetFont(Font* newFont) { this->zFont = newFont; }
	Font* GetFont() const { return this->zFont; }

	virtual void SetStrata(float strata);
	virtual float GetStrata() const;
	float GetStrata01() { return this->zStrata; }

};