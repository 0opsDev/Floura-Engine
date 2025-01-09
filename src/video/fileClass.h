#ifndef FILE_CLASS_H
#define FILE_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);
class FILEREAD
{
public:
	FILEREAD(const char* Filewrite, const char* FileBackup);
};


#endif