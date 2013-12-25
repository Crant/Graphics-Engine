#include "ResourceManager.h"
#include <algorithm>
#include "Exceptions.h"
#include "Material.h"

namespace
{
	static ResourceManager* resourceManager = NULL;
}

void ResourceManagerInit( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	resourceManager = new ResourceManager();

	resourceManager->Init(device, deviceContext);
}

ResourceManager* GetResourceManager() throw(...)
{
	if(!resourceManager)
	{
		throw("ResourceManager Not Initialized!");
	}

	return resourceManager;
}

bool FreeResourceManager()
{
	if(resourceManager)
	{
		delete resourceManager;
		resourceManager = NULL;
	}
	else
	{
		return false;
	}	

	return true;
}

void ResourceManager::CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal)
{
	float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrtf((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}

void ResourceManager::CalculateModelVectors(std::vector<VertexNormalMap>& verts)
{
	int faceCount, index;
	VertexNormalMap vert[3];
	D3DXVECTOR3 tangent, binormal, normal;

	std::vector<VertexNormalMap> vertices;

	// Calculate the number of faces in the model.
	faceCount = verts.size() / 3;

	// Initialize the index to the model data.
	index = 0;
	
	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (int i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		for (int j = 0; j < 3; j++)
		{
			vert[j].position = verts[index].position;
			vert[j].texCoord =  verts[index].texCoord;
			vert[j].normal =  verts[index].normal;

			index++;
		}

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vert[0], vert[1], vert[2], tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		verts[index - 1].tangent = tangent;
		verts[index - 2].tangent = tangent;
		verts[index - 3].tangent = tangent;
	}
}

void ResourceManager::CalculateTangentBinormal( Vertex v1, Vertex v2, Vertex v3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den, length;

	// Calculate the two vectors for this face.
	vector1[0] = v2.position.x - v1.position.x;
	vector1[1] = v2.position.y - v1.position.y;
	vector1[2] = v2.position.z - v1.position.z;

	vector2[0] = v3.position.x - v1.position.x;
	vector2[1] = v3.position.y - v1.position.y;
	vector2[2] = v3.position.z - v1.position.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = v2.texCoord.x - v1.texCoord.x;
	tvVector[0] = v2.texCoord.y - v1.texCoord.y;

	tuVector[1] = v3.texCoord.x - v1.texCoord.x;
	tvVector[1] = v3.texCoord.y - v1.texCoord.y;

	// Calculate the denominator of the tangent equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this tangent.
	length = sqrtf((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrtf((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

ResourceManager::ResourceManager()
{
	this->zMutex = CreateMutex(NULL, false, NULL);
}

ResourceManager::~ResourceManager()
{
	for (auto it_objData = this->zObjectDataResources.begin(); it_objData != this->zObjectDataResources.end(); it_objData++)
	{
		if (it_objData->second)
		{
			int refCount = it_objData->second->GetReferenceCount();
			for (int i = 0; i < refCount; i++)
			{
				it_objData->second->DecreaseReferenceCount();
			}
			it_objData->second = NULL;
		}
	}
	this->zObjectDataResources.clear();

	for (auto it_TexRes = this->zTextureResources.begin(); it_TexRes != this->zTextureResources.end(); it_TexRes++)
	{
		if (it_TexRes->second)
		{
			int refCount = it_TexRes->second->GetReferenceCount();
			for (int i = 0; i < refCount; i++)
			{
				it_TexRes->second->DecreaseReferenceCount();
			}
			it_TexRes->second = NULL;
		}
	}
	this->zTextureResources.clear();

	for (auto it_buffRes = this->zBufferResources.begin(); it_buffRes != this->zBufferResources.end(); it_buffRes++)
	{
		if (it_buffRes->second)
		{
			int refCount = it_buffRes->second->GetReferenceCount();
			for (int i = 0; i < refCount; i++)
			{
				it_buffRes->second->DecreaseReferenceCount();
			}
			it_buffRes->second = NULL;
		}
	}
	this->zBufferResources.clear();

	for (auto it_meshStrip = this->zMeshStripResources.begin(); it_meshStrip != this->zMeshStripResources.end(); it_meshStrip++)
	{
		if (it_meshStrip->second)
		{
			int refCount = it_meshStrip->second->GetReferenceCount();
			for (int i = 0; i < refCount; i++)
			{
				it_meshStrip->second->DecreaseReferenceCount();
			}
			it_meshStrip->second = NULL;
		}
	}
	this->zMeshStripResources.clear();

	CloseHandle(this->zMutex);
}

void ResourceManager::Init( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	this->zDevice = device;
	this->zDeviceContext = deviceContext;
}

TextureResource* ResourceManager::CreateTextureResourceFromFile( const char* filePath, bool generateMipMap /*= false*/ )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateTextureResourceFromFile.");


	std::string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto text = this->zTextureResources.find(file);

	if (text == this->zTextureResources.end())
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));

		loadInfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		ID3D11ShaderResourceView* srv = NULL;

		if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			this->zDevice, 
			file.c_str(), 
			&loadInfo, 
			NULL, &srv, NULL)))
		{
			throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
			ReleaseMutex(this->zMutex);
			return NULL;
		}
		else
		{
			this->zTextureResources[file] = new TextureResource(file, srv);

			this->zTextureResources[file]->IncreaseReferenceCount();
			
			ReleaseMutex(this->zMutex);
			return this->zTextureResources[file];
		}
	}
	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[file]->IncreaseReferenceCount();

	ReleaseMutex(this->zMutex);
	return text->second;
}

TextureResource* ResourceManager::CreateCubeTextureResourceFromFile( const char* filePath )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateCubeTextureReosurceFromFile.");


	std::string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto tex = this->zTextureResources.find(file);
	//If the texture wasn't found in the array create it
	if (tex == this->zTextureResources.end())
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		//Create
		ID3D11ShaderResourceView* SRV = NULL;
		if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			zDevice, 
			file.c_str(), 
			&loadInfo, 
			NULL, &SRV, NULL)))
		{
			throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
			ReleaseMutex(this->zMutex);
			return NULL;
		}
		else
		{
			this->zTextureResources[file] = new TextureResource(file, SRV);

			this->zTextureResources[file]->IncreaseReferenceCount();

			ReleaseMutex(this->zMutex);
			//Return newly created texture.
			return this->zTextureResources[file];
		}
	}

	//If the texture already exists, increase reference counter & return texture.
	this->zTextureResources[file]->IncreaseReferenceCount();

	ReleaseMutex(this->zMutex);
	return tex->second;
}

const TextureResource* ResourceManager::GetTextureResourcePointer( const char* id ) const
{
	string file = id;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	auto tex = this->zTextureResources.find(file);

	if(tex != this->zTextureResources.cend())
	{
		return tex->second;
	}

	return NULL;
}

BufferResource* ResourceManager::CreateBufferResource( const char* fileName, BUFFER_INIT_DESC bufferInitDesc )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateBufferResource.");

	std::string file = fileName;

	auto buff = this->zBufferResources.find(fileName);

	//If the buffer resource was not found in the array, create it.
	if (buff == this->zBufferResources.end())
	{
		Buffer* buffer = new Buffer();

		//Initialize buffer with device & deviceContext
		HRESULT hr = buffer->Init(this->zDevice, this->zDeviceContext, bufferInitDesc);
	
		if (FAILED(hr))
		{
			ReleaseMutex(this->zMutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zBufferResources[file] = new BufferResource(file, buffer);
			//Increase reference count.
			this->zBufferResources[file]->IncreaseReferenceCount();

			ReleaseMutex(this->zMutex);
			return this->zBufferResources[file];
		}
		
	}

	//If the buffer already exists, increase reference counter & return it.
	this->zBufferResources[file]->IncreaseReferenceCount();

	ReleaseMutex(this->zMutex);
	return buff->second;
}

MeshStripResource* ResourceManager::CreateMeshStripsResourceFromFile( const char* filePath)
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager CreateMeshStripsResourceFromFile.");

	string file = filePath;
	std::transform(file.begin(), file.end(), file.begin(), tolower); //Force lower case

	ObjectDataResource* tmp = this->LoadObjectDataResourceFromFile(file.c_str());

	if (tmp != NULL)
	{
		ObjData* objData = tmp->GetObjectDataPointer();
		if (objData != NULL)
		{
			file = file.substr(0, file.length() - 4);
			file += "_mesh";

			auto resource = this->zMeshStripResources.find(file);
			//Check if the meshStripResource already exists
			if(resource == this->zMeshStripResources.end())
			{
				std::vector<MeshStrip*> meshStrip;
				meshStrip = this->LoadMeshStrips(file.c_str(), objData);

				this->zMeshStripResources[file] = new MeshStripResource(file, meshStrip);

				//Increase reference counter and return.
				this->zMeshStripResources[file]->IncreaseReferenceCount();

				ReleaseMutex(this->zMutex);
				return this->zMeshStripResources[file];
			}
			else
			{
				resource->second->IncreaseReferenceCount();

				ReleaseMutex(this->zMutex);
				return resource->second;
			}
		}
	}
	ReleaseMutex(this->zMutex);
	return NULL;
}

std::vector<MeshStrip*> ResourceManager::LoadMeshStrips(const char* filePath, ObjData* objData)
{
	std::vector<MeshStrip*> meshStrips;

	std::vector<MaterialData>* mats = objData->mats;

	for (auto it_mats = mats->begin(); it_mats != mats->end(); it_mats++)
	{
		MaterialData md = (*it_mats);

		bool hasFace = false;
		MeshStrip* strip = new MeshStrip();

		/////// For hit/bounding boxes
		D3DXVECTOR3 min = D3DXVECTOR3(99999.9f, 99999.9f, 99999.9f);
		D3DXVECTOR3 max = -min;

		std::vector<VertexNormalMap> tempVerts;
		//tempVerts.resize(objData->faces->size() * 3);
		
		bool faceMaterialMisMatch = false;
		int faceMaterialMisMatchIndex = -1;

		for (auto it_faces = objData->faces->begin(); it_faces != objData->faces->end(); it_faces++)
		{
			FaceData fd = (*it_faces);

			if (fd.material == md.name)
			{
				int vertPos = fd.data[0][0] - 1;
				int texCoord = fd.data[0][1] - 1;
				int norm = fd.data[0][2] - 1;

				D3DXVECTOR3 vertex = objData->vertspos->at(vertPos);
				D3DXVECTOR3 normal = objData->vertsnorms->at(norm);
				D3DXVECTOR2 texCoords = objData->textcoords->at(texCoord);

				tempVerts.push_back(VertexNormalMap(vertex, texCoords, normal));

				vertPos = fd.data[2][0] - 1;
				texCoord = fd.data[2][1] - 1;
				norm = fd.data[2][2] - 1;

				vertex = objData->vertspos->at(vertPos);
				normal = objData->vertsnorms->at(norm);
				texCoords = objData->textcoords->at(texCoord);

				tempVerts.push_back(VertexNormalMap(vertex, texCoords, normal));

				vertPos = fd.data[1][0] - 1;
				texCoord = fd.data[1][1] - 1;
				norm = fd.data[1][2] - 1;

				vertex = objData->vertspos->at(vertPos);
				normal = objData->vertsnorms->at(norm);
				texCoords = objData->textcoords->at(texCoord);

				tempVerts.push_back(VertexNormalMap(vertex, texCoords, normal));

				hasFace = true;
			}
			else
			{
				faceMaterialMisMatch = true;
				faceMaterialMisMatchIndex = std::distance(objData->faces->begin(), it_faces);
			}
		}

		if(faceMaterialMisMatch)
		{
			std::string faceMaterial = objData->faces->at(faceMaterialMisMatchIndex).material;
			std::string material = md.name;
			std::stringstream ss;

			ss << "WARNING: ResourceManager: LoadMeshStrips(): Material mismatch for object: '" << string(filePath) 
				<< ", Material # " << std::distance(objData->mats->begin(), it_mats)
				<< "', Material: '" + material
				<< ", Face material: '" << faceMaterial + "'.";

			CJen::Debug(ss.str());
		}

		if (!hasFace)
		{
			delete strip;
		}
		else
		{
			std::vector<VertexNormalMap> verts;
			verts.resize(tempVerts.size());

			for (unsigned int i = 0; i < tempVerts.size(); i++)
			{
				verts[i] = tempVerts[i];
			}
			tempVerts.clear();

			this->CalculateModelVectors(verts);
			strip->SetVerts(verts);

			strip->SetTexturePath(md.texture);

			Material* mat = new Material();
			mat->AmbientColor = md.ka;
			if(mat->AmbientColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// Fix, otherwise completely black with most objs
				mat->AmbientColor += D3DXVECTOR3(0.2f, 0.2f, 0.2f);			//////////// Fix, otherwise completely black with most objs

			mat->DiffuseColor = md.kd;
			if(mat->DiffuseColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// Fix, otherwise completely black with most objs
				mat->DiffuseColor += D3DXVECTOR3(0.6f, 0.6f, 0.6f);			//////////// Fix, otherwise completely black with most objs

			mat->SpecularColor = md.ks;
			strip->SetMaterial(mat);

			//Create Boundingsphere

			meshStrips.push_back(strip);
		}
	}
	return meshStrips;
}

void ResourceManager::DeleteMeshStripsResource( MeshStripResource* &meshStripsResource )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager DeleteMeshStripsResource.");

	if (meshStripsResource)
	{
		std::string file = meshStripsResource->GetName();

		meshStripsResource->DecreaseReferenceCount();

		if (meshStripsResource->GetReferenceCount() == 1)
		{
			//Find Resource
			auto resource = this->zMeshStripResources.find(meshStripsResource->GetName());
			if (resource != this->zMeshStripResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				resource->second->DecreaseReferenceCount();
				//Remove buffer resource from table.
				this->zMeshStripResources.erase(resource);
			}
			meshStripsResource = NULL;
		}
	}
	ReleaseMutex(this->zMutex);
}

void ResourceManager::DeleteBufferResource( BufferResource* &bufferResource )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager DeleteBufferResource.");

	if (bufferResource)
	{
		bufferResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(bufferResource->GetReferenceCount() == 1)
		{
			//Find buffer resource.
			auto buff = this->zBufferResources.find(bufferResource->GetName());
			//If found..
			if(buff != this->zBufferResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				buff->second->DecreaseReferenceCount();
				//Remove buffer resource from table.
				this->zBufferResources.erase(buff);
			}
			bufferResource = NULL;
		}
	}
	ReleaseMutex(this->zMutex);
}

ObjectDataResource* ResourceManager::LoadObjectDataResourceFromFile( const char* filePath )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager LoadObjectDataResourceFromFile.");

	std::string file = filePath;
	//Force lower case
	std::transform(file.begin(), file.end(), file.begin(), tolower); 

	auto objData = this->zObjectDataResources.find(file);
	if (objData == this->zObjectDataResources.end())
	{
		ObjData* objectData = NULL;
		ObjLoader oj;

		objectData = oj.LoadObjFile(file);

		if (objectData == NULL)
		{
			throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
			ReleaseMutex(this->zMutex);
			return NULL;
		}
		else
		{
			//Create if loading was successful.
			this->zObjectDataResources[file] = new ObjectDataResource(file, objectData);
			//Increase reference count.
			this->zObjectDataResources[file]->IncreaseReferenceCount();

			ReleaseMutex(this->zMutex);
			//Return newly created object data resource.
			return this->zObjectDataResources[file];
		}
	}

	//If the object data resource already exists, increase reference counter & return it.
	this->zObjectDataResources[file]->IncreaseReferenceCount();

	ReleaseMutex(this->zMutex);
	return objData->second;
}

void ResourceManager::UnloadObjectDataResource( const char* filePath )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager UnloadObjectDataResource.");

	std::string file = filePath;
	//Force lower case
	std::transform(file.begin(), file.end(), file.begin(), tolower); 

	auto objData = this->zObjectDataResources.find(file);
	//If the object data resource was found in the array, decrease its reference counter.
	if(objData != this->zObjectDataResources.end())
	{
		ObjectDataResource* objectData = objData->second;
		objectData->DecreaseReferenceCount();

		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if(objectData->GetReferenceCount() == 1)
		{
			//therefore delete it by decreasing its reference count one more time.
			objectData->DecreaseReferenceCount();
			//Remove object data resource from table.
			this->zObjectDataResources.erase(objData);
		}
	}
	else
	{
		throw(GraphicalException(__FILE__, __LINE__, __FUNCTION__));
	}
	ReleaseMutex(this->zMutex);
}

void ResourceManager::DeleteTextureResource( TextureResource* &textureResource )
{
	if(this->zMutex)
	{
		WaitForSingleObject(this->zMutex, INFINITE);
	}
	else 
		CJen::Debug("Mutex is broken / hasn't been created / has been closed for Resourcemanager DeleteTextureResource.");

	if (textureResource)
	{
		textureResource->DecreaseReferenceCount();
		//If reference count is 1, no objects other than the resource manager itself has a reference to it.
		if (textureResource->GetReferenceCount() == 1)
		{
			//Find texture resource.
			auto tex = this->zTextureResources.find(textureResource->GetName());
			//If found..
			if(tex != this->zTextureResources.end())
			{
				//Decrease reference counter once more so that the texture will delete itself.
				tex->second->DecreaseReferenceCount();
				//Remove texture from table.
				this->zTextureResources.erase(tex);
			}
			else
			{
				std::stringstream ss;
				int refCount = textureResource->GetReferenceCount();

				ss << "WARNING: DETECTED MEMORY LEAK!:" << std::endl;
				ss << "ResourceManager::DeleteTextureResource: Could not find the specified texture: '" 
				   << textureResource->GetName() 
				   << "', references: " 
				   << refCount << std::endl;

				CJen::Debug(ss.str());
			}

			textureResource = NULL;
		}
	}
	ReleaseMutex(this->zMutex);
}