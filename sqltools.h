#ifndef SQLTOOLS
#define SQLTOOLS

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>

class WIMMModel;
class MonthItem;
class GroupItem;
class CategoryItem;

/*!
 * \brief Класс для загрузки данных из БД
 */
class SqlTools
{
public:
	static WIMMModel* loadModel();

	static bool monthRecordExists(int year, int month);
	static MonthItem *addMonthRecord(int year, int month);
	static bool deleteMonthRecord(int id);

	static bool saveMoneyRecord(CategoryItem *md);
	static int addCategoryRecord(QString name);

	static bool openDb();
	static void closeDb();

	static QSqlQuery emptyQuery();
	static bool execQuery(QSqlQuery &query);

private:
	static QSqlDatabase db();
	static QList<MonthItem*> loadMonths();
	static bool loadGroups(MonthItem *month);
	static bool loadCategories(GroupItem *group);


	static const QString mConnectionName;
};

#endif // SQLTOOLS
