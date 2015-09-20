#include "wimmmodel.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MoneyModel::MoneyModel(QObject *parent) : QAbstractListModel(parent)
{

}

MoneyModel::~MoneyModel()
{

}

void MoneyModel::addMoneyData(MoneyData md)
{
	beginInsertRows(QModelIndex(), mData.count(), mData.count());
	mData<<md;
	endInsertRows();
}

int MoneyModel::rowCount(const QModelIndex &parent) const
{
	return mData.count();
}

QVariant MoneyModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();

	if(row < 0 || row >= rowCount())
	{
		return QVariant();
	}

	const MoneyData &md = mData[row];

	if(role == Qt::DisplayRole)
	{
		return md.categoryName;
	}
	else if(role == RecordIdRole)
	{
		return md.recordId;
	}
	else if(role == CategoryIdRole)
	{
		return md.categoryId;
	}
	else if(role == CategoryNameRole)
	{
		return md.categoryName;
	}
	else if(role == FirstHalfIncomeRole)
	{
		return md.firstHalfIncome;
	}
	else if(role == FirstHalfOutcomeRole)
	{
		return md.firstHalfOutcome;
	}
	else if(role == SecondHalfIncomeRole)
	{
		return md.secondHalfIncome;
	}
	else if(role == SecondHalfOutcomeRole)
	{
		return md.secondHalfOutcome;
	}

	return QVariant();
}

QHash<int, QByteArray> MoneyModel::roleNames() const
{
	QHash<int, QByteArray> result;
	result[ RecordIdRole] = "recordId";
	result[ CategoryIdRole] = "categoryId";
	result[ CategoryNameRole] = "categoryName";
	result[ FirstHalfIncomeRole] = "firstHalfIncome";
	result[ FirstHalfOutcomeRole] = "firstHalfOutcome";
	result[ SecondHalfIncomeRole] = "secondHalfIncome";
	result[ SecondHalfOutcomeRole] = "secondHalfOutcome";

	return result;
}

///****

WIMMObject::WIMMObject(QObject *parent) : QObject(parent), mId(-1), mYear(-1), mMonth(-1)
{
	mMoneyModel = new MoneyModel(this);

	connect(mMoneyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SIGNAL(moneyModelChanged()));
}

WIMMObject::~WIMMObject()
{
	mMoneyModel->deleteLater();
}

int WIMMObject::id() const
{
	return mId;
}

void WIMMObject::setId(int id)
{
	if(mId != id)
	{
		mId = id;
		emit idChanged();
	}
}

int WIMMObject::year() const
{
	return mYear;
}

void WIMMObject::setYear(int year)
{
	if(mYear != year)
	{
		mYear = year;
		emit yearChanged();
	}
}

int WIMMObject::month() const
{
	return mMonth;
}

void WIMMObject::setMonth(int month)
{
	if(mMonth != month )
	{
		mMonth = month;
		emit monthChanged();
	}
}

MoneyModel * WIMMObject::moneyModel()
{
	return mMoneyModel;
}

void WIMMObject::addMoneyData(MoneyData md)
{
	Q_ASSERT(mMoneyModel);
	mMoneyModel->addMoneyData( md );
	emit moneyModelChanged();
}

///****

QList<QObject *> DataLoader::loadData()
{
	QList<QObject*> objs;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "wimm");
	db.setDatabaseName("wimm.db");
	if(!db.open())
	{
		qDebug()<<"Database not open. Error "<<db.lastError().text();
		return objs;
	}

	QSqlQuery query(db);
	query.prepare("SELECT id, year, month FROM months ORDER BY year, month");

	if(!query.exec())
	{
		qDebug()<<"Ошибка выполнения запроса"<<query.lastError();
		return objs;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		WIMMObject *obj = new WIMMObject();
		obj->setId( rec.value("id").toInt() );
		obj->setYear( rec.value("year").toInt() );
		obj->setMonth( rec.value("month").toInt() );

		if(!loadMoneyData(obj))
		{
			continue;
		}

		objs << obj;
	}

	db.close();

	return objs;
}


bool DataLoader::loadMoneyData(WIMMObject *obj)
{
	Q_ASSERT(obj);

	QSqlQuery query( QSqlDatabase::database("wimm"));
	query.prepare("SELECT m.id AS id, m.category_id AS category_id, c.name AS category_name, "
				  "m.first_half_income, m.first_half_outcome, m.second_half_income, m.second_half_outcome "
				  "FROM money m JOIN categories c ON m.category_id=c.id "
				  "WHERE month_id=:month_id");
	query.bindValue(":month_id", obj->id());

	if(!query.exec())
	{
		qDebug()<<"Ошибка выборки данных по деньгам"<<query.lastError();
		return false;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		MoneyData md;
		md.recordId = rec.value("id").toInt();
		md.categoryId = rec.value("category_id").toInt();
		md.categoryName = rec.value("category_name").toString();
		md.firstHalfIncome = rec.value("first_half_income").toDouble();
		md.firstHalfOutcome = rec.value("first_half_outcome").toDouble();
		md.secondHalfIncome = rec.value("second_half_income").toDouble();
		md.secondHalfOutcome = rec.value("second_half_outcome").toDouble();

		obj->addMoneyData(md);
	}

	return true;
}
