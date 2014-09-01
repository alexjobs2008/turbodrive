#ifndef FACTORIES_STORAGE_H
#define FACTORIES_STORAGE_H

#include "ifilesystemfactory.h"

namespace Drive
{

class FactoriesStorage
{
public:
	static FactoriesStorage& instance();

	void registerFileSystem(IFileSystemFactoryPtr factory);
	ISingletonFactory<IFileSystem>& fileSystem() const;

private:
    FactoriesStorage() = default;
    ~FactoriesStorage() = default;

    FactoriesStorage(const FactoriesStorage&) = delete;
    FactoriesStorage& operator=(const FactoriesStorage&) = delete;

private:
	IFileSystemFactoryPtr m_fileSystem;
};

}

#endif // FACTORIES_STORAGE_H
