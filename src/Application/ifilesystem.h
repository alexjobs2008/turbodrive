#ifndef I_FILE_SYSTEM_H
#define I_FILE_SYSTEM_H

#include <string>

namespace Drive
{

class IFileSystem
{
public:
	virtual bool isFileExists(const std::string& path) const = 0;
};

}

#endif // I_FILE_SYSTEM_H
