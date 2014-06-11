#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "ifilesystem.h"

namespace Drive
{

class FileSystem: public IFileSystem
{
public:
	virtual bool isFileExists(const std::string& path) const override;
};

}

#endif // FILE_SYSTEM_H
