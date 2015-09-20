#ifndef WIMMMODEL_H
#define WIMMMODEL_H

#include <QtQml>
#include <QObject>
#include <QAbstractListModel>

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

class MoneyModel : public QAbstractListModel
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

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;

private:
	QList<MoneyData> mData;
};

class WIMMObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(int year READ year WRITE setYear NOTIFY yearChanged)
	Q_PROPERTY(int month READ month WRITE setMonth NOTIFY monthChanged)
	Q_PROPERTY(QAbstractListModel * moneyModel READ moneyModel NOTIFY moneyModelChanged)
public:
	WIMMObject(QObject *parent = 0);
	~WIMMObject();

	int id() const;
	void setId(int id);

	int year() const;
	void setYear(int year);

	int month() const;
	void setMonth(int month);

	MoneyModel* moneyModel();
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


class DataLoader
{
public:
	static QList<QObject*> loadData();
private:
	static bool loadMoneyData(WIMMObject*obj);
};

#endif // WIMMMODEL_H
