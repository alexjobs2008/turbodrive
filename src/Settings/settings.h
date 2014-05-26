#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QSettings>

namespace Drive
{

class Settings : public QObject
{
	Q_OBJECT
private:
	Settings(QObject *parent = 0);
	~Settings();
	Q_DISABLE_COPY(Settings);

public:
	static const QString version;
	static const QString email;
	static const QString password;
	static const QString autoLogin;
	static const QString forceRelogin;
	static const QString folderPath;
	static const QString desktopNotifications;
	static const QString autostart;
	static const QString language;
	static const QString limitDownload;
	static const QString limitUpload;
	static const QString downloadSpeed;
	static const QString uploadSpeed;
	static const QString proxyUsage;
	static const QString proxyCustomSettings;
	static const QString env;
	static const QString remoteConfig;

	enum Kind
	{
		RealSetting = 0,
		CandidateSetting
	};

	static Settings& instance();

	QVariant get(const QString& settingName) const;

	void set(const QString& settingName, QVariant value,
		Kind kind = CandidateSetting);

	static QList<int> supportedLanguages();

public slots:
	void apply();
	void cancel();

signals:
	// emitted when real (not candidate) settings changed:
	void settingChanged(const QString& settingName, QVariant oldValue,
		QVariant newValue);

	// this signal emitted when candidate (not real) settings changed:
	// - if a setting has been changed, but not applied yet: isOn == true
	// - if candidate settings were applied: : isOn == false
	void dirtyStateChanged(bool isDirty);

	// helper signals:
	void folderPathChanged(const QString& folderPath);

private:
	QVariant defaultSettingValue(const QString& settingName) const;
	QString defaultFolderPath() const;

	bool applyImmediately;
	QMap<QString, QVariant> candidateSettings;

	QSettings *settings;
};


#ifdef Q_OS_WIN

class WindowsAutoexec
{
public:
	static bool get();
	static void set(bool autoexec);
};

}

#endif // Q_OS_WIN

#endif // SETTINGS_H
