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

	WIMMItem() = default;
	virtual ~WIMMItem()= default;

	virtual ItemLevel level() const = 0;
	virtual int id() const = 0;
	virtual QString name() const = 0;
	virtual double value(Column col) const = 0;
	virtual bool save() = 0;

	bool isDirty() const { return mDirty; }
	void setDirty(bool dirty) { mDirty = dirty; }

private:
	bool mDirty{};
};

class MonthItem : public WIMMItem
{
public:
	MonthItem() =default;
	~MonthItem() override { qDeleteAll(mGroups); }

	bool save() override;

	ItemLevel level() const override { return Month; }

	void setId(int id) { mId = id; }
	int id() const override { return mId; }

	QString name() const override;
	double value(Column col) const override;

	QString monthName() const;

	void setYear(int year) { mYear = year;}
	int year() const { return mYear; }

	void setMonth(int month) { mMonth = month; }
	int month() const { return mMonth; }

	QList<GroupItem *> groups() const { return mGroups; }
	void addGroup(GroupItem *group);

private:
	QList<GroupItem*> mGroups;
	int mId{-1};
	int mYear{-1};
	int mMonth{-1};
};

class GroupItem : public WIMMItem
{
public:
	GroupItem() = default;
	~GroupItem() override { qDeleteAll(mCategories); }

	bool save() override;

	ItemLevel level() const override { return Group; }
	double value(Column col) const override;

	void setId(int id) { mId = id; }
	int id() const override { return mId; }

	void setName(const QString &name) { mName = name; }
	QString name() const override { return mName; }

	void setMonth(MonthItem *value) { pMonth = value; }
	MonthItem *month() const { return pMonth; }

	QList<CategoryItem *> categories() const { return mCategories; }
	void addCategory(CategoryItem *category);

private:
	int mId{-1};
	QString mName;
	MonthItem *pMonth{nullptr};
	QList<CategoryItem *> mCategories;
};

class CategoryItem : public WIMMItem
{
public:

	CategoryItem() = default;
	~CategoryItem() override {}

	ItemLevel level() const override { return Category; }

	int id() const override { return mId; }
	void setId(int id) {mId = id;}

	void setName(const QString &name) { mName = name; }
	QString name() const override { return mName; }

	void setValue(Column col, double value);
	double value(Column col) const override { return mValues[col]; }

	void setComment(Column col, QString comment);
	QString comment(Column col) const { return mComments[col]; }

	void setCategoryId(int categoryId) { mCategoryId = categoryId; }
	int categoryId() const { return mCategoryId; }

	void setGroup(GroupItem *value) { pGroup = value; }
	GroupItem *group() const { return pGroup; }

	bool save() override;

private:
	int mId{-1};
	QString mName;
	int mCategoryId{-1};
	GroupItem *pGroup{nullptr};
	QMap<Column, QString> mComments;
	QMap<Column, double> mValues;
};

#endif // STRUCTS

