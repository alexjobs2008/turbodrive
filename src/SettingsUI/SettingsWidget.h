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
class QPushButton;

namespace Drive
{

struct ProfileData;
class AccountWidget;

class SettingsWidget : public QFrame
{
	Q_OBJECT
public:

	static SettingsWidget& instance();

public slots:
	void onProfileDataUpdated(const ProfileData& profileData);

signals:
	void openFolder();
	void logout();

protected:
	virtual void resizeEvent(QResizeEvent *event);

private slots:
	void on_tabs_currentItemChanged(QListWidgetItem*, QListWidgetItem*);

	void on_help_linkActivated(const QString& link);
	void on_OK_clicked(bool checked);
	void on_cancel_clicked(bool checked);
	void on_apply_clicked(bool checked);

	void onSettingsDirtyChanged(bool isDirty);

private:
	Q_DISABLE_COPY(SettingsWidget)
	SettingsWidget(QWidget *parent = 0);
	~SettingsWidget();

	void setupListView();

	static const QString linkHelp;

	QListWidget* tabs;
	QStackedWidget *stackedWidget;

	QPushButton *pbApply;

	AccountWidget *accountWidget;

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

}

#endif SETTINGS_WIDGET_H