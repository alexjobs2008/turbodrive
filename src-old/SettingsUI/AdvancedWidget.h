#ifndef ADVANCED_WIDGET_H
#define ADVANCED_WIDGET_H

#include <QtWidgets/QFrame>

class QLineEdit;
class QCheckBox;
class QComboBox;

namespace Drive
{

class AdvancedWidget : public QFrame
{
    Q_OBJECT
public:
    AdvancedWidget(QWidget *parent = 0);

private slots:
    void on_desktopNotifications_toggled(bool checked);
    void on_moveFolder_clicked(bool checked);
    void on_folderPath_textChanged(const QString &text);
    void on_autostart_toggled(bool checked);
    void on_language_currentIndexChanged(int index);

private:
    void setFromSettings();

    QLineEdit *leFolderPath;
    QCheckBox *cNotifications;
    QCheckBox *cAutostart;
    QComboBox *cbLanguage;

};

}

#endif ADVANCED_WIDGET_H