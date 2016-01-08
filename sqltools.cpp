#include "sqltools.h"
#include "wimmmodel.h"
#include "structs.h"

#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

const QString SqlTools::mConnectionName = "wimm";


WIMMModel *SqlTools::loadModel()
{
	WIMMModel *model = new WIMMModel();
	model->addMonths( loadMonths() );

	return model;
}

bool SqlTools::monthRecordExists(int year, int month)
{
	QSqlQuery query = emptyQuery();
	query.prepare("SELECT id FROM months WHERE year=:year AND month=:month");
	query.bindValue(":year", year);
	query.bindValue(":month", month);

	if(!execQuery(query))
	{
		return true;
	}

	return query.first();
}

MonthItem* SqlTools::addMonthRecord(int year, int month)
{
	QSqlQuery query = emptyQuery();
	query.prepare("INSERT INTO months (year, month) VALUES (:year, :month)");
	query.bindValue(":year", year);
	query.bindValue(":month", month);

	if(!execQuery(query))
	{
		return 0;
	}

	MonthItem *item = new MonthItem;
	item->setId(query.lastInsertId().toInt());
	item->setMonth(month);
	item->setYear(year);

	if(!loadGroups(item))
	{
		delete item;
		return 0;
	}

	return item;
}

bool SqlTools::deleteMonthRecord(int id)
{
	QSqlQuery query = emptyQuery();
	query.prepare("DELETE FROM months WHERE id=:id");
	query.bindValue(":id", id);

	return execQuery(query);
}

bool SqlTools::saveMoneyRecord(CategoryItem *md)
{
	QSqlQuery query = emptyQuery();
	if(md->id() < 0)
	{
		query.prepare("INSERT INTO money "
					  "(category_id, "
					  "first_half_income, "
					  "first_half_outcome, "
					  "first_half_est, "
					  "second_half_income, "
					  "second_half_outcome, "
					  "second_half_est) "
					  "VALUES (:category, "
					  ":first_in, "
					  ":first_out, "
					  ":first_est, "
					  ":second_in, "
					  ":second_out, "
					  ":second_est)");
		query.bindValue(":category_id", md->categoryId());
	}
	else
	{
		query.prepare("UPDATE money "
					  "SET first_half_income=:first_in, "
					  "first_half_outcome=:first_out, "
					  "first_half_est=:first_est, "
					  "second_half_income=:second_in, "
					  "second_half_outcome=:second_out, "
					  "second_half_est=:second_est "
					  "WHERE id=:record_id");
		query.bindValue(":record_id", md->id());
	}

	query.bindValue(":first_in", md->firstHalfIncome());
	query.bindValue(":first_out", md->firstHalfOutcome());
	query.bindValue(":first_est", md->firstHalfEstimated());
	query.bindValue(":second_in", md->secondHalfIncome());
	query.bindValue(":second_out", md->secondHalfOutcome());
	query.bindValue(":second_est", md->secondHalfEstimated());

	bool ok = execQuery(query);

	if(ok && md->id()< 0)
	{
		md->setId( query.lastInsertId().toInt() );
	}

	return ok;
}

int SqlTools::addCategoryRecord(QString name)
{
	QSqlQuery query = emptyQuery();
	query.prepare("INSERT INTO categories (name) VALUES (:name)");
	query.bindValue(":name", name);

	if(!execQuery(query))
	{
		return -1;
	}

	return query.lastInsertId().toInt(	);
}

bool SqlTools::openDb()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", mConnectionName);
	db.setDatabaseName("wimm.db");
	if(!db.open())
	{
		qDebug()<<"Database not open. Error "<<db.lastError().text();
		return false;
	}

	return true;
}

void SqlTools::closeDb()
{
	db().close();
	QSqlDatabase::removeDatabase(mConnectionName);
}

QSqlQuery SqlTools::emptyQuery()
{
	return QSqlQuery(db());
}

QSqlDatabase SqlTools::db()
{
	return QSqlDatabase::database(mConnectionName);
}

QList<MonthItem *> SqlTools::loadMonths()
{
	QList<MonthItem*> result;

	QSqlQuery query = emptyQuery();
	query.prepare("SELECT id, year, month FROM months ORDER BY year, month");
	if(!execQuery(query))
	{
		return result;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		MonthItem *item = new MonthItem;
		item->setId( rec.value("id").toInt() );
		item->setYear( rec.value("year").toInt() );
		item->setMonth( rec.value("month").toInt() );

		if(!loadGroups(item))
		{
			delete item;
			return result;
		}

		result << item;
	}

	return result;
}

bool SqlTools::loadGroups(MonthItem *month)
{
	QSqlQuery query = emptyQuery();
	query.prepare("SELECT id, name FROM category_groups ORDER BY pos");

	if(!execQuery(query))
	{
		return false;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();
		GroupItem *item = new GroupItem;
		item->setId( rec.value("id").toInt() );
		item->setName( rec.value("name").toString() );
		month->addGroup( item );

		if(!loadCategories(item))
		{
			return false;
		}

	}

	return true;
}

bool SqlTools::loadCategories(GroupItem *group)
{
	QSqlQuery query = emptyQuery();

	/// Убедиться, что в месяце есть данные для всех категорий.
	query.prepare("INSERT INTO money (category_id, month_id) "
				  "SELECT id, :month_id1 "
				  "FROM categories WHERE id NOT IN (SELECT category_id FROM money WHERE month_id=:month_id2)");
	query.bindValue(":month_id1", group->month()->id());
	query.bindValue(":month_id2", group->month()->id());
	if(!execQuery(query))
	{
		return false;
	}

	query.prepare("SELECT "
				  "m.id AS id, "
				  "m.category_id AS category_id, "
				  "c.name AS category_name, "
				  "m.first_half_income, "
				  "m.first_half_outcome, "
				  "m.first_half_est, "
				  "m.second_half_income, "
				  "m.second_half_outcome, "
				  "m.second_half_est "
				  "FROM money m JOIN categories c ON m.category_id=c.id "
				  "WHERE month_id=:month_id AND c.category_group=:group "
				  "ORDER BY c.pos");
	query.bindValue(":month_id", group->month()->id());
	query.bindValue(":group", group->id());

	if(!execQuery(query))
	{
		return false;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		CategoryItem *item = new CategoryItem;
		item->setId( rec.value("id").toInt() );
		item->setCategoryId( rec.value("category_id").toInt() );
		item->setName( rec.value("category_name").toString() );
		item->setFirstHalfIncome( rec.value("first_half_income").toDouble() );
		item->setFirstHalfOutcome( rec.value("first_half_outcome").toDouble() );
		item->setFirstHalfEstimated( rec.value("first_half_est").toDouble() );
		item->setSecondHalfIncome( rec.value("second_half_income").toDouble() );
		item->setSecondHalfOutcome( rec.value("second_half_outcome").toDouble() );
		item->setSecondHalfEstimated( rec.value("second_half_est").toDouble() );

		group->addCategory( item );
	}


	return true;
}

bool SqlTools::execQuery(QSqlQuery &query)
{
	if(!query.exec())
	{
		qDebug()<<"*** Ошибка выполнения запроса:\n"
				<<query.lastQuery()
				<<"\n"
				<<query.boundValues()
				<<"\n"
				<<query.lastError();
		return false;
	}

	return true;
}
