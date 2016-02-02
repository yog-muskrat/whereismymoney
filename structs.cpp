#include "structs.h"
#include "tools.h"

#include "sqltools.h"

/// MonthItem Class
QString MonthItem::monthName() const
{
	return Tools::capitalize( QDate::longMonthName(mMonth, QDate::StandaloneFormat) );
}

bool MonthItem::save()
{
	foreach(GroupItem *group, mGroups)
	{
		if(!group->save())
		{
			return false;
		}
	}

	return true;
}

QString MonthItem::name() const
{
	return QString("%1 %0").arg( monthName() ).arg(mYear);
}

double MonthItem::value(const WIMMItem::Column col) const
{
	double result = 0;
	foreach(GroupItem *item, mGroups)
	{
		result += item->value(col);
	}

	return result;
}

void MonthItem::addGroup(GroupItem *group)
{
	group->setMonth( this );
	mGroups << group;
}

/// End MonthItem Class


/// GroupItem Class
bool GroupItem::save()
{
	foreach(CategoryItem *category, mCategories)
	{
		if(!category->isDirty())
		{
			continue;
		}

		if(!category->save())
		{
			return false;
		}
	}

	return true;
}

double GroupItem::value(WIMMItem::Column col) const
{
	double result = 0;
	foreach(CategoryItem *item, mCategories)
	{
		result += item->value(col);
	}

	return result;
}

void GroupItem::addCategory(CategoryItem *category)
{
	category->setGroup( this );
	mCategories << category;
}

/// End GroupItem Class


/// CategoryItem Class
void CategoryItem::setValue(WIMMItem::Column col, double value)
{
	mValues[col] = value;
	setDirty(true);
}

void CategoryItem::setComment(WIMMItem::Column col, QString comment)
{
	mComments[col] = comment;
	setDirty(true);
}

bool CategoryItem::save()
{
	bool ok = SqlTools::saveMoneyRecord( this );
	setDirty(ok);
	return ok;
}
/// End CategoryItem Class
