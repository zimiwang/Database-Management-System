#pragma once
#include <fstream>
#include <sstream>
/* Directory that I put the dirent.h file
C:\Program Files(x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.29.30133\include
*/
#include <dirent.h>	// the header file that was needed. Download and add to directory.

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include "utils.h"

class FileHelper {
private:
	bool static CheckExtension(std::string filename, std::string ext);

public:
	void static listfiles(std::string dir, std::string ext);
	std::string static readfile(std::string dir, std::string filename);
	std::string static writefile(std::string contents, std::string filename, bool isbinary);

};

/// <summary>
/// Compares if filename size is greater than ext
/// </summary>
/// <param name="filename">filename</param>
/// <param name="ext">ext</param>
/// <returns>True if filename is greater than ext</returns>
bool FileHelper::CheckExtension(std::string filename, std::string ext) {
	return filename.size() >= ext.size() && 0 == filename.compare(filename.size() - ext.size(), ext.size(), ext);
}

/// <summary>
/// prints out a list of files in a directory with a file extension
/// </summary>
/// <param name="directory">The directory to print</param>
/// <param name="ext">the extension of the files to be printed</param>
void FileHelper::listfiles(std::string directory, std::string ext) {
	DIR* di;
	char* ptr1, * ptr2;
	int retn;
	struct dirent* dir;
	const char* direc = directory.c_str();
	const char* extension = ext.c_str();
	di = opendir(direc); //specify the directory name
	if (di)
	{
		while ((dir = readdir(di)) != NULL)
		{
			// strtok error. I security error cause
			ptr1 = strtok(dir->d_name, ".");
			ptr2 = strtok(NULL, ".");
			if (ptr2 != NULL)
			{
				retn = strcmp(ptr2, extension);
				if (retn == 0)
				{
					printf("> %s\n", ptr1);
				}
			}
		}
		closedir(di);
	}
}

/// <summary>
/// Reads a file from a directory
/// </summary>
/// <param name="dir">The directory to look in</param>
/// <param name="filename">The name of the file to open</param>
/// <returns>A string of the file content</returns>
std::string FileHelper::readfile(std::string dir, std::string filename) {
	std::string ret = "";
	std::ifstream file("data/" + filename);
	std::stringstream buffer;

	if (file.is_open()) {
		buffer << file.rdbuf();
		ret = buffer.str();

		file.close();
	}

	return ret;
}

/// <summary>
/// Writes to a file
/// </summary>
/// <param name="contents">The content to write</param>
/// <param name="filename">The name of the file</param>
/// <param name="isbinary"></param>
/// <returns>A string saying "closed"</returns>
std::string FileHelper::writefile(std::string contents, std::string filename, bool isbinary) {
	std::ofstream out(filename);

	out << contents;
	out.close();
	return "closed";	// Needed to return a string error.
}