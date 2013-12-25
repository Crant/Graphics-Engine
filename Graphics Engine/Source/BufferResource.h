#pragma once

#include "Buffer.h"
#include "ReferenceCounted.h"
#include <string>

class BufferResource : public ReferenceCounted
{
private:
	std::string	zName; //Used as identification.
	Buffer*	zBuffer;

public:
	BufferResource();
	BufferResource(std::string ID, Buffer* buffer);
	virtual ~BufferResource();

	const std::string& GetName() const { return this->zName; }
	Buffer* GetBufferPointer() const { return this->zBuffer; }
	void SetBufferPointer(Buffer* pointer) { this->zBuffer = pointer; }

};