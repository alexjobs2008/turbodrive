#ifndef PASSWORDRESETWIDGET_H
#define PASSWORDRESETWIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include "QsLog/QsLog.h"
#include "UtilUI/AuxWidgets.h"
#include "UtilUI/ExtraLabels.h"
#include "UtilUI/SpinnerWidget.h"

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
    void moveEvent(QMoveEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

signals:
    void finished();

private slots:
    void on_resetButton_clicked(bool checked);
    void on_recalled_linkActivated(const QString&);
    void onPasswordResetFailed(const QString& message);

private:
    void initControls();
    void enableControls(bool enable);
    void moveErrorWindow();

    CommonUI::LabeledEdit *phoneInput;
    QPushButton *resetButton;
    CommonUI::SpinnerWidget *m_spinner;
    CommonUI::LinkLabel *recalled;

    QString username;
    CommonUI::MessageWindow *errorWindow;
};

}

#endif // PASSWORDRESETWIDGET_H
