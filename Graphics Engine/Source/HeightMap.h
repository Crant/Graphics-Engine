#pragma once

#include <vector>
#include <string>

class HeightMap
{
private:
	std::string zHeightMapName;
	float zHeightScale;
	float zHeightOffset;
	std::vector<float> zHeightMap;

	float zRows;
	float zCols;
private:
	void Filter3x3();
	bool InBoundsOfHeightMap(int i, int j);
	float SampleHeight3x3(int i, int j);

public:
	HeightMap();
	HeightMap(int width, int depth, const std::string& filename, const float& heightScale = 1.0f, const float& heightOffset = 0.0f);

	void LoadRaw(int width, int depth, const std::string& filename, const float& heightScale, const float& heightOffset);

	float& GetHeightAt(int i, int j);
	const float& operator()(int i, int j) const;
	const std::string& GetFileName() {return this->zHeightMapName;}
};