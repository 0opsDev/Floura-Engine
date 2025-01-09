#include"fileClass.h"

std::string get_fileC_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

FILEREAD::FILEREAD(const char* Filewrite, const char* FileBackup)
{
	std::string writeCode = get_fileC_contents(Filewrite);
	std::string writeBackupCode = get_fileC_contents(FileBackup);

	const char* fileSource = writeCode.c_str();
	const char* BackupSource = writeBackupCode.c_str();


	std::cout << "read " << writeCode;

}