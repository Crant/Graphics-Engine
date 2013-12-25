#include "Exceptions.h"
#include "Cjen.h"
#include <fstream>

GraphicalException::GraphicalException(const std::string& filename, const int& lineNumber, const std::string& function)
{
	std::fstream write;

	write.open("Errors.txt", fstream::in | fstream::out | fstream::app);

	std::string newStr = filename;
	int index = filename.find_last_of("\\", filename.size());

	if (index != -1)
		newStr = filename.substr(index + 1, filename.size());
	
	write << __DATE__ << " " << __TIME__ << " GraphicalException" << endl;
	write << newStr <<endl;
	write << "Line: " << lineNumber <<endl;
	write << "Function: " << function << endl;
	write << endl;

	write.close();
}

void ClearFile()
{
	ofstream writeFile;
	writeFile.open ("Errors.txt", ios::out | ios::trunc);

	writeFile.close();
}

MeshException::MeshException( const std::string& filename, const int& lineNumber, const std::string& function, const std::string& extraInfo )
{
	std::fstream write;

	write.open("Errors.txt", fstream::in | fstream::out | fstream::app);

	std::string newStr = filename;
	int index = filename.find_last_of("\\", filename.size());

	if (index != -1)
		newStr = filename.substr(index + 1, filename.size());

	write << __DATE__ << " " << __TIME__ << " MeshException" << endl;
	write << newStr <<endl;
	write << "Line: " << lineNumber <<endl;
	write << "Function: " << function << endl;
	write << extraInfo << endl;
	write << endl;

	write.close();
}

ShaderException::ShaderException( const std::string& filename, const int& lineNumber, const std::string& function, const std::string& extraInfo )
{
	std::fstream write;

	write.open("Errors.txt", fstream::in | fstream::out | fstream::app);

	std::string newStr = filename;
	int index = filename.find_last_of("\\", filename.size());

	if (index != -1)
		newStr = filename.substr(index + 1, filename.size());

	write << __DATE__ << " " << __TIME__ << " ShaderException" << endl;
	write << newStr <<endl;
	write << "Line: " << lineNumber <<endl;
	write << "Function: " << function << endl;
	write << extraInfo << endl;
	write << endl;

	write.close();
}

VertexBufferException::VertexBufferException( const std::string& filename, const int& lineNumber, const std::string& function, const std::string& extraInfo )
{
	std::fstream write;

	write.open("Errors.txt", fstream::in | fstream::out | fstream::app);

	std::string newStr = filename;
	int index = filename.find_last_of("\\", filename.size());

	if (index != -1)
		newStr = filename.substr(index + 1, filename.size());

	write << __DATE__ << " " << __TIME__ << " VertexBufferException" << endl;
	write << newStr <<endl;
	write << "Line: " << lineNumber <<endl;
	write << "Function: " << function << endl;
	write << extraInfo << endl;
	write << endl;

	write.close();
}