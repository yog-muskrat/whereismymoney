#include "structs.h"
#include "tools.h"

/// MonthItem Class

MonthItem::MonthItem() : WIMMItem(), mId(-1), mYear(-1), mMonth(-1)
{
}

MonthItem::~MonthItem()
{
	qDeleteAll(mGroups);
}

ItemLevel MonthItem::level() const
{
	return Month;
}

int MonthItem::id() const
{
	return mId;
}

QString MonthItem::monthName() const
{
	return Tools::capitalize( QDate::longMonthName(mMonth, QDate::StandaloneFormat) );
}

QString MonthItem::name() const
{
	return QString("%0 %1").arg( monthName() ).arg(mYear);
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

void MonthItem::setId(int id)
{
	mId = id;
}

int MonthItem::year() const
{
	return mYear;
}

void MonthItem::setYear(int year)
{
	mYear = year;
}

int MonthItem::month() const
{
	return mMonth;
}

void MonthItem::setMonth(int month)
{
	mMonth = month;
}

QList<GroupItem *> MonthItem::groups() const
{
	return mGroups;
}

void MonthItem::addGroup(GroupItem *group)
{
	group->setMonth( this );
	mGroups << group;
}

/// End MonthItem Class


/// GroupItem Class

GroupItem::GroupItem() : WIMMItem(), mId(-1), pMonth(0)
{
}

GroupItem::~GroupItem()
{
	qDeleteAll(mCategories);
}

ItemLevel GroupItem::level() const
{
	return Group;
}

int GroupItem::id() const
{
	return mId;
}

QString GroupItem::name()const
{
	return mName;
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

void GroupItem::setId(int id)
{
	mId = id;
}

void GroupItem::setName(const QString &name)
{
	mName = name;
}

MonthItem *GroupItem::month() const
{
	return pMonth;
}

void GroupItem::setMonth(MonthItem *value)
{
	pMonth = value;
}

QList<CategoryItem *> GroupItem::categories() const
{
	return mCategories;
}

void GroupItem::addCategory(CategoryItem *category)
{
	category->setGroup( this );
	mCategories << category;
}

/// End GroupItem Class


/// CategoryItem Class
CategoryItem::CategoryItem() : WIMMItem(),
	mId(-1),
	mCategoryId(-1),
	mFirstHalfIncome(0),
	mFirstHalfOutcome(0),
	mFirstHalfEstimated(0),
	mSecondHalfIncome(0),
	mSecondHalfOutcome(0),
	mSecondHalfEstimated(0),
	pGroup(0)
{

}

CategoryItem::~CategoryItem()
{

}

ItemLevel CategoryItem::level() const
{
	return Category;
}

int CategoryItem::id() const
{
	return mId;
}

QString CategoryItem::name() const
{
	return mName;
}

double CategoryItem::value(WIMMItem::Column col) const
{
	switch(col)
	{
	case WIMMItem::FirstIn: return firstHalfIncome();
	case WIMMItem::FirstOut: return firstHalfOutcome();
	case WIMMItem::FirstEst: return firstHalfEstimated();
	case WIMMItem::SecondIn: return secondHalfIncome();
	case WIMMItem::SecondOut: return secondHalfOutcome();
	case WIMMItem::SecondEst: return secondHalfEstimated();
	default: break;
	}

	return 0;
}

void CategoryItem::setId(int id)
{
	mId = id;
}

void CategoryItem::setName(const QString &name)
{
	mName = name;
}

int CategoryItem::categoryId() const
{
	return mCategoryId;
}

void CategoryItem::setCategoryId(int categoryId)
{
	mCategoryId = categoryId;
}

double CategoryItem::firstHalfIncome() const
{
	return mFirstHalfIncome;
}

void CategoryItem::setFirstHalfIncome(double firstHalfIncome)
{
	mFirstHalfIncome = firstHalfIncome;
}

double CategoryItem::firstHalfOutcome() const
{
	return mFirstHalfOutcome;
}

void CategoryItem::setFirstHalfOutcome(double firstHalfOutcome)
{
	mFirstHalfOutcome = firstHalfOutcome;
}

double CategoryItem::firstHalfEstimated() const
{
	return mFirstHalfEstimated;
}

void CategoryItem::setFirstHalfEstimated(double firstHalfEstimated)
{
	mFirstHalfEstimated = firstHalfEstimated;
}

double CategoryItem::secondHalfIncome() const
{
	return mSecondHalfIncome;
}

void CategoryItem::setSecondHalfIncome(double secondHalfIncome)
{
	mSecondHalfIncome = secondHalfIncome;
}

double CategoryItem::secondHalfOutcome() const
{
	return mSecondHalfOutcome;
}

void CategoryItem::setSecondHalfOutcome(double secondHalfOutcome)
{
	mSecondHalfOutcome = secondHalfOutcome;
}

double CategoryItem::secondHalfEstimated() const
{
	return mSecondHalfEstimated;
}

void CategoryItem::setSecondHalfEstimated(double secondHalfEstimated)
{
	mSecondHalfEstimated = secondHalfEstimated;
}

GroupItem *CategoryItem::group() const
{
	return pGroup;
}

void CategoryItem::setGroup(GroupItem *value)
{
	pGroup = value;
}
/// End CategoryItem Class
