#ifndef FILE_SYSTEM_FACTORY_H
#define FILE_SYSTEM_FACTORY_H

#include "ifilesystemfactory.h"
#include "filesystem.h"

namespace Drive
{

class FileSystemFactory: public IFileSystemFactory
{
public:
	virtual IFileSystem& instance() const override;

private:
	mutable FileSystem m_fileSystem;
};

}

#endif // FILE_SYSTEM_FACTORY_H
