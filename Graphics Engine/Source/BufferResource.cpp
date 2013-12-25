#include "BufferResource.h"

BufferResource::~BufferResource()
{
	SAFE_DELETE(this->zBuffer);
}

BufferResource::BufferResource()
{
	this->zBuffer = NULL;
	this->zName = "";
}

BufferResource::BufferResource( std::string ID, Buffer* buffer )
{
	this->zBuffer = buffer;
	this->zName = ID;
}