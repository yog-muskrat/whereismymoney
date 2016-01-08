#ifndef STRUCTS
#define STRUCTS

#include <QDate>
#include <QString>

class MonthItem;
class GroupItem;
class CategoryItem;

enum ItemLevel {Month, Group, Category};

class WIMMItem
{
public:
	enum Column {FirstIn, FirstOut, FirstEst, SecondIn, SecondOut, SecondEst};

	WIMMItem(){}
	virtual ~WIMMItem(){}

	virtual ItemLevel level() const = 0;
	virtual int id() const = 0;
	virtual QString name() const = 0;
	virtual double value(Column col) const = 0;
};

class MonthItem : public WIMMItem
{
public:
	MonthItem();
	~MonthItem();

	virtual ItemLevel level() const;
	virtual int id() const;
	virtual QString name() const;
	virtual double value(const Column col) const;

	QString monthName() const;

	void setId(int id);

	int year() const;
	void setYear(int year);

	int month() const;
	void setMonth(int month);

	QList<GroupItem *> groups() const;
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
	GroupItem();
	~GroupItem();

	virtual ItemLevel level() const;
	virtual int id() const;
	virtual QString name() const;
	virtual double value(Column col) const;

	void setId(int id);

	void setName(const QString &name);

	MonthItem *month() const;
	void setMonth(MonthItem *value);

	QList<CategoryItem *> categories() const;
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

	CategoryItem();
	~CategoryItem();

	virtual ItemLevel level() const;
	virtual int id() const;
	virtual QString name() const;
	virtual double value(Column col) const;

	QString comment(Column col) const;
	void setComment(Column col, QString comment);

	void setId(int id);

	void setName(const QString &name);

	int categoryId() const;
	void setCategoryId(int categoryId);

	double firstHalfIncome() const;
	void setFirstHalfIncome(double firstHalfIncome);

	double firstHalfOutcome() const;
	void setFirstHalfOutcome(double firstHalfOutcome);

	double firstHalfEstimated() const;
	void setFirstHalfEstimated(double firstHalfEstimated);

	double secondHalfIncome() const;
	void setSecondHalfIncome(double secondHalfIncome);

	double secondHalfOutcome() const;
	void setSecondHalfOutcome(double secondHalfOutcome);

	double secondHalfEstimated() const;
	void setSecondHalfEstimated(double secondHalfEstimated);

	GroupItem *group() const;
	void setGroup(GroupItem *value);

	bool save();

	QString firstHalfIncomeComment() const;
	void setFirstHalfIncomeComment(const QString &firstHalfIncomeComment);

	QString firstHalfOutcomeComment() const;
	void setFirstHalfOutcomeComment(const QString &firstHalfOutcomeComment);

	QString firstHalfEstimatedComment() const;
	void setFirstHalfEstimatedComment(const QString &firstHalfEstimatedComment);

	QString secondHalfIncomeComment() const;
	void setSecondHalfIncomeComment(const QString &secondHalfIncomeComment);

	QString secondHalfOutcomeComment() const;
	void setSecondHalfOutcomeComment(const QString &secondHalfOutcomeComment);

	QString secondHalfEstimatedComment() const;
	void setSecondHalfEstimatedComment(const QString &secondHalfEstimatedComment);

private:
	int mId;
	QString mName;
	int mCategoryId;
	double mFirstHalfIncome;
	double mFirstHalfOutcome;
	double mFirstHalfEstimated;
	double mSecondHalfIncome;
	double mSecondHalfOutcome;
	double mSecondHalfEstimated;
	QString mFirstHalfIncomeComment;
	QString mFirstHalfOutcomeComment;
	QString mFirstHalfEstimatedComment;
	QString mSecondHalfIncomeComment;
	QString mSecondHalfOutcomeComment;
	QString mSecondHalfEstimatedComment;
	GroupItem *pGroup;
};

#endif // STRUCTS

