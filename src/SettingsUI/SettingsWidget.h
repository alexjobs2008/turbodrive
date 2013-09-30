#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QtWidgets/QFrame>
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QResizeEvent>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QStyledItemDelegate;
class QLabel;


class SettingsWidget : public QFrame
{
    Q_OBJECT
public:
    SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

signals:
    void openFolder();
    void logout();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    void onTabChanged(QListWidgetItem *, QListWidgetItem *);

    void onSettingsGotDirty();

    void on_pbOK_clicked(bool checked);

    void accept();
    void reject();


private:
    void setupListView();    

    QListWidget* tabs;
    QStackedWidget *stackedWidget;

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
    static void printStyleOptionStates(const QStyleOptionViewItem & option, int i);

    QModelIndex helperIndex;
    QLabel *label;
};


#endif SETTINGS_WIDGET_H