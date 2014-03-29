#ifndef RUNTIMESETTINGS_H
#define RUNTIMESETTINGS_H

namespace Drive
{

class RuntimeSettings
{
public:
	enum Key
	{
		lastLogin,
	};

	static RuntimeSettings& instance();

	QVariant get(RuntimeSettings::Key key) const;
	void set(RuntimeSettings::Key key, QVariant value);

	void remove(RuntimeSettings::Key key) const;

	void clear();

private:
	RuntimeSettings(const RuntimeSettings& other);
	operator=(const RuntimeSettings& other);

	std::map<Key, QVariant> m_storage;
};

#endif // RUNTIMESETTINGS_H
