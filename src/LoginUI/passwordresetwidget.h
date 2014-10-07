#ifndef PASSWORDRESETWIDGET_H
#define PASSWORDRESETWIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include "QsLog/QsLog.h"
#include "UtilUI/AuxWidgets.h"
#include "UtilUI/ExtraLabels.h"

namespace Drive
{


class PasswordResetWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PasswordResetWidget(
            const QString& username, QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *);

signals:
    void finished();

private slots:
    void on_resetButton_clicked(bool checked);
    void on_recalled_linkActivated(const QString&);

private:
    void initControls();

    CommonUI::LabeledEdit *phoneInput;
    QPushButton *resetButton;
    CommonUI::LinkLabel *recalled;

    QString username;
};

}

#endif // PASSWORDRESETWIDGET_H
