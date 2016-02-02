#ifndef WIMMMODEL
#define WIMMMODEL

#include "structs.h"

#include <QAbstractItemModel>

class QUndoStack;

class WIMMModel : public QAbstractItemModel
{
	Q_OBJECT

	friend class ChangeValueCommand;
public:
	WIMMModel(QObject *parent = 0);
	~WIMMModel();

	enum Column {
		COL_Title,
		COL_DbId,
		COL_FirstHalfIn,
		COL_FirstHalfOut,
		COL_FirstHalfEst,
		COL_SecondHalfIn,
		COL_SecondHalfOut,
		COL_SecondHalfEst,
		COL_Count
	};

	void addMonths( QList<MonthItem*> items);
	void addMonth( MonthItem* item, bool updateEstFromPrevious = false);
	void removeMonth(int monthId);
	QModelIndex monthIndex(int monthId) const;
	QModelIndex monthIndex(int year, int month) const;
	int monthId(const QModelIndex &index) const;

	bool isMoneyIndex(const QModelIndex &index) const;

	QString comment(const QModelIndex &index) const;
	void setComment(const QModelIndex &index, QString comment);

	ItemLevel indexLevel(const QModelIndex &index) const;
	int categoryId(const QModelIndex &index) const;

	void clear();

	bool save();

	QUndoStack *undoStack(){return pUndoStack;}

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

private:
	QVariant displayRole(const QModelIndex &index) const;
	QVariant editRole(const QModelIndex &index) const;
	QVariant decorationRole(const QModelIndex &index) const;
	QVariant backgroundRole(const QModelIndex &index) const;
	QVariant fontRole(const QModelIndex &index) const;
	QVariant textColorRole(const QModelIndex &index) const;
	QVariant alignmentRole(const QModelIndex &index) const;
	QVariant tooltipRole(const QModelIndex &index) const;

	WIMMItem* itemForIndex(const QModelIndex &index) const;

	void emitDataChanged(const QModelIndex &index);

	QList<MonthItem*> mData;
	QUndoStack *pUndoStack;

};

#endif // WIMMMODEL
