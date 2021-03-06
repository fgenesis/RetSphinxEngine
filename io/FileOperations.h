#pragma once
#include <string>
#include <set>

namespace FileOperations
{
	//Read an entire file into memory and return the buffer, NULL on error.
	//The pointer returned from this function must be free()d.
	unsigned char* readFile(std::string filename, unsigned int* fileSize);

	std::set<std::string> readFilesFromDir(std::string sDirPath);
}