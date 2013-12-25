#include "HeightMap.h"
#include <fstream>

HeightMap::HeightMap()
{

}

HeightMap::HeightMap( int width, int depth, const std::string& filename, const float& heightScale, const float& heightOffset)
{
	LoadRaw(width, depth, filename, heightScale, heightOffset);
}

float& HeightMap::GetHeightAt( int i, int j )
{
	if(!(i >= 0 && i < this->zRows && j >= 0 && j < this->zCols) )
		throw("out of bounds");
	
	return this->zHeightMap[i * this->zRows + j];
}

void HeightMap::LoadRaw(int rows, int cols, const std::string& filename, const float& heightScale, const float& heightOffset)
{
	this->zHeightMapName = filename;
	this->zHeightOffset = heightOffset;
	this->zHeightScale = heightScale;

	this->zCols = cols;
	this->zRows = rows;

	std::vector<unsigned char> vertexHeights(rows * cols);
	
	std::ifstream fin;
	fin.open(filename.c_str(), std::ios::binary);

	if (!fin.is_open())
		throw("");

	fin.read((char*)&vertexHeights[0], 
		(std::streamsize)vertexHeights.size());

	fin.close();

	this->zHeightMap.resize(rows * cols);

	for (int i = 0; i < cols; i++)
	{
		for(int j = 0; j < rows; j++)
		{
			int index = i * rows + j;

			this->zHeightMap[index] = (float)vertexHeights[index] 
				* this->zHeightScale 
					+ this->zHeightOffset;
		}
	}

	//Filter the table to smooth it out. We do this because x height
	//steps may be rough. And now that we copied the data into a
	//float-table, we have more precision. (Boxfilter)
	this->Filter3x3();
}

void HeightMap::Filter3x3()
{
	std::vector<float> filteredHeights(this->zRows * this->zCols);

	for(int i = 0; i < this->zRows; ++i)
	{
		for(int j = 0; j < this->zCols; ++j)
		{
			int index = i * this->zRows + j;
			filteredHeights[index] = SampleHeight3x3(i, j);
		}
	}

	this->zHeightMap = filteredHeights;
}

bool HeightMap::InBoundsOfHeightMap( int i, int j )
{
	return (i >= 0 && i < (int)this->zRows &&    
		j >= 0 && j < (int)this->zCols);
}

float HeightMap::SampleHeight3x3( int i, int j )
{
	float average = 0.0f;
	float sample = 0.0f;

	for(int m = i - 1; m <= i + 1; m++)
	{
		for(int n = j - 1; n <= j + 1; n++)
		{
			if(InBoundsOfHeightMap(m, n))
			{
				average += this->zHeightMap[m * this->zRows + n];
				sample += 1.0f;
			}
		}
	}
	return average / sample;
}