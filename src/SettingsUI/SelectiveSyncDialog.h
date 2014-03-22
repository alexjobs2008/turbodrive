#ifndef SELECTIVE_SYNC_H
#define SELECTIVE_SYNC_H

#include "APIClient/ApiTypes.h"
#include "APIClient/FilesService.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QTreeView>
#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QFileIconProvider>

namespace Drive
{

class TreeModel;
class TreeItem;

class SelectiveSyncDialog : public QDialog
{
	Q_OBJECT
public:
	SelectiveSyncDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);

public slots:
	virtual void accept();
	virtual void reject();

private slots:
	void onGetChildrenSucceeded(QList<RemoteFileDesc> list);
	void onGetChildrenFailed(QString);

private:
	QTreeView *treeView;
	TreeModel *model;
	TreeItem *root;
};

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	TreeModel(TreeItem *diskRoot, QObject *parent = 0);
	~TreeModel();

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	bool canFetchMore(const QModelIndex &parent) const;
	void fetchMore(const QModelIndex &parent);

signals:
	void loadFailed();

private slots:
	void onGetChildrenSucceeded(QList<RemoteFileDesc> list);
	void onGetChildrenFailed();

private:
	void loadItems(TreeItem* parentItem, const QModelIndex& parentIndex);

	GetChildrenResourceRef getChildrenResource;
	TreeItem *rootItem;
	QFileIconProvider iconProvider;
	TreeItem* currentLocadingItem;
	QModelIndex currentLocadingIndex;
};

class TreeItem
{
public:
	explicit TreeItem(const RemoteFileDesc &fileDesc, TreeItem *parent = 0);
	~TreeItem();

	void appendChild(TreeItem *child);

	TreeItem* child(int row);
	int childCount() const;
	RemoteFileDesc fileObject() const;
	int row() const;
	TreeItem* parent();

	Qt::CheckState checkState() const;
	void setCheckState(Qt::CheckState newCheckState);

private:
	QList<TreeItem*> childItems;
	RemoteFileDesc fileDesc;
	TreeItem *parentItem;
	Qt::CheckState state;
};

}

#endif // SELECTIVE_SYNC_H