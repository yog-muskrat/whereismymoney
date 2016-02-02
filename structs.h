#ifndef STRUCTS
#define STRUCTS

#include <QDate>
#include <QMap>
#include <QString>

class MonthItem;
class GroupItem;
class CategoryItem;

enum ItemLevel {Month, Group, Category};

class WIMMItem
{
public:
	enum Column {FirstIn, FirstOut, FirstEst, SecondIn, SecondOut, SecondEst};

	WIMMItem() : mDirty(false) {}
	virtual ~WIMMItem(){}

	virtual ItemLevel level() const = 0;
	virtual int id() const = 0;
	virtual QString name() const = 0;
	virtual double value(Column col) const = 0;
	virtual bool save() = 0;

	bool isDirty() const { return mDirty; }
	void setDirty(bool dirty) { mDirty = dirty; }

private:
	bool mDirty;
};

class MonthItem : public WIMMItem
{
public:
	MonthItem() : WIMMItem(), mId(-1), mYear(-1), mMonth(-1) {}
	~MonthItem() { qDeleteAll(mGroups); }

	virtual bool save();

	virtual ItemLevel level() const { return Month; }

	void setId(int id) { mId = id; }
	virtual int id() const { return mId; }

	virtual QString name() const;
	virtual double value(const Column col) const;

	QString monthName() const;

	void setYear(int year) { mYear = year;}
	int year() const { return mYear; }

	void setMonth(int month) { mMonth = month; }
	int month() const { return mMonth; }

	QList<GroupItem *> groups() const { return mGroups; }
	void addGroup(GroupItem *group);

private:
	QList<GroupItem*> mGroups;
	int mId;
	int mYear;
	int mMonth;
};

class GroupItem : public WIMMItem
{
public:
	GroupItem() : WIMMItem(), mId(-1), pMonth(0) {}
	~GroupItem() { qDeleteAll(mCategories); }

	virtual bool save();

	virtual ItemLevel level() const { return Group; }
	virtual double value(Column col) const;

	void setId(int id) { mId = id; }
	virtual int id() const { return mId; }

	void setName(const QString &name) { mName = name; }
	virtual QString name() const { return mName; }

	void setMonth(MonthItem *value) { pMonth = value; }
	MonthItem *month() const { return pMonth; }

	QList<CategoryItem *> categories() const { return mCategories; }
	void addCategory(CategoryItem *category);

private:
	int mId;
	QString mName;
	MonthItem *pMonth;
	QList<CategoryItem *> mCategories;
};

class CategoryItem : public WIMMItem
{
public:

	CategoryItem() : WIMMItem(), mId(-1), mCategoryId(-1), pGroup(0) {}
	~CategoryItem() {}

	virtual ItemLevel level() const { return Category; }

	virtual int id() const { return mId; }
	void setId(int id) {mId = id;}

	void setName(const QString &name) { mName = name; }
	virtual QString name() const { return mName; }

	void setValue(Column col, double value);
	virtual double value(Column col) const { return mValues[col]; }

	void setComment(Column col, QString comment);
	QString comment(Column col) const { return mComments[col]; }

	void setCategoryId(int categoryId) { mCategoryId = categoryId; }
	int categoryId() const { return mCategoryId; }

	void setGroup(GroupItem *value) { pGroup = value; }
	GroupItem *group() const { return pGroup; }

	virtual bool save();

private:
	int mId;
	QString mName;
	int mCategoryId;
	GroupItem *pGroup;
	QMap<Column, QString> mComments;
	QMap<Column, double> mValues;
};

#endif // STRUCTS

