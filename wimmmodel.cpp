#include "wimmmodel.h"
#include "sqltools.h"
#include "tools.h"

#include <QIcon>
#include <QFont>
#include <QColor>
#include <QDebug>
#include <QApplication>

WIMMModel::WIMMModel(QObject *parent)
	: QAbstractItemModel(parent)
{
}

WIMMModel::~WIMMModel()
{
	qDeleteAll(mData);
}

void WIMMModel::addMonths(QList<MonthItem *> items)
{
	if(items.isEmpty())
	{
		return;
	}

	int row = mData.count();

	beginInsertRows(QModelIndex(), row, row + items.count() -1 );
	mData.append( items );
	endInsertRows();
}

void WIMMModel::addMonth(MonthItem *item)
{
	QList<MonthItem*> items;
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
	delete mData.takeAt(row);
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

	return QModelIndex();
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

	return QModelIndex();
}

int WIMMModel::monthId(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() == Month)
	{
		return item->id();
	}
	else if(item->level() == Group)
	{
		GroupItem *group = dynamic_cast<GroupItem*>(item);
		Q_ASSERT(group);

		return group->month()->id();
	}
	else if(item->level() == Category)
	{
		CategoryItem *category= dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		return category->group()->month()->id();
	}

	return -1;
}

bool WIMMModel::hasComment(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() == Category &&
			(index.column() == COL_FirstHalfIn ||
			 index.column() == COL_FirstHalfOut ||
			 index.column() == COL_FirstHalfEst ||
			 index.column() == COL_SecondHalfIn ||
			 index.column() == COL_SecondHalfOut ||
			 index.column() == COL_SecondHalfEst))
	{
		return true;
	}

	return false;
}

QString WIMMModel::comment(const QModelIndex &index) const
{
	if(hasComment(index))
	{
		CategoryItem *item = dynamic_cast<CategoryItem*>( itemForIndex(index));
		Q_ASSERT(item);

		int col = index.column();

		if(col == COL_FirstHalfIn)
		{
			return item->comment(WIMMItem::FirstIn);
		}
		else if(col == COL_FirstHalfOut)
		{
			return item->comment(WIMMItem::FirstOut);
		}
		else if(col == COL_FirstHalfEst)
		{
			return item->comment(WIMMItem::FirstEst);
		}
		else if(col == COL_SecondHalfIn)
		{
			return item->comment(WIMMItem::SecondIn);
		}
		else if(col == COL_SecondHalfOut)
		{
			return item->comment(WIMMItem::SecondOut);
		}
		else if(col == COL_SecondHalfEst)
		{
			return item->comment(WIMMItem::SecondEst);
		}
	}

	return "";
}

void WIMMModel::setComment(const QModelIndex &index, QString comment)
{
	if(hasComment(index))
	{
		CategoryItem *item = dynamic_cast<CategoryItem*>( itemForIndex(index));
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

	CategoryItem *category = dynamic_cast<CategoryItem*>(item);
	Q_ASSERT(category);

	return category->categoryId();
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
	endRemoveRows();
}

int WIMMModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return COL_Count;
}

int WIMMModel::rowCount(const QModelIndex &parent) const
{
	WIMMItem *item = itemForIndex(parent);

	if(item)
	{
		if(item->level() == Month)
		{
			MonthItem *month = dynamic_cast<MonthItem*>(item);
			Q_ASSERT(month);

			return month->groups().count();
		}
		else if(item->level() == Group)
		{
			GroupItem *group = dynamic_cast<GroupItem*>(item);
			Q_ASSERT(group);

			return group->categories().count();

		}
		else if(item->level() == Category)
		{
			return 0;
		}
	}

	return mData.count();
}

QVariant WIMMModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();

	if(row < 0 || row >= rowCount())
	{
		return QVariant();
	}

	if(role == Qt::EditRole)
	{
		return editRole(index);
	}
	else if(role == Qt::DisplayRole)
	{
		return displayRole(index);
	}
	else if(role == Qt::DecorationRole)
	{
		return decorationRole(index);
	}
	else if(role == Qt::BackgroundColorRole)
	{
		return backgroundRole(index);
	}
	else if(role == Qt::FontRole)
	{
		return fontRole(index);
	}
	else if(role == Qt::TextAlignmentRole)
	{
		return alignmentRole(index);
	}
	else if(role == Qt::ForegroundRole)
	{
		return textColorRole(index);
	}
	else if(role == Qt::ToolTipRole)
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

	CategoryItem* category = dynamic_cast<CategoryItem*>(item);
	Q_ASSERT(category);

	int col = index.column();

	bool changed = false;

	if(col == COL_FirstHalfIn)
	{
		changed = true;
		category->setFirstHalfIncome( value.toDouble() );
	}
	else if(col == COL_FirstHalfOut)
	{
		changed = true;
		category->setFirstHalfOutcome( value.toDouble() );
	}
	else if(col == COL_FirstHalfEst)
	{
		changed = true;
		category->setFirstHalfEstimated( value.toDouble() );
	}
	else if(col == COL_SecondHalfIn)
	{
		changed = true;
		category->setSecondHalfIncome( value.toDouble() );
	}
	else if(col == COL_SecondHalfOut)
	{
		changed = true;
		category->setSecondHalfOutcome( value.toDouble() );
	}
	else if(col == COL_SecondHalfEst)
	{
		changed = true;
		category->setSecondHalfEstimated( value.toDouble() );
	}

	if(changed)
	{
		category->save();
		QVector<int> roles;
		roles << role;
		emit dataChanged(index, index, roles);
	}

	return true;
}

Qt::ItemFlags WIMMModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	WIMMItem *item = itemForIndex(index);
	if(!item)
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
		else if(section == COL_Title)
		{
			return "Категория";
		}
		else if(section == COL_FirstHalfIn)
		{
			return QString("%0 (I)").arg(QChar(0x002b));
		}
		else if(section == COL_FirstHalfOut)
		{
			return QString("%0 (I)").arg(QChar(0x2212));
		}
		else if(section == COL_FirstHalfEst)
		{
			return "? (I)";
		}
		else if(section == COL_SecondHalfIn)
		{
			return QString("%0 (II)").arg(QChar(0x002b));
		}
		else if(section == COL_SecondHalfOut)
		{
			return QString("%0 (II)").arg(QChar(0x2212));
		}
		else if(section == COL_SecondHalfEst)
		{
			return "? (II)";
		}
	}
	else if(role == Qt::FontRole)
	{
		QFont f = qApp->font();
		f.setBold(true);
		return f;
	}
	else if(role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
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
	else if(col == COL_Title)
	{
		return item->name();
	}
	else if(col == COL_FirstHalfIn )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstIn));
	}
	else if(col == COL_FirstHalfOut )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstOut));
	}
	else if(col == COL_FirstHalfEst )
	{
		return Tools::moneyString( item ->value(WIMMItem::FirstEst));
	}
	else if(col == COL_SecondHalfIn )
	{
		return Tools::moneyString( item ->value(WIMMItem::SecondIn));
	}
	else if(col == COL_SecondHalfOut )
	{
		return Tools::moneyString( item ->value(WIMMItem::SecondOut));
	}
	else if(col == COL_SecondHalfEst)
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
	else if(col == COL_Title)
	{
		return item->name();
	}
	else if(col == COL_FirstHalfIn)
	{
		return item->value(WIMMItem::FirstIn);
	}
	else if(col == COL_FirstHalfOut)
	{
		return item->value(WIMMItem::FirstOut);
	}
	else if(col == COL_FirstHalfEst)
	{
		return item->value(WIMMItem::FirstEst);
	}
	else if(col == COL_SecondHalfIn)
	{
		return item->value(WIMMItem::SecondIn);
	}
	else if(col == COL_SecondHalfOut)
	{
		return item->value(WIMMItem::SecondOut);
	}
	else if(col == COL_SecondHalfEst)
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
		CategoryItem *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		if(index.column() == COL_FirstHalfIn && !category->comment(WIMMItem::FirstIn).isEmpty())
		{
			return QIcon(":comment");
		}
		else if(index.column() == COL_FirstHalfOut && !category->comment(WIMMItem::FirstOut).isEmpty())
		{
			return QIcon(":comment");
		}
		else if(index.column() == COL_FirstHalfEst && !category->comment(WIMMItem::FirstEst).isEmpty())
		{
			return QIcon(":comment");
		}
		else if(index.column() == COL_SecondHalfIn && !category->comment(WIMMItem::SecondIn).isEmpty())
		{
			return QIcon(":comment");
		}
		else if(index.column() == COL_SecondHalfOut && !category->comment(WIMMItem::SecondOut).isEmpty())
		{
			return QIcon(":comment");
		}
		else if(index.column() == COL_SecondHalfEst && !category->comment(WIMMItem::SecondEst).isEmpty())
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
		CategoryItem *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		if(index.column() == COL_FirstHalfIn)
		{
			return category->comment(WIMMItem::FirstIn);
		}
		else if(index.column() == COL_FirstHalfOut)
		{
			return category->comment(WIMMItem::FirstOut);
		}
		else if(index.column() == COL_FirstHalfEst)
		{
			return category->comment(WIMMItem::FirstEst);
		}
		else if(index.column() == COL_SecondHalfIn)
		{
			return category->comment(WIMMItem::SecondIn);
		}
		else if(index.column() == COL_SecondHalfOut)
		{
			return category->comment(WIMMItem::SecondOut);
		}
		else if(index.column() == COL_SecondHalfEst)
		{
			return category->comment(WIMMItem::SecondEst);
		}
	}

	return QVariant();
}

WIMMItem *WIMMModel::itemForIndex(const QModelIndex &index) const
{
	WIMMItem *item = static_cast<WIMMItem*>(index.internalPointer());
	return item;
}

QModelIndex WIMMModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(column);

	if(parent.isValid())
	{
		WIMMItem *item = itemForIndex(parent);
		if(item->level() == Month)
		{
			MonthItem *month = dynamic_cast<MonthItem*>(item);
			Q_ASSERT(month); /// wtf?

			if(row < 0 || row >= month->groups().count())
			{
				return QModelIndex();
			}

			return createIndex(row, column, month->groups().at(row));
		}
		else if(item->level() == Group)
		{
			GroupItem *group = dynamic_cast<GroupItem*>(item);
			Q_ASSERT(group); /// wtf?

			if(row < 0 || row >= group->categories().count())
			{
				return QModelIndex();
			}

			return createIndex(row, column, group->categories().at(row));
		}
		else
		{
			// Этого быть не должно
			return QModelIndex();
		}
	}

	if(row < 0 || row >= mData.count())
	{
		return QModelIndex();
	}

	return createIndex(row, column, mData[row]);
}

QModelIndex WIMMModel::parent(const QModelIndex &child) const
{
	WIMMItem *item = itemForIndex(child);

	if(!item)
	{
		return QModelIndex();
	}

	if(item->level() == Month)
	{
		return QModelIndex();
	}
	else if(item->level() == Group)
	{
		GroupItem *group = dynamic_cast<GroupItem*>(item);
		Q_ASSERT(group);

		int row = mData.indexOf(group->month());
		if(row < 0)
		{
			return QModelIndex();
		}

		return createIndex(row, 0, group->month());
	}
	else if(item->level() == Category)
	{
		CategoryItem *category = dynamic_cast<CategoryItem*>(item);
		Q_ASSERT(category);

		int row = category->group()->month()->groups().indexOf(category->group());
		if(row < 0)
		{
			return QModelIndex();
		}

		return createIndex(row, 0, category->group());
	}

	return QModelIndex();
}
