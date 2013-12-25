#include "Text.h"
#include <algorithm>

Text::Text(string text, D3DXVECTOR2 position, float size)
{
	this->zText = text;
	//this->ToUpper();

	this->zPosition = position;
	this->zSize = size;
	this->zStrata = 0.5f;
	this->zColor = D3DXVECTOR3(0, 0, 0);
	this->zFont = new Font();
	this->zFont->textureResource = NULL;
	for(int i = 0; i < 256; i++)
		this->zFont->charTexCoords[i] = 0;
}

void Text::ToUpper()
{
	std::transform(this->zText.begin(), this->zText.end(), this->zText.begin(), ::toupper);
}

void Text::DeleteFromEnd(unsigned int CharsToDel) 
{
	if(CharsToDel <= this->zText.size())
		this->zText = this->zText.substr(0, this->zText.size() - CharsToDel);
}

Text::~Text()
{
	if(this->zFont->textureResource) 
		GetResourceManager()->DeleteTextureResource(this->zFont->textureResource);

	if(this->zFont)
		delete this->zFont;
}

void Text::SetText( const char* text )
{
	this->zText = text;
}

const char* Text::GetText() const
{
	return this->zText.c_str();
}

void Text::SetPosition( Vector2 pos )
{
	this->zPosition = D3DXVECTOR2(pos.x, pos.y);
}

Vector2 Text::GetPosition() const
{
	return Vector2(this->zPosition.x, this->zPosition.y);
}

void Text::SetSize( float size )
{
	this->zSize = size;
}

float Text::GetSize() const
{
	return this->zSize;
}

void Text::AppendText( const char* app )
{
	this->zText += app;
}

void Text::SetColor( Vector3 color )
{
	this->zColor = D3DXVECTOR3(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f);
}

void Text::SetStrata( float strata )
{
	this->zStrata = strata / 1000.0f;
}

float Text::GetStrata() const
{
	return this->zStrata * 1000.0f;
}