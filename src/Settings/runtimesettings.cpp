#include "runtimesettings.h"

namespace Drive
{

RuntimeSettings& RuntimeSettings::instance()
{
	static RuntimeSettings settings;
	return settings;
}

QVariant RuntimeSettings::get(RuntimeSettings::Key key) const
{
	return m_storage.value(key, QVariant::Invalid);
}

void RuntimeSettings::set(RuntimeSettings::Key key, QVariant value)
{
	m_storage[key] = value;
}

void RuntimeSettings::remove(RuntimeSettings::Key key)
{
	m_storage.remove(key);
}

void RuntimeSettings::clear()
{
	m_storage.clear();
}

RuntimeSettings::RuntimeSettings()
{
}

}
