#ifndef ADVANCED_WIDGET_H
#define ADVANCED_WIDGET_H

#include <QtWidgets/QFrame>

class QLineEdit;

class AdvancedWidget : public QFrame
{
    Q_OBJECT
public:
    AdvancedWidget(QWidget *parent = 0);

private slots:
    void on_pbMove_clicked(bool checked);

};

class AdvancedWidgetV2 : public QFrame
{
    Q_OBJECT
public:
    AdvancedWidgetV2(QWidget *parent = 0);

private slots:
    void on_pbMove_clicked(bool checked);

private:
    QLineEdit *leLocation;

};

#endif ADVANCED_WIDGET_H