#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QResizeEvent>

class QListView;
class QStyledItemDelegate;
class QLabel;


class SettingsWidget : public QFrame
{
    Q_OBJECT
public:
    SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

signals:

protected:
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    void accept();
    void reject();


private:
    void setupListView();

    QListView* tabs;

};

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    Delegate(QObject *parent = 0);
    virtual ~Delegate();

    virtual QSize sizeHint(const QStyleOptionViewItem & option,
                           const QModelIndex & index) const;
    
    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option,
                       const QModelIndex & index) const;
private:
    QModelIndex helperIndex;
    QLabel *label;
};


#endif SETTINGS_WIDGET_H