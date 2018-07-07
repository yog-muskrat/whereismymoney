#include "wimmmodel.h"
#include "sqltools.h"
#include "tools.h"
#include "changevaluecommand.h"

#include <QIcon>
#include <QFont>
#include <QColor>
#include <QDebug>
#include <QSettings>
#include <QUndoStack>
#include <QApplication>

WIMMModel::WIMMModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	pUndoStack = new QUndoStack(this);
}

WIMMModel::~WIMMModel()
{
	qDeleteAll(mData);
}

void WIMMModel::addMonths(const QList<MonthItem *>& items)
{
	if(items.isEmpty())
	{
		return;
	}

	int row = mData.count();

	beginInsertRows(QModelIndex(), row, row + items.count() -1 );

	addToCache(items);

	mData.append( items );

	endInsertRows();
}

void WIMMModel::addMonth(MonthItem *item, bool updateEstFromPrevious)
{
	QList<MonthItem*> items;

	if(updateEstFromPrevious && !mData.isEmpty())
	{
		MonthItem *prevMonth = mData.last();

		foreach(GroupItem *prevGroup, prevMonth->groups())
		{
			foreach(GroupItem *newGroup, item->groups())
			{
				if(prevGroup->id() != newGroup->id())
				{
					continue;
				}

				foreach(CategoryItem *prevCat, prevGroup->categories())
				{
					foreach (CategoryItem *newCat, newGroup->categories())
					{
						if(prevCat->categoryId() != newCat->categoryId())
						{
							continue;
						}

						newCat->setValue(WIMMItem::FirstEst,  prevCat->value(WIMMItem::FirstEst));
						newCat->setValue(WIMMItem::SecondEst,  prevCat->value(WIMMItem::SecondEst));
					}
				}
			}
		}
	}

	items << item;
	addMonths(items);
}

void WIMMModel::removeMonth(int monthId)
{
	int row = -1;

	for(int i = 0; i < mData.count(); ++i)
	{
		if(mData[i]->id() == monthId)
		{
			row = i;
			break;
		}
	}

	if(row < 0)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), row, row);

	auto monthItem = mData.takeAt(row);
	removeFromCache({monthItem});

	delete monthItem;

	endRemoveRows();
}

QModelIndex WIMMModel::monthIndex(int monthId) const
{
	for(int i = 0; i < mData.count(); ++i)
	{
		if(mData[i]->id() == monthId)
		{
			return index(i, 0, QModelIndex());
		}
	}

	return {};
}

QModelIndex WIMMModel::monthIndex(int year, int month) const
{
	for(int i = 0; i < mData.count(); ++i)
	{
		if(mData[i]->year() == year && mData[i]->month() == month)
		{
			return index(i, 0);
		}
	}

	return {};
}

int WIMMModel::monthId(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() == Month)
	{
		return item->id();
	}

	if(item->level() == Group)
	{
		auto *group = dynamic_cast<GroupItem*>(item);
		Q_ASSERT(group);

		return group->month()->id();
	}

	if(item->level() == Category)
	{
		auto *category= dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		return category->group()->month()->id();
	}

	return -1;
}

bool WIMMModel::isMoneyIndex(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	return item->level() == Category &&
			(index.column() == COL_FirstHalfIn ||
			 index.column() == COL_FirstHalfOut ||
			 index.column() == COL_FirstHalfEst ||
			 index.column() == COL_SecondHalfIn ||
			 index.column() == COL_SecondHalfOut ||
			 index.column() == COL_SecondHalfEst);
}

QString WIMMModel::comment(const QModelIndex &index) const
{
	if(isMoneyIndex(index))
	{
		auto *item = dynamic_cast<CategoryItem*>( itemForIndex(index));
		Q_ASSERT(item);

		int col = index.column();

		if(col == COL_FirstHalfIn)
		{
			return item->comment(WIMMItem::FirstIn);
		}

		if(col == COL_FirstHalfOut)
		{
			return item->comment(WIMMItem::FirstOut);
		}

		if(col == COL_FirstHalfEst)
		{
			return item->comment(WIMMItem::FirstEst);
		}

		if(col == COL_SecondHalfIn)
		{
			return item->comment(WIMMItem::SecondIn);
		}

		if(col == COL_SecondHalfOut)
		{
			return item->comment(WIMMItem::SecondOut);
		}

		if(col == COL_SecondHalfEst)
		{
			return item->comment(WIMMItem::SecondEst);
		}
	}

	return "";
}

void WIMMModel::setComment(const QModelIndex &index, const QString& comment)
{
	if(isMoneyIndex(index))
	{
		auto *item = dynamic_cast<CategoryItem*>( itemForIndex(index));
		Q_ASSERT(item);

		int col = index.column();

		bool changed = false;
		if(col == COL_FirstHalfIn)
		{
			changed = true;
			item->setComment(WIMMItem::FirstIn, comment);
		}
		else if(col == COL_FirstHalfOut)
		{
			changed = true;
			item->setComment(WIMMItem::FirstOut, comment);
		}
		else if(col == COL_FirstHalfEst)
		{
			changed = true;
			item->setComment(WIMMItem::FirstEst, comment);
		}
		else if(col == COL_SecondHalfIn)
		{
			changed = true;
			item->setComment(WIMMItem::SecondIn, comment);
		}
		else if(col == COL_SecondHalfOut)
		{
			changed = true;
			item->setComment(WIMMItem::SecondOut, comment);
		}
		else if(col == COL_SecondHalfEst)
		{
			changed = true;
			item->setComment(WIMMItem::SecondEst, comment);
		}

		if(changed)
		{
			item->save();
			emit dataChanged(index, index);
		}
	}
}

ItemLevel WIMMModel::indexLevel(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	return item->level();
}

int WIMMModel::categoryId(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() != Category)
	{
		return -1;
	}

	auto *category = dynamic_cast<CategoryItem*>(item);
	Q_ASSERT(category);

	return category->categoryId();
}

QMap<int, QPair<double, double> > WIMMModel::totals() const
{
	QMap<int, QPair<double, double> > result;

	foreach(MonthItem *month, mData)
	{
		foreach(GroupItem *group, month->groups())
		{
			foreach(CategoryItem *category, group->categories())
			{
				int id = category->categoryId();
				double sumIn = category->value( WIMMItem::FirstIn) + category->value(WIMMItem::SecondIn);
				double sumOut = category->value( WIMMItem::FirstOut) + category->value(WIMMItem::SecondOut);

				result[id].first += sumIn;
				result[id].second += sumOut;
			}
		}
	}

	return result;
}

void WIMMModel::clear()
{
	if(rowCount() == 0)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), 0, rowCount()-1);
	qDeleteAll(mData);
	mData.clear();
	mCache.clear();
	mLastId = 0;
	endRemoveRows();
}

bool WIMMModel::save()
{
	foreach(MonthItem *month, mData)
	{
		month->save();
	}
	return true;
}

int WIMMModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return COL_Count;
}

int WIMMModel::rowCount(const QModelIndex &parent) const
{
	if(mData.empty())
	{
		return 0;
	}

	WIMMItem *item = itemForIndex(parent);

	if(item != nullptr)
	{
		if(item->level() == Month)
		{
			auto *month = dynamic_cast<MonthItem*>(item);
			Q_ASSERT(month);

			return month->groups().count();
		}

		if(item->level() == Group)
		{
			auto *group = dynamic_cast<GroupItem*>(item);
			Q_ASSERT(group);

			return group->categories().count();

		}

		if(item->level() == Category)
		{
			return 0;
		}
	}

	return mData.count();
}

QVariant WIMMModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();

	if(row < 0 || row >= rowCount(parent(index)))
	{
		return QVariant();
	}

	if(role == Qt::EditRole)
	{
		return editRole(index);
	}

	if(role == Qt::DisplayRole)
	{
		return displayRole(index);
	}

	if(role == Qt::DecorationRole)
	{
		return decorationRole(index);
	}

	if(role == Qt::BackgroundColorRole)
	{
		return backgroundRole(index);
	}

	if(role == Qt::FontRole)
	{
		return fontRole(index);
	}

	if(role == Qt::TextAlignmentRole)
	{
		return alignmentRole(index);
	}

	if(role == Qt::ForegroundRole)
	{
		return textColorRole(index);
	}

	if(role == Qt::ToolTipRole)
	{
		return tooltipRole(index);
	}

	return QVariant();
}

bool WIMMModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() != Category || (role != Qt::EditRole && role != Qt::DisplayRole))
	{
		return false;
	}

	auto * category = dynamic_cast<CategoryItem*>(item);
	Q_ASSERT(category);

	if(isMoneyIndex(index))
	{
		pUndoStack->push(new ChangeValueCommand(index, value, this));
	}

	return true;
}

Qt::ItemFlags WIMMModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	WIMMItem *item = itemForIndex(index);
	if(item == nullptr)
	{
		return flags;
	}

	if(item->level() == Category && index.column() != COL_Title)
	{
		flags	|= Qt::ItemIsEditable;
	}

	return flags;
}

QVariant WIMMModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal )
	{
		return QVariant();
	}

	if(role == Qt::DisplayRole)
	{
		if(section == COL_DbId)
		{
			return "БД";
		}

		if(section == COL_Title)
		{
			return "Категория";
		}

		if(section == COL_FirstHalfIn)
		{
			return QString("%0 (I)").arg(QChar(0x002b));
		}

		if(section == COL_FirstHalfOut)
		{
			return QString("%0 (I)").arg(QChar(0x2212));
		}

		if(section == COL_FirstHalfEst)
		{
			return "? (I)";
		}

		if(section == COL_SecondHalfIn)
		{
			return QString("%0 (II)").arg(QChar(0x002b));
		}

		if(section == COL_SecondHalfOut)
		{
			return QString("%0 (II)").arg(QChar(0x2212));
		}

		if(section == COL_SecondHalfEst)
		{
			return "? (II)";
		}
	}
	else if(role == Qt::FontRole)
	{
		QSettings set("mudbay", "wimm");
		auto f = set.value("font", qApp->font()).value<QFont>();
		f.setBold(true);
		return f;
	}
	else if(role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}
	else if(role == Qt::ToolTipRole)
	{
		if(section == COL_DbId)
		{
			return "Код в БД";
		}

		if(section == COL_Title)
		{
			return "Название категории";
		}

		if(section == COL_FirstHalfIn)
		{
			return "Первая половина месяца: приход";
		}

		if(section == COL_FirstHalfOut)
		{
			return "Первая половина месяца: расход";
		}

		if(section == COL_FirstHalfEst)
		{
			return "Первая половина месяца: справочно";
		}

		if(section == COL_SecondHalfIn)
		{
			return "Вторая половина месяца: приход";
		}

		if(section == COL_SecondHalfOut)
		{
			return "Вторая половина месяца: расход";
		}

		if(section == COL_SecondHalfEst)
		{
			return "Вторая половина месяца: справочно";
		}
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant WIMMModel::displayRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	int col = index.column();

	if(col == COL_DbId)
	{
		return item->id();
	}

	if(col == COL_Title)
	{
		return item->name();
	}

	if(col == COL_FirstHalfIn )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstIn));
	}

	if(col == COL_FirstHalfOut )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstOut));
	}

	if(col == COL_FirstHalfEst )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstEst));
	}

	if(col == COL_SecondHalfIn )
	{
		return Tools::moneyString( item ->value(WIMMItem::SecondIn));
	}

	if(col == COL_SecondHalfOut )
	{
		return Tools::moneyString( item ->value(WIMMItem::SecondOut));
	}

	if(col == COL_SecondHalfEst)
	{
		return Tools::moneyString( item ->value(WIMMItem::SecondEst));
	}

	return QVariant();
}

QVariant WIMMModel::editRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	int col = index.column();

	if(col == COL_DbId)
	{
		return item->id();
	}

	if(col == COL_Title)
	{
		return item->name();
	}

	if(col == COL_FirstHalfIn)
	{
		return item->value(WIMMItem::FirstIn);
	}

	if(col == COL_FirstHalfOut)
	{
		return item->value(WIMMItem::FirstOut);
	}

	if(col == COL_FirstHalfEst)
	{
		return item->value(WIMMItem::FirstEst);
	}

	if(col == COL_SecondHalfIn)
	{
		return item->value(WIMMItem::SecondIn);
	}

	if(col == COL_SecondHalfOut)
	{
		return item->value(WIMMItem::SecondOut);
	}

	if(col == COL_SecondHalfEst)
	{
		return item->value(WIMMItem::SecondEst);
	}

	return QVariant();
}

QVariant WIMMModel::decorationRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() == Category)
	{
		auto *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		if(index.column() == COL_FirstHalfIn && !category->comment(WIMMItem::FirstIn).isEmpty())
		{
			return QIcon(":comment");
		}

		if(index.column() == COL_FirstHalfOut && !category->comment(WIMMItem::FirstOut).isEmpty())
		{
			return QIcon(":comment");
		}

		if(index.column() == COL_FirstHalfEst && !category->comment(WIMMItem::FirstEst).isEmpty())
		{
			return QIcon(":comment");
		}

		if(index.column() == COL_SecondHalfIn && !category->comment(WIMMItem::SecondIn).isEmpty())
		{
			return QIcon(":comment");
		}

		if(index.column() == COL_SecondHalfOut && !category->comment(WIMMItem::SecondOut).isEmpty())
		{
			return QIcon(":comment");
		}

		if(index.column() == COL_SecondHalfEst && !category->comment(WIMMItem::SecondEst).isEmpty())
		{
			return QIcon(":comment");
		}
	}

	return QVariant();
}

QVariant WIMMModel::backgroundRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	int col = index.column();

	QColor c(255,255,255);

	if(col == COL_FirstHalfIn || col == COL_SecondHalfIn)
	{
		c.setRgb(200, 255, 200);
	}
	else if(col == COL_FirstHalfOut || col == COL_SecondHalfOut)
	{
		c.setRgb(255, 200, 200);
	}
	else if(col == COL_FirstHalfEst || col == COL_SecondHalfEst)
	{
		c.setRgb(220, 250, 255);
	}

	if(item->level() == Group)
	{
		//		c = c.darker(110);
	}
	else if(item->level() == Month)
	{
		//		c = c.darker(110);
	}

	return c;
}

QVariant WIMMModel::fontRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	QFont f = qApp->font();

	if(item->level() == Month)
	{
		f.setBold(true);
		f.setPointSize( f.pointSize() + 1 );
	}
	else if(item->level() == Group)
	{
		f.setBold(true);
	}

	return f;
}

QVariant WIMMModel::textColorRole(const QModelIndex &index) const
{
	if(index.column() == COL_FirstHalfEst || index.column() == COL_SecondHalfEst)
	{
		return QColor(100,100,100);
	}

	return QVariant();
}

QVariant WIMMModel::alignmentRole(const QModelIndex &index) const
{
	if(index.column() != COL_Title)
	{
		return Qt::AlignCenter;
	}

	return QVariant();
}

QVariant WIMMModel::tooltipRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() == Category)
	{
		auto *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		if(index.column() == COL_FirstHalfIn)
		{
			return category->comment(WIMMItem::FirstIn);
		}

		if(index.column() == COL_FirstHalfOut)
		{
			return category->comment(WIMMItem::FirstOut);
		}

		if(index.column() == COL_FirstHalfEst)
		{
			return category->comment(WIMMItem::FirstEst);
		}

		if(index.column() == COL_SecondHalfIn)
		{
			return category->comment(WIMMItem::SecondIn);
		}

		if(index.column() == COL_SecondHalfOut)
		{
			return category->comment(WIMMItem::SecondOut);
		}

		if(index.column() == COL_SecondHalfEst)
		{
			return category->comment(WIMMItem::SecondEst);
		}
	}

	return QVariant();
}

WIMMItem *WIMMModel::itemForIndex(const QModelIndex &index) const
{
	if(!index.isValid())
	{
		return nullptr;
	}

	auto const id = index.internalId();

	Q_ASSERT(mCache.contains(id));

	return mCache.value(id);
}

void WIMMModel::emitDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

void WIMMModel::addToCache(QList<MonthItem *> months)
{
	for(auto month : months)
	{
		mCache[nextCacheId()] = month;

		for(auto group : month->groups())
		{
			mCache[nextCacheId()] = group;

			for(auto category : group->categories())
			{
				mCache[nextCacheId()] = category;
			}
		}
	}
}

void WIMMModel::removeFromCache(QList<MonthItem *> months)
{
	for(auto month : months)
	{
		mCache.remove(mCache.key(month));

		for(auto group : month->groups())
		{
			mCache.remove(mCache.key(group));

			for(auto category : group->categories())
			{
				mCache.remove(mCache.key(category));
			}
		}
	}
}

quintptr WIMMModel::cacheId(WIMMItem *item) const
{
	Q_ASSERT(mCache.values().contains(item));
	return mCache.key(item);
}

quintptr WIMMModel::nextCacheId()
{
	return mLastId++;
}

QModelIndex WIMMModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(column);

	if(parent.isValid())
	{
		WIMMItem *item = itemForIndex(parent);
		if(item->level() == Month)
		{
			auto *month = dynamic_cast<MonthItem*>(item);
			Q_ASSERT(month); /// wtf?

			if(row < 0 || row >= month->groups().count())
			{
				return {};
			}

			auto const id = cacheId(month->groups().at(row));

			return createIndex(row, column, id);
		}

		if(item->level() == Group)
		{
			auto *group = dynamic_cast<GroupItem*>(item);
			Q_ASSERT(group); /// wtf?

			if(row < 0 || row >= group->categories().count())
			{
				return {};
			}

			auto const id = cacheId(group->categories().at(row));

			return createIndex(row, column, id);
		}

		// Этого быть не должно
		Q_ASSERT(false);
		return {};
	}

	if(row < 0 || row >= mData.count())
	{
		return {};
	}

	auto const id = cacheId(mData.value(row));

	return createIndex(row, column, id);
}

QModelIndex WIMMModel::parent(const QModelIndex &child) const
{
	WIMMItem *item = itemForIndex(child);

	if(item == nullptr)
	{
		return {};
	}

	if(item->level() == Month)
	{
		return {};
	}

	if(item->level() == Group)
	{
		auto *group = dynamic_cast<GroupItem*>(item);
		Q_ASSERT(group);

		int row = mData.indexOf(group->month());
		if(row < 0)
		{
			return {};
		}

		auto const id = cacheId(group->month());

		return createIndex(row, 0, id);
	}

	if(item->level() == Category)
	{
		auto *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		int row = category->group()->month()->groups().indexOf(category->group());
		if(row < 0)
		{
			return {};
		}

		auto const id = cacheId(category->group());

		return createIndex(row, 0, id);
	}

	return {};
}
