#include "factoriesstorage.h"

#include <QtCore/QtGlobal>

namespace Drive
{

FactoriesStorage& FactoriesStorage::instance()
{
	static FactoriesStorage s_instance;
	return s_instance;
}

void FactoriesStorage::registerFileSystem(IFileSystemFactoryPtr factory)
{
	m_fileSystem = std::move(factory);
}

ISingletonFactory<IFileSystem>& FactoriesStorage::fileSystem() const
{
	Q_ASSERT(m_fileSystem.get() != nullptr);
	return *m_fileSystem;
}

}
