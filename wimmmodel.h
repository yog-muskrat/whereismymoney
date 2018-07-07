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
	explicit WIMMModel(QObject *parent = nullptr);
	~WIMMModel() override;

	enum Column
	{
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

	void addMonths( const QList<MonthItem*>& items);
	void addMonth( MonthItem* item, bool updateEstFromPrevious = false);
	void removeMonth(int monthId);
	QModelIndex monthIndex(int monthId) const;
	QModelIndex monthIndex(int year, int month) const;
	int monthId(const QModelIndex &index) const;

	bool isMoneyIndex(const QModelIndex &index) const;

	QString comment(const QModelIndex &index) const;
	void setComment(const QModelIndex &index, const QString& comment);

	ItemLevel indexLevel(const QModelIndex &index) const;
	int categoryId(const QModelIndex &index) const;

	/*!
	 * \brief Возвращает суммы прихода/ухода денег по категориям
	 * В возвращаемом массиве ключ - код категории затрат, значение - пара значений,
	 * соответствующим суммарным приходу и уходу денег по данной категории.
	 */
	QMap<int, QPair<double, double> > totals() const;

	void clear();

	bool save();

	QUndoStack *undoStack(){return pUndoStack;}

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &child) const override;

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

	void emitDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

	void addToCache(QList<MonthItem *> months);
	void removeFromCache(QList<MonthItem *> months);
	quintptr cacheId(WIMMItem*item) const;

	quintptr nextCacheId();

	QList<MonthItem*> mData;
	QMap<quintptr, WIMMItem*> mCache;
	quintptr mLastId{0};
	QUndoStack *pUndoStack{nullptr};

};

#endif // WIMMMODEL
