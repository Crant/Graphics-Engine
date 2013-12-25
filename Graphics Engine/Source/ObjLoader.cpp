#include "ObjLoader.h"
#include "Exceptions.h"

D3DXVECTOR3 ObjLoader::GetVertPosData( std::string line )
{
	float x;
	float y;
	float z;
	sscanf_s(line.c_str(), "v %f %f %f", &x, &y, &z);

	return D3DXVECTOR3(x, y, z);
}

D3DXVECTOR2 ObjLoader::GetTextCoordsData( std::string line )
{
	float u;
	float v;
	sscanf_s(line.c_str(), "vt %f %f", &u, &v);

	return D3DXVECTOR2(u, v);
}

D3DXVECTOR3 ObjLoader::GetVertsNormsData( std::string line )
{
	float x;
	float y;
	float z;

	sscanf_s(line.c_str(), "vn %f %f %f", &x, &y, &z);

	return D3DXVECTOR3(x, y, z);
}

FaceData ObjLoader::GetFaceData( std::string line )
{
	FaceData face;
	//f  v1/vt1/vn1   v2/vt2/vn2   v3/vt3/vn3
	sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
		&face.data[0][0], &face.data[0][1], &face.data[0][2], 
		&face.data[1][0], &face.data[1][1], &face.data[1][2], 
		&face.data[2][0], &face.data[2][1], &face.data[2][2]);
	
	return face;
}

ObjData* ObjLoader::LoadObjFile( const std::string& filepath )
{
	std::string mtlfile = "";
	ObjData* returndata = new ObjData();
	//this->trianglulate(filename);								////////// Not quite working

	std::string folders = "";
	//std::string filename = filepath;
	//std::size_t slashpos = filename.find("/");
	//while(slashpos != std::string::npos)
	//{
	//	slashpos = filename.find("/");
	//	folders += filename.substr(0, slashpos + 1);
	//	filename = filename.substr(slashpos + 1);
	//}

	std::string filename = filepath;
	int index = filepath.find_last_of("/", filepath.size());

	if (index != -1)
	{
		folders = filepath.substr(0, index + 1);
		filename = filepath.substr(index + 1, filename.size());
	}

	std::ifstream binfile;
	binfile.open(folders + "Cache/" + filename.substr(0, filename.size() - 4) + ".CEng", std::ios::binary);
	if (binfile)
	{
		this->ReadFromBinaryFile(returndata, binfile);
		binfile.close();
		return returndata;
	}
	std::ifstream file;
	file.open(filepath);
	if(!file)
	{
		throw(MeshException(__FILE__, __LINE__, __FUNCTION__, "Couldn't open file in ObjLoader: " + filepath));
	}
	else
	{
		std::string currentMaterial = "";

		while(!file.eof())
		{
			std::string line = "";
			getline(file, line);
			if(line.size() > 0 && line.substr(0, 2) == "v ")
			{
				D3DXVECTOR3 vertpos = this->GetVertPosData(line);
				vertpos.z *= -1;	// For D3DX
				returndata->vertspos->push_back(vertpos);
			}
			else if(line.size() > 0 && line.substr(0, 2) == "vt")
			{
				returndata->textcoords->push_back(this->GetTextCoordsData(line));
			}
			else if(line.size() > 0 && line.substr(0, 2) == "vn")
			{
				D3DXVECTOR3 norm = this->GetVertsNormsData(line);
				norm.z *= -1;	// For D3DX
				returndata->vertsnorms->push_back(norm);
			}
			else if(line.size() > 0 && line.at(0) == 'f')
			{
				FaceData data = this->GetFaceData(line);
				data.material = currentMaterial;
				returndata->faces->push_back(data);
			}
			else if(line.size() > 0 && line.substr(0, 6) == "mtllib")
				mtlfile = line.substr(7);

			else if(line.size() > 0 && line.substr(0, 6) == "usemtl")
				currentMaterial = line.substr(7);
		}
		file.close();
	}

	// mtl file
	if(mtlfile == "")
		return returndata;

	mtlfile = folders + mtlfile;

	file.open(mtlfile);
	if(!file)
	{
		throw(MeshException(__FILE__, __LINE__, __FUNCTION__, "Couldn't open file in ObjLoader: " + mtlfile));
	}

	bool firstMat = true;
	MaterialData md;

	while(!file.eof())
	{
		std::string line = "";
		std::getline(file, line);

		if(line.size() > 0 && line.substr(0, 6) == "newmtl")
		{
			if(!firstMat)
			{
				returndata->mats->push_back(md);
				md = MaterialData();
			}
			md.name = line.substr(7);
			firstMat = false;
		}
		else if(line.size() > 0 && line.substr(0, 5) == "illum")
		{
			sscanf_s(line.c_str(), "illum %d", &md.illum);
		}
		else if(line.size() > 0 && line.substr(0, 2) == "Kd")
		{
			D3DXVECTOR3 Kd = this->GetVertPosData(line);
			md.kd = Kd;
		}
		else if(line.size() > 0 && line.substr(0, 2) == "Ka")
		{
			D3DXVECTOR3 Ka = this->GetVertPosData(line);
			md.ka = Ka;
		}
		else if(line.size() > 0 && line.substr(0, 2) == "Tf")
		{
			D3DXVECTOR3 Tf = this->GetVertPosData(line);
			md.tf = Tf;
		}
		else if(line.size() > 0 && line.substr(0, 6) == "map_Kd")
		{
			md.texture = folders;
			md.texture += line.substr(7);
		}
		else if(line.size() > 0 && line.substr(0, 2) == "Ks")
		{
			D3DXVECTOR3 Ks = this->GetVertPosData(line);
			md.ks = Ks;
		}
		else if(line.size() > 0 && line.substr(0, 2) == "Ni")
		{
			sscanf_s(line.c_str(), "Ni %f", &md.ni);
		}
	}

	if(!firstMat)
		returndata->mats->push_back(md);

	file.close();

	this->CreateBinaryFile(folders + "Cache/" + filename, folders + "Cache/", returndata);

	return returndata;
}

void ObjLoader::CreateBinaryFile( const std::string& filename, std::string cacheFolder, ObjData* returndata )
{
	std::string cacheRearranged = "";
	size_t slashpos = cacheFolder.find("/");
	while(slashpos != std::string::npos)
	{
		cacheFolder.replace(slashpos, 1, "\\");
		cacheRearranged += cacheFolder.substr(0, slashpos + 1);
		cacheFolder = cacheFolder.substr(slashpos + 1);
		slashpos = cacheFolder.find("/");
	}
	CreateDirectory(cacheRearranged.c_str(), NULL);
	std::string binFilename = filename.substr(0, filename.length() - 4) + ".CEng";

	std::ofstream binFile;
	binFile.open(binFilename, std::ios::binary);
	if(!binFile)
	{
		throw(MeshException(__FILE__, __LINE__, __FUNCTION__, "Couldn't create " + binFilename + " when trying to create binary file from obj-file"));
		return;
	}

	int nrOfVerts = returndata->vertspos->size();
	binFile.write(reinterpret_cast<const char*>(&nrOfVerts), sizeof(int));

	for (int i = 0; i < nrOfVerts; i++)
	{
		D3DXVECTOR3 verts = returndata->vertspos->at(i);

		binFile.write(reinterpret_cast<const char*>(&verts), sizeof(D3DXVECTOR3));
	}


	int nrOfTextCoords = returndata->textcoords->size();
	binFile.write(reinterpret_cast<const char*>(&nrOfTextCoords), sizeof(int));

	for (int i = 0; i < nrOfTextCoords; i++)
	{
		D3DXVECTOR2 texCoords = returndata->textcoords->at(i);

		binFile.write(reinterpret_cast<const char*>(&texCoords), sizeof(D3DXVECTOR2));
	}

	int nrOfNorms = returndata->vertsnorms->size();
	binFile.write(reinterpret_cast<const char*>(&nrOfNorms), sizeof(int));

	for (int i = 0; i < nrOfNorms; i++)
	{
		D3DXVECTOR3 normals = returndata->vertsnorms->at(i);

		binFile.write(reinterpret_cast<const char*>(&normals), sizeof(D3DXVECTOR3));
	}

	int nrOfFaces = returndata->faces->size();
	binFile.write(reinterpret_cast<const char*>(&nrOfFaces), sizeof(int));

	for (int i = 0; i < nrOfFaces; i++)
	{
		FaceData fd = returndata->faces->at(i);

		unsigned int size = fd.material.length();
		binFile.write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
		binFile.write(&fd.material[0], fd.material.length());

		for (int u = 0; u < 3; u++)
		{
			for (int y = 0; y < 3; y++)
			{
				binFile.write(reinterpret_cast<const char*>(&fd.data[u][y]), sizeof(int));
			}
		}
	}

	int nrOfMats = returndata->mats->size();
	binFile.write(reinterpret_cast<const char*>(&nrOfMats), sizeof(int));

	for (int i = 0; i < nrOfMats; i++)
	{
		MaterialData md = returndata->mats->at(i);

		unsigned int size = md.name.length();
		binFile.write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
		binFile.write(&md.name[0], md.name.length());

		size = md.texture.length();
		binFile.write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
		binFile.write(&md.texture[0], md.texture.length());

		binFile.write(reinterpret_cast<const char*>(&md.illum), sizeof(int));

		float x = md.kd.x;
		float y = md.kd.y;
		float z = md.kd.z;

		binFile.write(reinterpret_cast<const char*>(&x), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&y), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&z), sizeof(float));

		x = md.ka.x; 
		y = md.ka.y; 
		z = md.ka.z;

		binFile.write(reinterpret_cast<const char*>(&x), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&y), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&z), sizeof(float));

		x = md.tf.x; 
		y = md.tf.y; 
		z = md.tf.z;

		binFile.write(reinterpret_cast<const char*>(&x), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&y), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&z), sizeof(float));

		x = md.ks.x; 
		y = md.ks.y; 
		z = md.ks.z;

		binFile.write(reinterpret_cast<const char*>(&x), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&y), sizeof(float));
		binFile.write(reinterpret_cast<const char*>(&z), sizeof(float));

		binFile.write(reinterpret_cast<const char*>(&md.ni), sizeof(float));
	}
}

void ObjLoader::ReadFromBinaryFile( ObjData* returndata, std::ifstream& binfile )
{
	int nrOfVerts = 0;
	binfile.read(reinterpret_cast<char*>(&nrOfVerts), sizeof(int));

	for(int i = 0; i < nrOfVerts; i++)
	{
		D3DXVECTOR3 verts;

		binfile.read(reinterpret_cast<char*>(&verts), sizeof(D3DXVECTOR3));

		returndata->vertspos->push_back(verts);
	}

	int nrOfTextCoords = 0;
	binfile.read(reinterpret_cast<char*>(&nrOfTextCoords), sizeof(int));

	for(int i = 0; i < nrOfTextCoords; i++)
	{
		D3DXVECTOR2 texCoords;

		binfile.read(reinterpret_cast<char*>(&texCoords), sizeof(D3DXVECTOR2));

		returndata->textcoords->push_back(texCoords);
	}

	int nrOfNorms = 0;
	binfile.read(reinterpret_cast<char*>(&nrOfNorms), sizeof(int));

	for(int i = 0; i < nrOfNorms; i++)
	{
		D3DXVECTOR3 normals;

		binfile.read(reinterpret_cast<char*>(&normals), sizeof(D3DXVECTOR3));

		returndata->vertsnorms->push_back(normals);
	}

	int nrOfFaces = 0;
	binfile.read(reinterpret_cast<char*>(&nrOfFaces), sizeof(int));

	for(int i = 0; i < nrOfFaces; i++)
	{
		std::string materialName;
		unsigned int size;
		binfile.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
		materialName.resize(size);
		binfile.read(&materialName[0], size);

		FaceData fd;

		for(int u = 0; u < 3; u++)
		{
			for(int y = 0; y < 3; y++)
			{
				binfile.read(reinterpret_cast<char*>(&fd.data[u][y]), sizeof(int));
			}
		}

		fd.material = materialName;
		returndata->faces->push_back(fd);
	}

	int nrOfMats = 0;
	binfile.read(reinterpret_cast<char*>(&nrOfMats), sizeof(int));

	for(int i = 0; i < nrOfMats; i++)
	{

		std::string materialName;
		unsigned int size;
		binfile.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
		materialName.resize(size);
		binfile.read(&materialName[0], size);

		std::string texture;
		binfile.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
		texture.resize(size);
		binfile.read(&texture[0], size);

		MaterialData md;
		md.name = materialName;
		md.texture = texture;

		binfile.read(reinterpret_cast<char*>(&md.illum), sizeof(int));

		float x = 0; 
		float y = 0; 
		float z = 0;

		binfile.read(reinterpret_cast<char*>(&x), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&y), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&z), sizeof(float));
		md.kd = D3DXVECTOR3(x, y, z);

		binfile.read(reinterpret_cast<char*>(&x), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&y), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&z), sizeof(float));
		md.ka = D3DXVECTOR3(x, y, z);

		binfile.read(reinterpret_cast<char*>(&x), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&y), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&z), sizeof(float));
		md.tf = D3DXVECTOR3(x, y, z);

		binfile.read(reinterpret_cast<char*>(&x), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&y), sizeof(float));
		binfile.read(reinterpret_cast<char*>(&z), sizeof(float));
		md.ks = D3DXVECTOR3(x, y, z);

		binfile.read(reinterpret_cast<char*>(&md.ni), sizeof(float));

		returndata->mats->push_back(md);
	}
}