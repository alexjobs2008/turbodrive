#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

#include <QMap>
#include <QVariant>

namespace Drive
{

class RuntimeSettings
{
public:
	enum Key
	{
		login,
		password,
	};

	static RuntimeSettings& instance();

	QVariant get(RuntimeSettings::Key key) const;
	void set(RuntimeSettings::Key key, QVariant value);

	void remove(RuntimeSettings::Key key);

	void clear();

private:
	RuntimeSettings();
	RuntimeSettings(const RuntimeSettings& other);
	RuntimeSettings& operator=(const RuntimeSettings& other);

	QMap<Key, QVariant> m_storage;
};

}

#endif // RUNTIMESETTINGS_H
