#ifndef ACCOUNT_WIDGET_H
#define ACCOUNT_WIDGET_H

#include <QtWidgets/QFrame>

class AccountWidget : public QFrame
{
    Q_OBJECT
public:
    AccountWidget(QWidget *parent = 0);

signals:
    void openFolder();
    void logout();

private slots:
    void on_folderLabel_linkActivated(const QString &);
    void on_pbLogout_clicked(bool checked);

};

#endif ACCOUNT_WIDGET_H