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
	static QList<GroupItem*> loadSummary();

	static bool monthRecordExists(int year, int month);
	static MonthItem *addMonthRecord(int year, int month);
	static bool deleteMonthRecord(int id);

	static bool saveMoneyRecord(CategoryItem *md);
	static int addCategoryRecord(QString name);

	static bool openDb();
	static void closeDb();

	static bool transaction();
	static bool commit();
	static bool rollback();

	static QSqlQuery emptyQuery();
	static bool execQuery(QSqlQuery &query);

	static QList<MonthItem*> loadMonths();
	static bool loadGroups(MonthItem *month);
	static bool loadCategories(GroupItem *group);

	static QList<QPair<int, QString> > groupsList();
	static QList<QPair<int, QString> > groupCategories(int groupId);
private:
	static QSqlDatabase db();


	static const QString mConnectionName;
};

#endif // SQLTOOLS
