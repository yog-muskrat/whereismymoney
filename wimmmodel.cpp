#include "wimmmodel.h"

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MoneyModel::MoneyModel(QObject *parent)
#ifdef QMLVERSION
	: QAbstractListModel(parent)
#else
	: QAbstractTableModel(parent)
#endif
{
	mDirty = false;
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


QVariant MoneyModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();

	if(row < 0 || row >= rowCount())
	{
		return QVariant();
	}

#ifdef QMLVERSION

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

#else

	if(role == Qt::EditRole)
	{
		return editRole(index);
	}
	else if(role == Qt::DisplayRole)
	{
		return displayRole(index);
	}

#endif

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

QVariant MoneyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
#ifdef QMLVERSION
	return QAbstractListModel::headerData(section, orientation, role);
#else

	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
	{
		return QVariant();
	}

	if(section == COL_RecordId)
	{
		return "БД";
	}
	else if(section == COL_Category)
	{
		return "Категория";
	}
	else if(section == COL_FirstHalfIn)
	{
		return "+ (I)";
	}
	else if(section == COL_FirstHalfOut)
	{
		return "- (I)";
	}
	else if(section == COL_SecondHalfIn)
	{
		return "+ (II)";
	}
	else if(section == COL_SecondHalfOut)
	{
		return "- (II)";
	}

	return QAbstractTableModel::headerData(section, orientation, role);
#endif
}

QVariant MoneyModel::displayRole(const QModelIndex &index) const
{
	int col = index.column();
	const MoneyData &md = mData[index.row()];

	if(col == COL_RecordId)
	{
		return md.recordId;
	}
	else if(col == COL_Category)
	{
		QString result = md.categoryName;
		if(md.recordId < 0)
		{
			result += " *";
		}

		return result;
	}
	else if(col == COL_FirstHalfIn)
	{
		return md.firstHalfIncome;
	}
	else if(col == COL_SecondHalfIn)
	{
		return md.secondHalfIncome;
	}
	else if(col == COL_FirstHalfOut)
	{
		return md.firstHalfOutcome;
	}
	else if(col == COL_SecondHalfOut)
	{
		return md.secondHalfOutcome;
	}

	return QVariant();
}

QVariant MoneyModel::editRole(const QModelIndex &index) const
{
	int col = index.column();
	const MoneyData &md = mData[index.row()];

	if(col == COL_RecordId)
	{
		return md.recordId;
	}
	else if(col == COL_Category)
	{
		return md.categoryId;
	}
	else if(col == COL_FirstHalfIn)
	{
		return md.firstHalfIncome;
	}
	else if(col == COL_SecondHalfIn)
	{
		return md.secondHalfIncome;
	}
	else if(col == COL_FirstHalfOut)
	{
		return md.firstHalfOutcome;
	}
	else if(col == COL_SecondHalfOut)
	{
		return md.secondHalfOutcome;
	}

	return QVariant();
}

bool MoneyModel::submit()
{
	return true;
}

void MoneyModel::revert()
{
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



void WIMMObject::setId(int id)
{
	if(mId != id)
	{
		mId = id;
		emit idChanged();
	}
}

void WIMMObject::setYear(int year)
{
	if(mYear != year)
	{
		mYear = year;
		emit yearChanged();
	}
}

void WIMMObject::setMonth(int month)
{
	if(mMonth != month )
	{
		mMonth = month;
		emit monthChanged();
	}
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

WIMMModel *DataLoader::loadModel()
{
	return new WIMMModel();
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


WIMMModel::WIMMModel(QObject *parent) : QAbstractListModel(parent)
{
	mObjects = DataLoader::loadData();
	mIsDirty = false;
}

WIMMModel::~WIMMModel()
{
	qDeleteAll(mObjects);
}

WIMMObject *WIMMModel::objectAt(const QModelIndex &index)
{
	return qobject_cast<WIMMObject*>(mObjects[index.row()]);
}

bool WIMMModel::appendObject(WIMMObject *obj)
{
	if(mObjects.contains(obj))
	{
		return false;
	}

	int row = rowCount();

	beginInsertRows(QModelIndex(), row, row);
	mObjects << obj;
	mIsDirty = true;
	endInsertRows();

	return true;
}

int WIMMModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mObjects.count();
}

QVariant WIMMModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(index.row() >=0 && index.row() <= rowCount());

	if(role == Qt::DisplayRole)
	{
		WIMMObject *obj = qobject_cast<WIMMObject*>( mObjects[index.row()] );
		Q_ASSERT(obj);

		QString string = QString("%0 %1").arg(obj->year()).arg(QDate::longMonthName(obj->month(), QDate::StandaloneFormat));
		if(obj->id() < 0 || obj->moneyModel()->isDirty())
		{
			string += " *";
		}
		return string;
	}

	return QVariant();
}



bool WIMMModel::removeRows(int row, int count, const QModelIndex &parent)
{
	beginRemoveRows(parent, row, row + count -1 );

	for(int i = 0; i < count; ++i)
	{
		delete mObjects.takeAt(row);
	}

	endRemoveRows();

	return true;
}


bool WIMMModel::submit()
{
	return true;
}

void WIMMModel::revert()
{
}
