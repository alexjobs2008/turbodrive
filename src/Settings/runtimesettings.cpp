#include "runtimesettings.h"

namespace Drive
{

static RuntimeSettings& RuntimeSettings::instance()
{
	static RuntimeSettings settings;
	return settings;
}

QVariant RuntimeSettings::get(RuntimeSettings::Key key) const
{
	return QVariant::invalid;
}

void RuntimeSettings::set(RuntimeSettings::Key key, QVariant value)
{
}

void RuntimeSettings::remove(RuntimeSettings::Key key) const
{
}

void RuntimeSettings::clear()
{
	m_storage.clear();
}

}
