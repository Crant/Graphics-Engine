#pragma once

#include <string>

class GraphicalException
{
public:
	GraphicalException(const std::string& filename, const int& lineNumber, 
		const std::string& function);
};

class MeshException
{
public:
	MeshException(const std::string& filename, const int& lineNumber, 
		const std::string& function, const std::string& extraInfo);
};

class ShaderException
{
public:
	ShaderException(const std::string& filename, const int& lineNumber, 
		const std::string& function, const std::string& extraInfo);
};

class VertexBufferException
{
public:
	VertexBufferException(const std::string& filename, const int& lineNumber, 
		const std::string& function, const std::string& extraInfo);
};