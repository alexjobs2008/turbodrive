#ifndef I_FILE_SYSTEM_FACTORY_H
#define I_FILE_SYSTEM_FACTORY_H

#include "isingletonfactory.h"
#include "ifilesystem.h"

#include <memory>

namespace Drive
{

class IFileSystemFactory: public ISingletonFactory<IFileSystem>
{
public:
	virtual ~IFileSystemFactory() = default;
};

typedef std::unique_ptr<IFileSystemFactory> IFileSystemFactoryPtr;

}

#endif // I_FILE_SYSTEM_FACTORY_H
