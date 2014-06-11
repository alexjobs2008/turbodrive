#include "filesystem.h"

#include <QtCore/QFileInfo>

namespace Drive
{

bool FileSystem::isFileExists(const std::string& path) const
{
	return QFileInfo::exists(QString::fromStdString(path));
}

}
