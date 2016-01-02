#ifndef WIMMMODEL_H
#define WIMMMODEL_H

#include <QtQml>
#include <QObject>
#include <QAbstractListModel>
#include <QAbstractTableModel>

struct MoneyData
{
	int recordId;
	QString categoryName;
	int categoryId;
	double firstHalfIncome;
	double secondHalfIncome;
	double firstHalfOutcome;
	double secondHalfOutcome;
};


/*!
 * \brief Класс модели данных с цифрами по одной категории затрат за один месяц
 */
#ifdef QMLVERSION
class MoneyModel : public QAbstractListModel
#else
class MoneyModel : public QAbstractTableModel
#endif
{
	Q_OBJECT
public:
	enum DataRoles
	{
		RecordIdRole = Qt::UserRole + 1,
		CategoryIdRole,
		CategoryNameRole,
		FirstHalfIncomeRole,
		FirstHalfOutcomeRole,
		SecondHalfIncomeRole,
		SecondHalfOutcomeRole
	};

	MoneyModel(QObject *parent = 0);
	~MoneyModel();

	void addMoneyData(MoneyData md);

#ifndef QMLVERSION
	enum Columns {
		COL_RecordId,
		COL_Category,
		COL_FirstHalfIn,
		COL_FirstHalfOut,
		COL_SecondHalfIn,
		COL_SecondHalfOut,
		COL_Count
	};

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {return COL_Count;}
#endif
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const { return mData.count(); }
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	bool isDirty() const {return mDirty;}

public slots:
	virtual bool submit();
	virtual void revert();

private:
	QVariant displayRole(const QModelIndex &index) const;
	QVariant editRole(const QModelIndex &index) const;

	QList<MoneyData> mData;
	bool mDirty;

};

/*!
 * \brief Класс объекта, содержащего данные по всем статьям затрат одного месяца
 */
class WIMMObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(int year READ year WRITE setYear NOTIFY yearChanged)
	Q_PROPERTY(int month READ month WRITE setMonth NOTIFY monthChanged)
#ifdef QMLVERSION
	Q_PROPERTY(QAbstractListModel * moneyModel READ moneyModel NOTIFY moneyModelChanged)
#else
	Q_PROPERTY(QAbstractTableModel * moneyModel READ moneyModel NOTIFY moneyModelChanged)
#endif
public:
	WIMMObject(QObject *parent = 0);
	~WIMMObject();

	int id() const { return mId; }
	void setId(int id);

	int year() const { return mYear; }
	void setYear(int year);

	int month() const { return mMonth; }
	void setMonth(int month);

	MoneyModel* moneyModel() { return mMoneyModel; }
	void addMoneyData(MoneyData md);

signals:
	void idChanged();
	void yearChanged();
	void monthChanged();
	void moneyModelChanged();

private:
	int mId;
	int mYear;
	int mMonth;
	MoneyModel *mMoneyModel;
};

class WIMMModel : public QAbstractListModel
{
	Q_OBJECT
public:
	WIMMModel(QObject *parent = 0);
	~WIMMModel();

	WIMMObject * objectAt(const QModelIndex &index);

	bool appendObject(WIMMObject *obj);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool removeRows(int row, int count, const QModelIndex &parent);
	virtual bool submit();
	virtual void revert();
	bool isDirty() const {return mIsDirty;}

private:
	QList<QObject*> mObjects;
	bool mIsDirty;

};

/*!
 * \brief Класс для загрузки данных из БД
 */
class DataLoader
{
public:
	static QList<QObject*> loadData();
	static WIMMModel* loadModel();
private:
	static bool loadMoneyData(WIMMObject*obj);
};

#endif // WIMMMODEL_H
