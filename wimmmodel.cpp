#include "wimmmodel.h"
#include "sqltools.h"
#include "structs.h"

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

void WIMMModel::removeMonthById(int monthId)
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
	return QVariant();
}

bool WIMMModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role != Qt::EditRole && role != Qt::DisplayRole)
	{
		return false;
	}

	///TODO: Implement!

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
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
	{
		return QVariant();
	}

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
		return "+ (I)";
	}
	else if(section == COL_FirstHalfOut)
	{
		return "- (I)";
	}
	else if(section == COL_FirstHalfEst)
	{
		return "? (I)";
	}
	else if(section == COL_SecondHalfIn)
	{
		return "+ (II)";
	}
	else if(section == COL_SecondHalfOut)
	{
		return "- (II)";
	}
	else if(section == COL_SecondHalfEst)
	{
		return "? (II)";
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
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::FirstIn));
	}
	else if(col == COL_FirstHalfOut )
	{
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::FirstOut));
	}
	else if(col == COL_FirstHalfEst )
	{
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::FirstEst));
	}
	else if(col == COL_SecondHalfIn )
	{
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::SecondIn));
	}
	else if(col == COL_SecondHalfOut )
	{
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::SecondOut));
	}
	else if(col == COL_SecondHalfEst)
	{
		return QLocale::system().toCurrencyString( item ->value(WIMMItem::SecondEst));
	}

	return QVariant();
}

QVariant WIMMModel::editRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	if(item->level() != Category)
	{
		return QVariant();
	}

	int col = index.column();

	CategoryItem *category = dynamic_cast<CategoryItem*>(item);
	Q_ASSERT(category);

	if(col == COL_DbId)
	{
		return category->id();
	}
	else if(col == COL_Title)
	{
		return category->name();
	}
	else if(col == COL_FirstHalfIn)
	{
		return category->firstHalfIncome();
	}
	else if(col == COL_SecondHalfIn)
	{
		return category->secondHalfIncome();
	}
	else if(col == COL_SecondHalfEst)
	{
		return category->secondHalfEstimated();
	}
	else if(col == COL_FirstHalfOut)
	{
		return category->firstHalfOutcome();
	}
	else if(col == COL_SecondHalfOut)
	{
		return category->secondHalfOutcome();
	}
	else if(col == COL_SecondHalfEst)
	{
		return category->secondHalfEstimated();
	}

	return QVariant();
}

QVariant WIMMModel::decorationRole(const QModelIndex &index) const
{
	return QVariant();
}

QVariant WIMMModel::backgroundRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	int col = index.column();

	if(item->level() == Category)
	{
		if(col == COL_FirstHalfIn || col == COL_SecondHalfIn)
		{
			return QColor(230, 255, 230);
		}
		else if(col == COL_FirstHalfOut || col == COL_SecondHalfOut)
		{
			return QColor(255, 230, 230);
		}
		else if(col == COL_FirstHalfEst || col == COL_SecondHalfEst)
		{
			return QColor(240, 240, 255);
		}
	}
	/*else if(item->level() == Group)
	{
		return QColor(240, 240, 240);
	}
	else if(item->level() == Month)
	{
		return QColor(220, 220, 220);
	}*/

	return QVariant();
}

QVariant WIMMModel::fontRole(const QModelIndex &index) const
{
	WIMMItem *item = itemForIndex(index);
	Q_ASSERT(item);

	QFont f = QApplication::font();

	if(item->level() == Month)
	{
		f.setBold(true);
		f.setPointSize( f.pointSize() + 2 );
	}
	else if(item->level() == Group)
	{
		f.setPointSize( f.pointSize() + 1 );
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
