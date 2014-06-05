#include "SelectiveSyncDialog.h"

#include <QtGui/QIcon>
#include <QtGui/QFont>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

#include "QsLog/QsLog.h"

namespace Drive
{

SelectiveSyncDialog::SelectiveSyncDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	// avoid app close on window close
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setWindowFlags(Qt::CustomizeWindowHint
		| Qt::WindowTitleHint
		| Qt::WindowCloseButtonHint);

	setWindowIcon(QIcon(":/appicon.ico"));

	setWindowTitle(tr("Selective Sync"));

	QFont largerFont("MS Shell Dlg", 10);
	setFont(largerFont);

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *topLabel = new QLabel(tr("Select which files and folder to sync:"), this);

	treeView = new QTreeView(this);
	treeView->header()->close();

	QDialogButtonBox *buttonBox =
		new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel
		, Qt::Horizontal
		, this);

	connect(buttonBox, &QDialogButtonBox::accepted,
			this, &SelectiveSyncDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,
			this, &SelectiveSyncDialog::reject);

	layout->addSpacing(4);
	layout->addWidget(topLabel);
	layout->addSpacing(8);
	layout->addWidget(treeView);
	layout->addSpacing(8);
	layout->addWidget(buttonBox);

	GetChildrenResourceRef getChildrenRes =
			GetChildrenResource::create();

	connect(getChildrenRes.data(), &GetChildrenResource::succeeded,
			this, &SelectiveSyncDialog::onGetChildrenSucceeded);

	connect(getChildrenRes.data(), &GetChildrenResource::failed,
			this, &SelectiveSyncDialog::onGetChildrenFailed);

	getChildrenRes->getChildren(-2);

}

void SelectiveSyncDialog::accept()
{
	QDialog::accept();
}

void SelectiveSyncDialog::reject()
{
	QDialog::reject();
}

void SelectiveSyncDialog::onGetChildrenSucceeded(QList<RemoteFileDesc> list)
{
	RemoteFileDesc rootFileObj;
	TreeItem *root = new TreeItem(rootFileObj, 0);

	for (int i = 0; i < list.size(); i++)
	{
		TreeItem *item = new TreeItem(list.value(i), root);
		root->appendChild(item);
	}

	model = new TreeModel(root, this);
	treeView->setModel(model);
}

void SelectiveSyncDialog::onGetChildrenFailed()
{
	reject();
}

TreeModel::TreeModel(TreeItem *diskRoot, QObject *parent)
	: QAbstractItemModel(parent)
{
	rootItem = diskRoot;
	currentLocadingItem = 0;
}

TreeModel::~TreeModel()
{
}

void TreeModel::loadItems(TreeItem* parentItem, const QModelIndex& parentIndex)
{
	QLOG_TRACE() << "TreeModel::loadItems()";

	getChildrenResource = GetChildrenResource::create();

	connect(getChildrenResource.data(), &GetChildrenResource::succeeded,
			this, &TreeModel::onGetChildrenSucceeded);

	connect(getChildrenResource.data(), &GetChildrenResource::failed,
			this, &TreeModel::onGetChildrenFailed);

	getChildrenResource->getChildren(parentItem->fileObject().id);

	currentLocadingItem = parentItem;
	currentLocadingIndex = parentIndex;
}

void TreeModel::onGetChildrenSucceeded(QList<RemoteFileDesc> list)
{
	QLOG_TRACE() << "TreeModel::onGetChildrenSucceeded()";

	if (!currentLocadingItem)
		return;

	beginInsertRows(currentLocadingIndex, 0, list.size() - 1);

	for (int i = 0; i < list.size(); i++)
	{
		TreeItem *item = new TreeItem(list.value(i), currentLocadingItem);
		currentLocadingItem->appendChild(item);
	}

	endInsertRows();

	currentLocadingItem = 0;
}

void TreeModel::onGetChildrenFailed()
{
	QLOG_TRACE() << "TreeModel::onGetChildrenFailed()";

	emit loadFailed();
	currentLocadingItem = 0;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	QLOG_TRACE() << "TreeModel::index()" << row << column << parent;

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex & index) const
{
	QLOG_TRACE() << "TreeModel::parent()" << index;

	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	QLOG_TRACE() << "file name: " << childItem->fileObject().name
		<< ", parent row, col:" << index.row() << "," << index.column();

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	QLOG_TRACE() << "TreeModel::rowCount()" << parent << parent.isValid();

	TreeItem *item;

	if (!parent.isValid())
	{
		item = rootItem;
	}
	else
	{
		item = static_cast<TreeItem*>(parent.internalPointer());
	}

	return item->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	QLOG_TRACE() << "TreeModel::data()" << index << role;

	if (!index.isValid())
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
	RemoteFileDesc fileObj = item->fileObject();

	if (role == Qt::DisplayRole)
		return fileObj.name;

	if (role == Qt::DecorationRole)
	{
		if (fileObj.type == RemoteFileDesc::Dir)
		{
			return iconProvider.icon(QFileIconProvider::Folder);
		}
		else
		{
			QFileInfo fileInfo(fileObj.name);
			return iconProvider.icon(fileInfo);
		}
	}

	if (role == Qt::CheckStateRole)
		return item->checkState();

	return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

bool TreeModel::setData(const QModelIndex & index, const QVariant &value, int role)
{
	if (role != Qt::CheckStateRole)
		return false;

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
	item->setCheckState(static_cast<Qt::CheckState>(value.toInt()));

	QLOG_TRACE() << "setData()" << value << "role: " << role;
	return false;
}


bool TreeModel::hasChildren(const QModelIndex &parent) const
{
	QLOG_TRACE() << "TreeModel::hasChildren()" << parent;

	TreeItem *item;

	if (!parent.isValid())
	{
		// item == rootItem
		return true;
	}

	item = static_cast<TreeItem*>(parent.internalPointer());
	RemoteFileDesc fileObj = item->fileObject();

	QLOG_TRACE() << "childCount: " << item->childCount()
		<< "; fileObj.hasChildren: " << fileObj.hasChildren
		<< "; fileObj.hasSubfolders" << fileObj.hasSubfolders;

	bool result = item->childCount() || fileObj.hasChildren || fileObj.hasSubfolders;

	QLOG_TRACE() << "hasChildren?" << result;

	return result;
}

bool TreeModel::canFetchMore(const QModelIndex &parent) const
{
	QLOG_TRACE() << "TreeModel::canFetchMore()" << parent;

	TreeItem *item;

	if (!parent.isValid())
	{
		item = rootItem;
		return false;
	}
	else
	{
		item = static_cast<TreeItem*>(parent.internalPointer());
	}

	bool result = !item->childCount();

	QLOG_TRACE() << "canFetchMore? " << result;

	return result;
}

void TreeModel::fetchMore(const QModelIndex &parent)
{
	QLOG_TRACE() << "TreeModel::fetchMore()" << parent;

	TreeItem *item = static_cast<TreeItem*>(parent.internalPointer());
	loadItems(item, parent);
}

// ===================================================================================


TreeItem::TreeItem(const RemoteFileDesc &fileDesc, TreeItem *parent)
	: fileDesc(fileDesc)
	, parentItem(parent)
{
}

TreeItem::~TreeItem()
{
	qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
	return childItems.value(row);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

RemoteFileDesc TreeItem::fileObject() const
{
	return fileDesc;
}

TreeItem *TreeItem::parent()
{
	return parentItem;
}

int TreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

Qt::CheckState TreeItem::checkState() const
{
	return state;
}

void TreeItem::setCheckState(Qt::CheckState newCheckState)
{
	state = newCheckState;
}

}
