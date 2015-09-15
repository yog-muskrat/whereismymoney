#include "dbmanager.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>

DBManager::DBManager(QObject *parent) : QObject(parent)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("wimm.db");
	if(!db.open())
	{
		qDebug()<<"Database not open. Error "<<db.lastError().text();
		return;
	}
}

QAbstractItemModel *DBManager::categoriesModel()
{
	QSqlTableModel *model = new QSqlTableModel(this, QSqlDatabase::database());
	model->setTable("categories");
	model->setSort( model->fieldIndex("name"), Qt::AscendingOrder);
	model->select();

	qDebug()<<model->rowCount();

	return model;
}

QAbstractItemModel *DBManager::dataModel()
{
	return 0;
}

