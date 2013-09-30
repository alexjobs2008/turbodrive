#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QtWidgets/QFrame>
#include "QsLog/QsLog.h"

class LoginWidget : public QFrame
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = 0);
    ~LoginWidget() { QLOG_DEBUG() << "abra"; };

};

#endif 