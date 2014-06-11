#include "filesystemfactory.h"

namespace Drive
{

IFileSystem& FileSystemFactory::instance() const
{
	return m_fileSystem;
}

}

