#pragma once

#include "ReferenceCounted.h"
#include "ObjLoader.h"
#include <string>
using namespace std;

class ObjectDataResource : public ReferenceCounted
{
private:
	string		zName; //Used as identification.
	ObjData*	zObjData;

private:
	virtual ~ObjectDataResource();

public:
	ObjectDataResource();
	ObjectDataResource(string ID, ObjData* objData);

	const string& GetName() const { return this->zName; }
	ObjData* GetObjectDataPointer() const { return this->zObjData; }
	void SetObjectDataPointer(ObjData* pointer) { this->zObjData = pointer; }

};