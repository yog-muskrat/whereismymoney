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

QList<GroupItem *> SqlTools::loadSummary()
{
	QList<GroupItem*> result;

	QSqlQuery query = emptyQuery();
	query.prepare("SELECT id, name FROM category_groups ORDER BY pos");

	if(!execQuery(query))
	{
		return result;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		GroupItem *group = new GroupItem;
		group->setId(rec.value("id").toInt());
		group->setName(rec.value("name").toString());

		QSqlQuery moneyQuery = emptyQuery();

		QString sql = "SELECT "
					  "sum(m.first_half_income ) AS first_in, "
					  "sum(m.second_half_income) AS second_in, "
					  "sum(m.first_half_outcome) AS first_out, "
					  "sum(m.second_half_outcome) AS second_out, "
					  "sum(m.first_half_est) AS first_est, "
					  "sum(m.second_half_est) AS second_est, "
					  "c.name AS category_name, "
					  "c.id AS category_id "
					  "FROM money m JOIN categories c ON m.category_id = c.id "
					  "WHERE category_group=:group "
					  "GROUP BY c.name, c.id ORDER BY c.pos";

		moneyQuery.prepare(sql);
		moneyQuery.bindValue(":group", group->id());

		if(!execQuery(moneyQuery))
		{
			qDeleteAll(result);
			result.clear();
			break;
		}

		while(moneyQuery.next())
		{
			QSqlRecord moneyRec = moneyQuery.record();

			CategoryItem *item = new CategoryItem;
			item->setValue(WIMMItem::FirstIn, moneyRec.value("first_in").toDouble() );
			item->setValue(WIMMItem::FirstOut, moneyRec.value("first_out").toDouble() );
			item->setValue(WIMMItem::FirstEst, moneyRec.value("first_est").toDouble() );
			item->setValue(WIMMItem::SecondIn, moneyRec.value("second_in").toDouble() );
			item->setValue(WIMMItem::SecondOut, moneyRec.value("second_out").toDouble() );
			item->setValue(WIMMItem::SecondEst, moneyRec.value("second_est").toDouble() );
			item->setCategoryId(moneyRec.value("category_id").toInt());
			item->setName(moneyRec.value("category_name").toString());

			group->addCategory( item );
		}

		result << group;
	}

	return result;
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
					  "month_id, "
					  "first_half_income, "
					  "first_half_outcome, "
					  "first_half_est, "
					  "second_half_income, "
					  "second_half_outcome, "
					  "second_half_est, "
					  "first_half_income_comment, "
					  "first_half_outcome_comment, "
					  "first_half_est_comment, "
					  "second_half_income_comment, "
					  "second_half_outcome_comment, "
					  "second_half_est_comment) "
					  "VALUES (:category, "
					  ":month_id, "
					  ":first_in, "
					  ":first_out, "
					  ":first_est, "
					  ":second_in, "
					  ":second_out, "
					  ":second_est, "
					  ":first_in_comment, "
					  ":first_out_comment, "
					  ":first_est_comment, "
					  ":second_in_comment, "
					  ":second_out_comment, "
					  ":second_est_comment)");
		query.bindValue(":category", md->categoryId());
		query.bindValue(":month_id", md->group()->month()->id());
	}
	else
	{
		query.prepare("UPDATE money "
					  "SET first_half_income=:first_in, "
					  "first_half_outcome=:first_out, "
					  "first_half_est=:first_est, "
					  "second_half_income=:second_in, "
					  "second_half_outcome=:second_out, "
					  "second_half_est=:second_est, "
					  "first_half_income_comment=:first_in_comment, "
					  "first_half_outcome_comment=:first_out_comment, "
					  "first_half_est_comment=:first_est_comment,"
					  "second_half_income_comment=:second_in_comment, "
					  "second_half_outcome_comment=:second_out_comment, "
					  "second_half_est_comment=:second_est_comment "
					  "WHERE id=:record_id");
		query.bindValue(":record_id", md->id());
	}

	query.bindValue(":first_in", md->value(WIMMItem::FirstIn));
	query.bindValue(":first_out", md->value(WIMMItem::FirstOut));
	query.bindValue(":first_est", md->value(WIMMItem::FirstEst));
	query.bindValue(":second_in", md->value(WIMMItem::SecondIn));
	query.bindValue(":second_out", md->value(WIMMItem::SecondOut));
	query.bindValue(":second_est", md->value(WIMMItem::SecondEst));
	query.bindValue(":first_in_comment", md->comment(WIMMItem::FirstIn));
	query.bindValue(":first_out_comment", md->comment(WIMMItem::FirstOut));
	query.bindValue(":first_est_comment", md->comment(WIMMItem::FirstEst));
	query.bindValue(":second_in_comment", md->comment(WIMMItem::SecondIn));
	query.bindValue(":second_out_comment", md->comment(WIMMItem::SecondOut));
	query.bindValue(":second_est_comment", md->comment(WIMMItem::SecondEst));

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
				  "m.second_half_est, "
				  "m.first_half_income_comment, "
				  "m.first_half_outcome_comment, "
				  "m.first_half_est_comment, "
				  "m.second_half_income_comment, "
				  "m.second_half_outcome_comment, "
				  "m.second_half_est_comment "
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
		item->setValue(WIMMItem::FirstIn, rec.value("first_half_income").toDouble() );
		item->setValue(WIMMItem::FirstOut, rec.value("first_half_outcome").toDouble() );
		item->setValue(WIMMItem::FirstEst, rec.value("first_half_est").toDouble() );
		item->setValue(WIMMItem::SecondIn, rec.value("second_half_income").toDouble() );
		item->setValue(WIMMItem::SecondOut, rec.value("second_half_outcome").toDouble() );
		item->setValue(WIMMItem::SecondEst, rec.value("second_half_est").toDouble() );
		item->setComment(WIMMItem::FirstIn, rec.value("first_half_income_comment").toString() );
		item->setComment(WIMMItem::FirstOut, rec.value("first_half_outcome_comment").toString() );
		item->setComment(WIMMItem::FirstEst, rec.value("first_half_est_comment").toString() );
		item->setComment(WIMMItem::SecondIn, rec.value("second_half_income_comment").toString() );
		item->setComment(WIMMItem::SecondOut, rec.value("second_half_outcome_comment").toString() );
		item->setComment(WIMMItem::SecondEst, rec.value("second_half_est_comment").toString() );

		item->setDirty(false);

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
