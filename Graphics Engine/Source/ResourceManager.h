#pragma once

#include <map>
#include <vector>
#include "TextureResource.h"
#include "BufferResource.h"
#include "MeshStripResource.h"
#include "ObjectDataResource.h"

#include <string>
#include "CjenFileDebug.h"
#include <mutex>

class ResourceManager
{
private:
	ID3D11Device* zDevice;
	ID3D11DeviceContext* zDeviceContext;

	HANDLE zMutex;

	std::map<std::string, TextureResource*>	zTextureResources;
	std::map<std::string, BufferResource*> zBufferResources;
	std::map<std::string, MeshStripResource*> zMeshStripResources;
	std::map<std::string, ObjectDataResource*>	zObjectDataResources; //Used by meshstripresources

private:
	std::vector<MeshStrip*> LoadMeshStrips(const char* filePath, ObjData* objData); //Helper function for loading the mesh strips from file.
	void CalculateModelVectors(std::vector<VertexTangent>& verts);

public:
	ResourceManager();
	virtual ~ResourceManager();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);


	//Object data
	ObjectDataResource* LoadObjectDataResourceFromFile(const char* filePath);
	void UnloadObjectDataResource(const char* filePath);

	//Textures
		/*	Creates a texture resource from file with:
			BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			and returns a pointer to it.
			Is the default format used.
		*/
	TextureResource* CreateTextureResourceFromFile(const char* filePath, bool generateMipMap = false);
	
	/*	Creates a cube texture resource from file with:
			BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			and returns a pointer to it.
		*/
	TextureResource* CreateCubeTextureResourceFromFile(const char* filePath);

	//Buffer
		/*	
			Creates a buffer resource and returns a pointer to it. 
			const char* fileName - used as resource identification. Using the name of the file containing the vertex data is recommended.
			BUFFER_INIT_DESC bufferInitDesc - buffer data (a description) of the buffer to create.
		*/
		BufferResource* CreateBufferResource(const char* fileName, BUFFER_INIT_DESC bufferInitDesc);

	/*	Returns a pointer to the found texture resource. Returns NULL if no texture resource was found.	*/
	const TextureResource* GetTextureResourcePointer(const char* id) const;

	/*	Creates and returns a MeshStripResource from an .obj-file. Height of the mesh is returned through the second parameter.	*/
	MeshStripResource* CreateMeshStripsResourceFromFile(const char* filePath);

	/*	Deletes the MeshStripsResource sent through the parameter. The pointer to MeshStripsResource is automatically set to NULL.	*/
	void DeleteMeshStripsResource(MeshStripResource* &meshStripsResource);

	/*	Deletes	the buffer resource sent through the parameter. The pointer to the buffer resource is automatically set to NULL.	*/
	void DeleteBufferResource(BufferResource* &bufferResource);

	/*	Deletes the texture resource sent through the parameter. The pointer to the texture sent is automatically set to NULL.	*/
	void DeleteTextureResource(TextureResource* &textureResource);

	void CalculateTangentBinormal( Vertex v1, Vertex v2, Vertex v3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal);
	void CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal);
};

void ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();