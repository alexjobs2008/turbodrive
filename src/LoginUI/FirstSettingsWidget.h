#ifndef FIRST_SETTINGS_H
#define FIRST_SETTINGS_H

#include <QtWidgets/QFrame>

class QRadioButton;
class QPushButton;

class FirstSettingsWidget : public QFrame
{
	Q_OBJECT
public:
	FirstSettingsWidget(QWidget *parent = 0);

private slots:
	void on_typicalSettings_toggled(bool);
	void on_advancedSettings_toggled(bool);


private:

	QRadioButton *rbTypicalSettings;
	QRadioButton *rbAdvancedSettings;
	QPushButton *pbNextFinish;

};

#endif
